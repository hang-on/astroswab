; gs_titlescreen.inc
; -----------------------------------------------------------------------------
prepare_titlescreen:
; -----------------------------------------------------------------------------
  di
  ;
  call disable_display_and_sound
  ;
  call ClearVRam
  ld hl,titlescreen_arrange_vdp_parameters
  call arrange_vdp
  ;
  ; Setup the blinking text.
  ld ix,transition_trigger
  ld hl,transition_trigger_titlescreen_init_table
  call initialize_trigger
  ld ix,blinking_text_trigger
  ld hl,blinking_text_trigger_init_table
  call initialize_trigger
  ;
  ; Un-blank left column (mostly for emulators)
  ld a,SCROLL_0__LCB_0_LINE_0_SPRITES_0
  ld b,REGISTER_0
  call set_register
  ;
  ;
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,REGISTER_1
  call set_register
  ei
  call FadeInScreen
  ;
  ; Stop sound effects.
  call PSGSFXStop
  ;
  ; Play one of two intro tracks.
  SELECT_EXTRAM
    ld a,(EXTRAM_COUNTER)
    inc a
    ld (EXTRAM_COUNTER),a
  SELECT_ROM
    bit 0,a
    jp z,+
      ld hl,mutef
      jp ++
    +:
      ld hl,title_1
    ++:
  SELECT_BANK SOUND_BANK
  call PSGPlay
  ;
  ; When all is set, change the game state.
  ;
  ld a,GS_RUN_TITLESCREEN
  ld (game_state),a
jp main_loop
;
; -----------------------------------------------------------------------------
run_titlescreen:
; -----------------------------------------------------------------------------
  ; Make sure we start at the first line of vblank.
  call await_frame_interrupt
  ;
  call load_sat
  ;
  ; "Press start button" blinking text.
  ld b,20                             ; Row.
  ld c,7                              ; Column.
  ld a,(blinking_text_trigger.cycle)  ; On or off?
  cp 1
  jp z,+
    ld hl,titlescreen_string_1        ; "Press start button".
  jp ++
  +:
    ld hl,titlescreen_string_2        ; Empty string.
  ++:
  call print

  ;
  ; End of (unsafe) VDP-updating...
  ; ---------------------------------------------------------------------------
  call get_input_ports
  call begin_sprites
  ;
  call get_random_number        ; Randomness based on user input.
  ;
  ld ix,blinking_text_trigger
  call process_trigger

  ld ix,transition_trigger
  call process_trigger
  jp nc,+
    ld a,DISABLED
    ld (transition_trigger.state),a
    ld a,GS_PREPARE_HISCORE
    call transition_to_gamestate
  +:
  ;
  ld a,(full_reset_keys_end-full_reset_keys)/2
  ld hl,full_reset_keys
  call test_multiple_keys_AND
  jp nc,+
    ld a,GS_PREPARE_CONFIRM_RESET_DATA
    call transition_to_gamestate
    jp main_loop
  +:
  ;
  ld hl,both_player_1_buttons
  ld a,(both_player_1_buttons_end-both_player_1_buttons)/2
  call test_multiple_keys_OR
  jp nc,+
    ; Toggle secret cheat mode.
    ld a,FALSE
    ld (cheat_mode_enabled),a
    ;
    call is_left_pressed
    jp nc,skip_cheat
      ; Setup cheat mode! Enable a subtle shield that absorbs the hits Swabby
      ; otherwise would have taken. There is an element of semi-randomness:
      ; The greater the shield strength, the higher the chance of an effective
      ; absorbation. The shield strength wears down as it is used.
      ld a,TRUE
      ld (cheat_mode_enabled),a                 ; Set cheat mode flag.
      ld a,CHEAT_MODE_SHIELD_INIT               ; Initialize shield strength.
      ld (cheat_mode_shield),a                  ;
    skip_cheat:
    ld a,(transition_trigger.cycle)
    cp TITLESCREEN_CYCLE_VALUE
    jp z,+
    call initialize_variables_once_per_game
    ld a,GS_PREPARE_LEVEL
    call transition_to_gamestate
  +:
  ;
  ;
  call PSGSFXFrame
  call PSGFrame
  ;
  ld hl,frame_counter
  inc (hl)
  ; Re-seed the random number generator.
  ld a,(frame_counter)
  ld (rnd_seed+1),a
  ld b,a
  ld a,(rnd_seed)
  add a,b
  ld (rnd_seed),a
  ;
jp main_loop
; -----------------------------------------------------------------------------
