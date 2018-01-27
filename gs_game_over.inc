; gs_game_over.inc
; -----------------------------------------------------------------------------
prepare_game_over:
; -----------------------------------------------------------------------------
  di
  ; Turn off display and frame interrupts.
  call disable_display_and_sound
  ;
  ;
  ld a,ASCII_SPACE
  ld b,TILE_BANK_1
  call reset_name_table
  ;
  ; Load the font tiles.
  SELECT_BANK FONT_BANK
  ld hl,font_table
  call load_vram_from_table
  ;
  SELECT_BANK SPRITE_BANK
  ld bc,sprite_tiles_end-sprite_tiles
  ld de,SPRITE_BANK_START
  ld hl,sprite_tiles
  call load_vram
  ; Set border color.
  ld a,DARK_BLUE
  ld b,REGISTER_7
  call set_register
  ; Wipe sprites.
  call begin_sprites
  call load_sat
  SELECT_BANK GAME_OVER_BANK
  ; Background graphics
  ;ld hl,game_over_background_table
  ;call load_vram_from_table       ; Load the tiles.
  ;call load_vram_from_table       ; Load the tilemap.
  ;
  ; Print "GAME OVER"
  ld b,GAME_OVER_TEXT_ROW
  ld c,GAME_OVER_TEXT_COLUMN
  ld hl,game_over_text
  call print
  ;
  ld hl,transition_trigger_game_over_init_table
  ld ix,transition_trigger
  call initialize_trigger
  ;

  ;
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,REGISTER_1
  call set_register
  ei
  call FadeInScreen
  ;
  call PSGSilenceChannels
  ;
  ; When all is set, change the game state.
  ;
  ld a,GS_RUN_GAME_OVER
  ld (game_state),a
jp main_loop
;
; -----------------------------------------------------------------------------
run_game_over:
; -----------------------------------------------------------------------------
  ; Make sure we start at the first line of vblank.
  -:
    call await_frame_interrupt
    in a,(V_COUNTER_PORT)
    cp FIRST_LINE_OF_VBLANK+1
  jp nz,-
  ;
  call load_sat
  ;
  ; End of (unsafe) VDP-updating...
  ; ---------------------------------------------------------------------------
  call get_input_ports
  call begin_sprites
  ;
  ;
  ;call is_button_1_pressed
  ;jp c,+
  ;call is_button_2_pressed
  ;jp nc,++
  ;  +:
  ;  ld a,GS_PREPARE_HISCORE
  ;  call transition_to_gamestate
    ;jp setup_new_game           ; up a new game.
  ;++:
  ld ix,transition_trigger
  call process_trigger
  jp nc,+
    ld a,GS_PREPARE_HISCORE
    call transition_to_gamestate
  +:
  ;
  ;call PSGFrame
  ;call PSGSFXFrame
  ;
  ld hl,frame_counter
  inc (hl)
  ;
jp main_loop
; -----------------------------------------------------------------------------