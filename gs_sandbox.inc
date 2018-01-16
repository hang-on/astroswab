; gs_sandbox.inc
; .
; ---------------------------------------------------------------------------
; S A N D B O X
; ---------------------------------------------------------------------------
prepare_sandbox:
  di
  call disable_display_and_sound
  ;
  call randomize
  ;
  call initialize_variables_once_per_game
  call initialize_variables_once_per_level
  call setup_level_graphics
  ;
  call initialize_variables_once_per_life
  ;
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,REGISTER_1
  call set_register
  ;
  ei
  ;
  call FadeInScreen
  ; When all is set, change the game state.
  ld a,GS_RUN_SANDBOX
  ld (game_state),a
jp main_loop
; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
run_sandbox:
  call await_frame_interrupt
  ;
  call load_sat
  ;
  call update_dashboard
  ;
  ; End of VDP-updating...
  call get_input_ports
  call begin_sprites
  ;
  ;
  call handle_swabby_death
  call handle_swabby_sprite_and_movement
  call handle_swabby_gun
  ;
  ;
  ld ix,bullet
  ld iy,update_bullet_parameters
  ld de,_sizeof_game_object
  ld b,BULLET_MAX
  -:
    call update_game_object
    add ix,de
  djnz -
  ;
  ;
  ld ix,ufo
  ld iy,update_ufo_parameters
  call update_game_object
  ;
  ld ix,ufo_gun_trigger
  call process_trigger
  jp nc,+
    ld ix,ufo_bomb
    call get_inactive_game_object
    jp c,+
      ld hl,ufo_bomb_init_table
      call initialize_game_object
      call activate_game_object
      push ix
      pop iy
      ld b,UFO_BOMB_Y_OFFSET
      ld c,UFO_BOMB_X_OFFSET
      ld ix,ufo
      call position_game_object_in_relation_to_game_object
  +:
  ;
  ld ix,ufo_bomb
  ld iy,update_ufo_bomb_parameters
  ld de,_sizeof_game_object
  ld b,UFO_BOMB_MAX
  -:
    call update_game_object
    add ix,de
  djnz -
  ;
  ; Make the ufo shot with player 2 button 1.
  call is_player_2_button_1_pressed
  jp nc,+
    ld a,(ufo_gun_trigger.state)
    cp ENABLED
    jp z,+
      ld a,ENABLED
      ld (ufo_gun_trigger.state),a
  +:
  ;
  call is_reset_pressed
  jp nc,+
    ld a,GS_PREPARE_DEVMENU
    call transition_to_gamestate
  +:

  ;
jp main_loop
;
