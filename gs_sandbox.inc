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
  call move_and_draw_bullets
  ;
  ; ---------------------------------------------------------------------------
  ; Spawn and update asteroids.
  ld ix,asteroid_trigger
  call process_trigger ; NB: Save the registers!
  jp nc,+
    ld ix,asteroid
    ld iy,asteroid_respawn_parameters
    call respawn_game_object
  +:
  ;
  ld ix,asteroid
  ld iy,update_asteroid_parameters
  ld de,_sizeof_game_object
  ld b,ASTEROID_MAX
  -:
    call update_game_object
    add ix,de
  djnz -
  ; ---------------------------------------------------------------------------
  ; Spawn and update shards.
  ld ix,shard_trigger
  call process_trigger ; NB: Save the registers!
  jp nc,+
    ld ix,shard
    ld iy,shard_respawn_parameters
    call respawn_game_object
  +:
  ;
  ld ix,shard
  ld iy,update_shard_parameters
  ld de,_sizeof_game_object
  ld b,SHARD_MAX
  -:
    call update_game_object
    add ix,de
  djnz -
  ;
  ;
  call is_reset_pressed
  jp nc,+
    ld a,GS_PREPARE_DEVMENU
    call transition_to_gamestate
  +:

  ;
jp main_loop
;
