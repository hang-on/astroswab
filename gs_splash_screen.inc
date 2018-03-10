; gs_splash_screen.inc
; -----------------------------------------------------------------------------
prepare_splash_screen:
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
  ld hl,alternative_font_table
  call load_vram_from_table
  ;
  SELECT_BANK SPRITE_BANK
  ld bc,sprite_tiles_end-sprite_tiles
  ld de,SPRITE_BANK_START
  ld hl,sprite_tiles
  call load_vram
  ; Set border color.
  ld a,DARK_PURPLE
  ld b,REGISTER_7
  call set_register
  ; Wipe sprites.
  call begin_sprites
  call load_sat
  ;
  ; Print splash screen message
  ld a,(splash_batch_print_table_end-splash_batch_print_table)/SIZE_OF_BATCH_PRINT_ITEM
  ld ix,splash_batch_print_table
  call batch_print
  ;
  ld hl,transition_trigger_splash_screen_init_table
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
  ld a,GS_RUN_SPLASH_SCREEN
  ld (game_state),a
jp main_loop
;
; -----------------------------------------------------------------------------
run_splash_screen:
; -----------------------------------------------------------------------------
  ; Make sure we start at the first line of vblank.
  call await_frame_interrupt
  ;
  call load_sat
  ;
  ; End of (unsafe) VDP-updating...
  ; ---------------------------------------------------------------------------
  call get_input_ports
  call begin_sprites
  ;
  ld ix,transition_trigger
  call process_trigger
  jp nc,+
    ld a,GS_PREPARE_TITLESCREEN
    ld (game_state),a
    ;call transition_to_gamestate
  +:
  ;
  ld hl,frame_counter
  inc (hl)
  ;
jp main_loop
; -----------------------------------------------------------------------------
