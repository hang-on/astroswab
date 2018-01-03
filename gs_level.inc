; gs_level.inc
; This section contains code for two different game states that handles
; preparing and running a level.
;
; ---------------------------------------------------------------------------
prepare_level:
; ---------------------------------------------------------------------------
  di
  call disable_display_and_sound
  ;
  call initialize_variables_once_per_level
  call setup_level_graphics
  call initialize_variables_once_per_life
  ;
  ; Turn on screen, frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,REGISTER_1
  call set_register
  ;
  ei
  ;
  call FadeInScreen
  ; When all is set, change the game state.
  ld a,GS_RUN_LEVEL
  ld (game_state),a
  call await_frame_interrupt                ; To avoid profiler failsafe!
  ;
jp main_loop
;
; -----------------------------------------------------------------------------
run_level:
; -----------------------------------------------------------------------------
  call await_frame_interrupt
  ;
  call load_sat
  ;
  call update_dashboard
  ; ---------------------------------------------------------------------------
  ; The rumble effect when asteroid hits ground.
  ld a,(vscroll_value)      ; Move this rnd out of vblank!
  ld b,VERTICAL_SCROLL_REGISTER
  call set_register
  ;
  ; ---------------------------------------------------------------------------
  ; End of VDP-updating...
  .ifdef DEVELOPER_MODE
    call profile_vblank
  .endif
  ;
  ; Respond to the pause button.
  ld a,(pause_flag)
  cp TRUE
  jp nz,+
    call PSGSFXStop
    call PSGStop
    call PSGFrame
    jp main_loop
  +:
  ;
  call get_input_ports            ; Get all keys and save them in RAM.
  call begin_sprites              ; Prepare to fill the SAT-buffer.
  ;
  ; Resolve possible collisions:
  call detect_and_handle_bullet_collisions  ; FIXME! Remove game object instance from function name
  call detect_and_handle_swabby_collision   ; FIXME! Remove game object instance from function name
  ; End of collision tests for this frame.
  ;
  ; -------------------------------------------------------------------------
  call begin_collision_tables     ; Restart filling the coll.tables.
  ;
  call handle_swabby_death                  ; FIXME! Remove game object instance from function name
  call handle_swabby_sprite_and_movement    ; FIXME! Remove game object instance from function name
  call handle_swabby_gun                    ; FIXME! Remove game object instance from function name
  ;
  call move_and_draw_bullets                ; FIXME! Remove game object instance from function name
  ;
  call handle_asteroid_spawning             ; FIXME! Remove game object instance from function name
  ;
  ld ix,asteroid
  ld iy,update_asteroid_parameters
  ld de,_sizeof_game_object
  ld b,ASTEROID_MAX
  -:
    call update_game_object
    add ix,de
  djnz -
  ;
  ;
  ; ---------------------------------------------------------------------------
  ; Shards
  ; ---------------------------------------------------------------------------
  ld ix,shard_trigger               ;
  call process_trigger
  jp nc,++
    ld b,SHARD_MAX
    ld ix,shard
    -:
      push bc
      ld a,(ix+game_object.state)
      cp GAME_OBJECT_INACTIVE        ; Search for an inactive shard.
      jp nz,+
        call spawn_game_object_in_invisible_area
        ld a,SHARD_YELLOW_SPRITE
        ld b,a
        call get_random_number
        and %00000011
        add a,b
        ld (ix+game_object.sprite),a
        call get_random_number
        and SHARD_SPEED_MODIFIER
        inc a
        ld b,SHARD_FREEFALLING_XSPEED
        ld (ix+game_object.yspeed),a
        ld (ix+game_object.xspeed),b
        call activate_game_object
        ; Fix for exploding shards:
        ld a,SHARD
        ld (ix+game_object.id),a
        jp ++                        ; Jump out of loop.
      +:
      ld de,_sizeof_game_object
      add ix,de
      pop bc
    djnz -
  ++:
  ;

  ld ix,shard
  ld iy,update_shard_parameters
  ld de,_sizeof_game_object
  ld b,SHARD_MAX
  -:
    call update_game_object
    add ix,de
  djnz -

  /*
  ld ix,shard
  ld b,SHARD_MAX
  process_shards:
    push bc
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    jp nz,+
      call move_game_object              ; Move shard.
      push ix
      ld a,SHARD_DEACTIVATE_ZONE_START
      ld b,SHARD_DEACTIVATE_ZONE_END
      ld hl,shard_hits_ground
      call horizontal_zone_send_to_handler
      pop ix
      ;call horizontal_zone_deactivate_game_object
      ld a,VERTICAL_DEACTIVATE_ZONE_START
      ld b,VERTICAL_DEACTIVATE_ZONE_END
      call vertical_zone_deactivate_game_object
      ;
      call draw_game_object              ; Put it in the SAT.
      call add_game_object_to_collision_table
    ;
    +:
    ld de,_sizeof_game_object
    add ix,de
    pop bc
  djnz process_shards

  */
  ;
  ;
  ;
  ; ---------------------------------------------------------------------------
  ld ix,spinner
  ; ---------------------------------------------------------------------------
  ld a,(ix+game_object.state)           ; If spinner is already out, skip!
  cp GAME_OBJECT_ACTIVE
  jp z,+
    ld ix,spinner_trigger               ;
    ld a,(ix+trigger.state)
    cp ENABLED
    jp nz,+
      call process_trigger
      jp nc,+
        ; If spinner_generator_timer is up, do...
        ; Activate a new spinner, if missile is not out!
        ld iy,missile
        ld a,(game_object.state)
        cp GAME_OBJECT_ACTIVE
        jp z,+
          ld ix,spinner
          call spawn_game_object_in_invisible_area
          ld hl,spinner_anim_table
          call load_animation_game_object
          SELECT_BANK SOUND_BANK    ; Select the sound assets bank.
          ld hl,spinner_coming_down
          call PSGPlayNoRepeat
  +:
  ;
  ld ix,spinner
  ld a,(ix+game_object.state)
  cp GAME_OBJECT_ACTIVE
  jp nz,+
    call move_game_object              ; Move
    push ix
      ld a,SPINNER_DEACTIVATE_ZONE_START
      ld b,SPINNER_DEACTIVATE_ZONE_END
      ld hl,spinner_hits_ground
      call horizontal_zone_send_to_handler
    pop ix
    ;
    call animate_game_object
    call draw_game_object              ; Put it in the SAT.
    call add_game_object_to_collision_table
  +:
  ;
  ;
  ;
  ; -------------------------------------------------------------------------
  ; Swabby death
  ; ---------------------------------------------------------------------------
  ld ix,swabby_death_trigger
  call process_trigger
  jp nc,+
    ld a,DISABLED                   ; Switch off trigger.
    ld (ix+trigger.state),a
    ;
    ld ix,swabby_death              ; Remove the death animation object.
    ld a,GAME_OBJECT_INACTIVE
    ld (ix+game_object.state),a
  +:
  ;
  ; Swabby death game object: The death animation (little explosion).
  ld ix,swabby_death
  ld a,(ix+game_object.state)
  cp GAME_OBJECT_ACTIVE
  jp nz,+
    ;
    call animate_game_object
    call draw_game_object              ; Put it in the SAT.
  +:
  ;
  ;
  ;
  ; ---------------------------------------------------------------------------
  ld ix,booster
  ; ---------------------------------------------------------------------------
  ld a,(ix+game_object.state)          ; If booster is already out, skip!
  cp GAME_OBJECT_ACTIVE
  jp z,+
    ld ix,booster_trigger               ;
    ld a,(ix+trigger.state)
    cp ENABLED
    jp nz,+
      call process_trigger
      jp nc,+
        ; If booster_trigger generates a trigger event - activate a new booster.
        ld ix,booster
        call spawn_game_object_in_invisible_area
        call get_random_number
        and BOOSTER_SPRITE_MASK
        ld hl,booster_sprite_table
        ld d,0
        ld e,a
        add hl,de
        ld a,(hl)
        ld (ix+game_object.sprite),a
        call activate_game_object
      +:
  ;
  ld ix,booster
  ld a,(ix+game_object.state)
  cp GAME_OBJECT_ACTIVE
  jp nz,+
    call move_game_object              ; Move
    ld a,ASTEROID_DEACTIVATE_ZONE_START
    ld b,ASTEROID_DEACTIVATE_ZONE_END
    call horizontal_zone_deactivate_game_object
    call draw_game_object              ; Put it in the SAT.
    call add_game_object_to_collision_table
  +:
  ;
  ;
  ;
  ; ---------------------------------------------------------------------------
  ld ix,missile
  ; ---------------------------------------------------------------------------
  ld a,(ix+game_object.state)          ; If missile is already out, skip!
  cp GAME_OBJECT_ACTIVE
  jp z,+
    ld ix,missile_trigger
    ld a,(ix+trigger.state)
    cp ENABLED
    jp nz,+
      call process_trigger
      jp nc,+
        ; If missile_generator_timer is up, activate a new missile.
        ld ix,missile
        call spawn_game_object_in_invisible_area
        call activate_game_object
        SELECT_BANK SOUND_BANK    ; Select the sound assets bank.
        ld hl,missile_warning
        call PSGPlayNoRepeat
  +:
  ;
  ld ix,missile
  ld a,(ix+game_object.state)
  cp GAME_OBJECT_ACTIVE
  jp nz,missile_loop_end
    ld a,(ix+game_object.y)
    cp MISSILE_TRACKING_LINE
    jp c,++
    ; Make missile track/follow Swabby.
    ld iy,swabby
    ld a,(iy+game_object.x)
    ld b,(ix+game_object.x)
    cp b
    jp z,++
    jp c,+
      ld a,MISSILE_XSPEED       ; Move missile right.
      jp +++
    +:
      ld a,-(MISSILE_XSPEED)    ; Move missile left.
    jp +++
  ++:                           ; Missile directly above Swabby.
    xor a                       ; Don't move missile horizontally.
  +++:
    ld (ix+game_object.xspeed),a
    call move_game_object
    push ix
      ld a,ASTEROID_DEACTIVATE_ZONE_START
      ld b,ASTEROID_DEACTIVATE_ZONE_END
      ld hl,missile_hits_ground
      call horizontal_zone_send_to_handler
    pop ix
    call draw_game_object              ; Put it in the SAT.
    call add_game_object_to_collision_table
  missile_loop_end:
  ; -------------------------------------------------------------------------
  ;
  ;
  ;
  ; Update peak score as necessary.
  ld iy,player_score
  ld ix,peak_score
  call compare_scores
  jp nc,+
    ld hl,player_score
    ld de,peak_score
    call copy_score_and_increment_pointers
  +:
  ;
  ; Process the respawn trigger.
  ; FIXME: Use initialize_variables_once_per_life instead!
  ld ix,swabby_respawn_trigger
  call process_trigger
  jp nc,+
    ld a,DISABLED
    ld (ix+trigger.state),a
    ld ix,swabby
    ld hl,swabby_init_table
    call initialize_game_object
    ld a,TRUE
    ld (swabby_responds_to_player_input),a
    ld (swabby_can_collide_with_objects),a
    call init_triggers_according_to_difficulty
  +:
  ;
  ; Rumble effect.
  ld ix,rumble_end_trigger
  call process_trigger
  jp nc,+
    ld a,DISABLED
    ld (ix+trigger.state),a
    ld a,FLAG_RESET
    ld (rumble_flag),a
    xor a
    ld (vscroll_value),a
  +:
  ;
  ld a,(rumble_flag)
  cp FLAG_SET
  jp nz,+
    call get_random_number
    and %00000011
    add a,b
    ld (vscroll_value),a
  +:
  ;
  ;
  ;
  ; Is it time to transition to next difficulty?
  ld a,(difficulty)
  cp DIFFICULTY_MAX
  jp z,+
    ld iy,peak_score
    ld ix,transition_score
    call compare_scores
    jp nc,+
      ld ix,transition_trigger
      call batch_wipe_all_object_triggers_and_enable_trigger
  +:
  ;
  ; Process transition to next difficulty timer.
  ld ix,transition_trigger
  call process_trigger
  jp nc,+
  ld a,(difficulty)
  cp DIFFICULTY_MAX
  jp z,+
    inc a
    ld (difficulty),a
    ld a,GS_PREPARE_LEVEL
    call transition_to_gamestate
  +:
  ;
  ;
  ;
  call PSGSFXFrame
  call PSGFrame
  ;
  ld hl,frame_counter
  inc (hl)
  ;
  .ifdef DEVELOPER_MODE
    ; Exit to developer menu if reset button is pressed.
    call is_reset_pressed
    jp nc,+
      call PSGSFXStop
      call PSGStop
      call FadeOutScreen
      ld a,GS_PREPARE_DEVMENU
      ld (game_state),a
      jp ++
    +:
    ; Press player 2 button 1 to stop enemy objects spawning.
    call is_player_2_button_1_pressed
    jp nc,+
      ld a,(difficulty)
      cp DIFFICULTY_MAX
      jp z,+
        ld ix,transition_trigger
        call batch_wipe_all_object_triggers_and_enable_trigger
    +:
    ; Put the profiler bee in the SAT.
    ld ix,bee
    call mark_scanline_with_game_object
    ++:
  .endif
jp main_loop
;
;
