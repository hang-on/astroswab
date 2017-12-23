; gs_console.inc
; .
; ---------------------------------------------------------------------------
prepare_console:
; ---------------------------------------------------------------------------
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
  ld b,3
  ld c,0
  ld hl,console_title_string
  call print
  ;
  ld b,CONSOLE_LOGGER_START_ROW
  call reset_logger
  ; ----------------------------------------------------------
  call console_tests
  ; ----------------------------------------------------------
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,1
  call set_register
  ei
  call FadeInScreen
  ; When all is set, change the game state.
  ld a,GS_RUN_CONSOLE
  ld (game_state),a
jp main_loop
  ;
  console_title_string:
    .asc "  Astroswab unit test console#"
; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
run_console:
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
  call is_reset_pressed
  jp nc,+
    ld a,GS_PREPARE_DEVMENU
    call transition_to_gamestate
  +:
jp main_loop
;
; Tests for the console (run-once)
; -----------------------------------------------------------------------------
console_tests:
; -----------------------------------------------------------------------------
  ; call console_test_1
  ; ld a,1
  ; call console_test_2

  ;
ret
; -----------------------------------------------------------------------------
;
; console_test_1:
;
; ret
;
