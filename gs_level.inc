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
  call initialize_variables_once_per_life
  ;
  call ClearVRam
  ld hl,level_arrange_vdp_parameters
  call arrange_vdp
  call difficulty_to_random_background
  ;
  ; Print the dummy text under the playfield.
  ld b,DUMMY_TEXT_ROW
  ld c,DUMMY_TEXT_COLUMN
  ld hl,dummy_text
  call print
  ; Print the lives meter.
  ld hl,lives_string
  ld b,(lives_string_end-lives_string-2)/2
  call put_string
  ;
  ; Blank left column.
  ld a,SCROLL_0__LCB_1_LINE_0_SPRITES_0
  ld b,REGISTER_0
  call set_register
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
    call PSGSilenceChannels
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
  ; NOTE: Turn into process_swabby?
  call handle_swabby_death
  ld ix,swabby_respawn_trigger
  call process_trigger
  call c,respawn_swabby
  call handle_swabby_sprite_and_movement
  call handle_swabby_gun
  ; -------------------------------------------------------------------------
  ;
  ld ix,bullet
  ld iy,update_bullet_parameters
  ld de,_sizeof_game_object
  ld b,BULLET_MAX
  -:
    call update_game_object
    add ix,de
  djnz -
  ; ---------------------------------------------------------------------------
  ; Spawn and update asteroids.
  ld ix,asteroid_trigger
  call process_trigger ; NB: Save the registers!
  jp nc,+
    ld ix,asteroid
    ld iy,asteroid_respawn_parameters
    call respawn_game_object
  +:
  ; Make sure there is always at least one active asteroid, (if swabby is not
  ; respawning at the moment).
  ld a,(swabby_respawn_trigger.state)
  cp ENABLED
  jp z,+
    ld a,(transition_trigger.state)
    cp ENABLED
    jp z,+
      ld hl,asteroid.state
      ld b,ASTEROID_MAX
      ld de,_sizeof_game_object
      ld a,GAME_OBJECT_ACTIVE
      -:
        cp (hl)                   ; Run through all the asteroid.states...
        jp z,+                    ; Jump out if we find one active asteroid.
          add hl,de
      djnz -
        ; Else, fall through: No active asteroids at the moment.
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
  ld ix,shard
  ld iy,update_shard_parameters
  ld de,_sizeof_game_object
  ld b,SHARD_MAX
  -:
    call update_game_object
    add ix,de
  djnz -
  ;
  ; ---------------------------------------------------------------------------
  ; Spawn and update spinner.
  ld ix,spinner
  ld a,(ix+game_object.state)           ; If spinner is already out, skip!
  cp GAME_OBJECT_ACTIVE
  jp z,+
    ld ix,missile                       ; If missile is already out, skip!
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    jp z,+
      ld ix,spinner_trigger
      call process_trigger
      jp nc,+
          ld ix,spinner
          ld iy,spinner_respawn_parameters
          call respawn_game_object
          jp z,+
            call PSGGetStatus
            cp PSG_PLAYING
            jp z,+
              SELECT_BANK SOUND_BANK    ; Select the sound assets bank.
              ld hl,spinner_coming_down
              call PSGPlayNoRepeat
  +:
  ld ix,spinner
  ld iy,update_spinner_parameters
  call update_game_object
  ;
  ; ---------------------------------------------------------------------------
  ; Spawn and update booster.
  ld ix,booster
  ld a,(ix+game_object.state)           ; If booster is already out, skip!
  cp GAME_OBJECT_ACTIVE
  jp z,+
    ld ix,booster_trigger
    call process_trigger
    jp nc,+
        ld ix,booster
        ld iy,booster_respawn_parameters
        call respawn_game_object
  +:
  ld ix,booster
  ld iy,update_booster_parameters
  call update_game_object
  ;
  ; ---------------------------------------------------------------------------
  ; Spawn  missile.
  ld ix,missile
  ld a,(ix+game_object.state)           ; If missile is already out, skip!
  cp GAME_OBJECT_ACTIVE
  jp z,+
    ld ix,missile_trigger
    call process_trigger
    jp nc,+
        ld ix,missile
        ld iy,missile_respawn_parameters
        call respawn_game_object
        ld a,(ufo.state)
        cp ENABLED
        jp z,+
          SELECT_BANK SOUND_BANK    ; Select the sound assets bank.
          ld hl,missile_warning
          call PSGPlayNoRepeat
  +:
  ;
  ld a,(ix+game_object.y)
  cp MISSILE_TRACKING_LINE
  jp c,+
    ld ix,missile
    ld iy,swabby
    ld a,MISSILE_XSPEED
    call set_game_object_xspeed_depending_on_other_game_object_x
  +:
  ;
  ld ix,missile
  ld iy,update_missile_parameters
  call update_game_object
  ; -------------------------------------------------------------------------
  ;
  ; Spawn UFO.
  ld ix,ufo
  ld a,(ix+game_object.state)           ; If UFO is already out, skip!
  cp GAME_OBJECT_ACTIVE
  jp z,++
    ld ix,ufo_trigger
    call process_trigger
    jp nc,++
        ; Left-to-right or reverse ufo..?
        ld iy,ufo_respawn_parameters
        call get_random_number
        cp PERCENT_CHANCE_50
        jp c,+
          ld iy,reverse_ufo_respawn_parameters
        +:
        ld ix,ufo
        call respawn_game_object
        ld a,ENABLED
        ld (ufo_gun_trigger.state),a
        SELECT_BANK SOUND_BANK
        ld hl,ufo_sound
        call PSGPlay
  ++:
  ;
  ld ix,ufo
  ld iy,update_ufo_parameters
  call update_game_object
  ;
  ld a,(ufo.state)
  cp GAME_OBJECT_ACTIVE
  jp nz,+
    ld ix,ufo_gun_trigger   ; Note: This might be rot! The trigger is always on
    call process_trigger    ; but is only processed when ufo is active...?
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
  ; Is it time to transition to next difficulty?
  ld a,(difficulty)
  cp DIFFICULTY_MAX
  jp z,+
    ld a,(game_state)           ; Rot! If we are actually having a game over in
    cp GS_PREPARE_GAME_OVER     ; this frame!
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
  jp nc,++
  ld a,(difficulty)
  cp DIFFICULTY_MAX
  jp z,+
    inc a
    ld (difficulty),a
    ld a,GS_PREPARE_LEVEL
    call transition_to_gamestate
    call PSGStop
    call PSGSFXStop
    jp ++
  +:
    ; We are here becuase player has maxxed score...
    ld a,GS_PREPARE_GAME_OVER
    call transition_to_gamestate
    call PSGStop
    call PSGSFXStop
  ++:
  ;
  ; Compare to max score
  ld a,(transition_trigger.state)
  cp DISABLED
  jp nz,+
  ld iy,player_score
  ld ix,max_score
  call compare_scores
  jp nc,+
    ;ld a,GS_PREPARE_GAME_OVER
    ;call transition_to_gamestate
    ;call PSGStop
    ;call PSGSFXStop
    ld ix,transition_trigger
    call batch_wipe_all_object_triggers_and_enable_trigger
  +:
  ;
  ;
  call PSGSFXFrame
  call PSGFrame
  ;
  ;
  ld hl,frame_counter
  inc (hl)
  ;
    ; Exit to developer menu if reset button is pressed.
    call is_reset_pressed
    jp nc,+
      .ifdef DEVELOPER_MODE
        call PSGSFXStop
        call PSGStop
        call FadeOutScreen
        ld a,GS_PREPARE_DEVMENU
        ld (game_state),a
      .endif
      ;
      call disable_display_and_sound
      call initialize_variables_once_per_gaming_session
      call reset_scores
      jp main_loop ;!!
    +:
    .ifdef DEVELOPER_MODE
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
    ; ld ix,bee
    ; call mark_scanline_with_game_object
    .endif
jp main_loop
;
;
