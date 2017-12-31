; gs_sandbox.inc
; .
; ---------------------------------------------------------------------------
; S A N D B O X
; ---------------------------------------------------------------------------
prepare_sandbox:
  di
  ; Turn off display and frame interrupts.
  ld a,DISPLAY_0_FRAME_0_SIZE_0
  ld b,REGISTER_1
  call set_register
  ;
  ; Initialize variables.
  ld a,INITIAL_DIFFICULTY           ; Set difficulty.
  ld (difficulty),a
  ld a,TRUE
  ld (gun_level_flag),a
  ld a,GUN_LEVEL_INIT               ; Reset gun
  ld (gun_level),a
  ;
  call setup_level_graphics
  ; Wipe sprites.
  call begin_sprites
  call load_sat
  ; Stop music and sound effects.
  call PSGSFXStop
  call PSGStop
  ; ----------------------------------------------------------
  call init_tests
  ; ----------------------------------------------------------
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,1
  call set_register
  ei
  call FadeInScreen
  ; When all is set, change the game state.
  ld a,GS_RUN_SANDBOX
  ld (game_state),a
jp main_loop
; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
run_sandbox:
  ;
  -:
    call await_frame_interrupt
    in a,(V_COUNTER_PORT)
    cp FIRST_LINE_OF_VBLANK+1
  jp nz,-
  ;
  call load_sat
  ;
  ld a,(gun_level_flag)
  cp TRUE
  call z,print_gun_level
  ;
  ; End of VDP-updating...
  call get_input_ports
  call begin_sprites
  ;
  ;
  ; Tests below:
  ;call test_rebuild_hiscore_table_against_fine_score
  ;call test_rebuild_against_fine_and_print
  ;
  call is_reset_pressed
  jp nc,+
    ld a,GS_PREPARE_DEVMENU
    call transition_to_gamestate
  +:

  ;
jp main_loop
;
; Tests for the sandbox:
; -----------------------------------------------------------------------------
init_tests:
; -----------------------------------------------------------------------------
  ; Tests below:
  call initialize_variables_once_per_game
  ;
ret
; -----------------------------------------------------------------------------
; Update() tests:
