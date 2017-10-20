; gs_hiscore.inc
; ---------------------------------------------------------------------------
; H I S C O R E
; ---------------------------------------------------------------------------
prepare_hiscore:
  di
  ; Turn off display and frame interrupts.
  ld a,DISPLAY_0_FRAME_0_SIZE_0
  ld b,1
  call set_register
  ;
  ld a,ASCII_SPACE
  ld b,TILE_BANK_1
  call reset_name_table
  ;
  SELECT_BANK SPRITE_BANK
  ld bc,sprite_tiles_end-sprite_tiles
  ld de,SPRITE_BANK_START
  ld hl,sprite_tiles
  call load_vram
  ; Wipe sprites.
  call begin_sprites
  call load_sat
  ; Stop music and sound effects.
  call PSGSFXStop
  call PSGStop
  ;
  ; Build, and write to VRAM, the hiscore table.
  ; TODO: Get rid of the demo add/sub and dummy table. It should be loaded
  ; directly from EXTRAM.
  ;ld hl,multe_score_init
  ;ld de,player_score
  ;call copy_score_and_increment_pointers
  ;ld hl,player_score
  ;ld a,SCORE_HUNDREDS
  ;ld b,1
  ;call add_to_score
  ;ld hl,player_score
  ;ld a,SCORE_TENS
  ;ld b,3
  ;call subtract_from_score
  ;
  ld hl,hiscore_init
  ld de,hiscore_item.1
  call copy_hiscore_table
  ;
  ld hl,player_score
  call rebuild_hiscore_table
  ;
  ld a,HISCORE_TABLE_ROW
  ld b,HISCORE_TABLE_COLUMN
  call print_hiscore_table
  ;
  ;
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,1
  call set_register
  ei
  call FadeInScreen
  ; When all is set, change the game state.
  ld a,GS_RUN_HISCORE
  ld (game_state),a
jp main_loop
;
; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
run_hiscore:
  ;
  -:
    call await_frame_interrupt
    in a,(V_COUNTER_PORT)
    cp FIRST_LINE_OF_VBLANK+1
  jp nz,-

  call load_sat
  ; End of VDP-updating...
  call get_input_ports
  call begin_sprites
  ;
  ;
  call PSGSFXFrame
  call PSGFrame
  ;
  ld hl,frame_counter
  inc (hl)
  .ifdef DEVELOPER_MODE
    call is_reset_pressed
    jp nc,+
      call PSGSFXStop
      call PSGStop
      call FadeOutScreen
      ld a,GS_PREPARE_DEVMENU
      ld (game_state),a
    +:
  .endif

jp main_loop
