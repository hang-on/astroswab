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
  SELECT_BANK HISCORE_BANK
  ; Background graphics
  ld hl,hiscore_background_table
  call load_vram_from_table       ; Load the tiles.
  call load_vram_from_table       ; Load the tilemap.

  ; Stop music and sound effects.
  call PSGSFXStop
  call PSGStop
  ;
  ;
  ld hl,HISCORE_EXTRAM_ADDRESS
  call load_hiscore_table_from_extram
  ;
  ld hl,peak_score                      ; Rebuild the hiscore table againt the
  call rebuild_hiscore_table            ; player's peak score.
  ;
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
  ; TODO: Add two modes - Read only and write (if player enters the table).
  ; Add If/else here...
  ; Read-only mode (view hiscore list until timer depletes).


  ; Write mode (wait for player to enter initials and confirm with button).
  ;
  ; Save hiscore table and exit when player press button.
  call is_button_1_pressed
  jp nc,+
    ld hl,HISCORE_EXTRAM_ADDRESS
    call save_hiscore_table_to_extram
    ;
    call PSGSFXStop
    call PSGStop
    call FadeOutScreen
    ld a,GS_PREPARE_DEVMENU
    ld (game_state),a
  +:


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
