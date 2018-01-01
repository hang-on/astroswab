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
  call get_input_ports            ; Get all keys and save them in RAM.
  call begin_sprites              ; Prepare to fill the SAT-buffer.
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
  ;
  ; ---------------------------------------------------------------------------
  ; Resolve possible bullet collisions:
  ld ix,bullet
  ld b,BULLET_MAX
  bullet_collision_loop:
    push bc
      ld a,(ix+game_object.state)
      cp GAME_OBJECT_INACTIVE
      jp z,+
      ld a,(ix+game_object.y)       ; Prevent shooting objects in the invisible
      cp SWABBY_Y_INIT              ; area...
      jp nc,+
        ; Test this bullet instance.
        push ix
          call detect_collision_using_collision_table
          call c,handle_bullet_collision
        pop ix
      +:
      ld de,_sizeof_game_object
      add ix,de
    pop bc
  djnz bullet_collision_loop
  ; Resolve possible swabby collisions.
  ld a,(swabby_can_collide_with_objects)
  cp TRUE
  jp nz,+
    ld ix,swabby
    call detect_collision_using_collision_table
    call c,handle_swabby_collision
  +:
  ; End of collision tests for this frame.
  call begin_collision_tables     ; Restart filling the coll.tables.
  ; -------------------------------------------------------------------------
  ;
  ;
  ;
  ; First, see if the collision tests resulted in flipping the kill switch.
  ld a,(kill_swabby_flag)             ; Test flag - should we kill Swabby?
  cp TRUE
  jp nz,+
    cpl                               ; Toggle the flag (== FALSE).
    ld (kill_swabby_flag),a
    ; Prepare the death animation game object.
    ld ix,swabby_death
    ld hl,swabby_death_anim_table
    call load_animation_game_object
    xor a
    ld (ix+game_object.anim_counter),a
    ld a,GAME_OBJECT_ACTIVE
    ld (ix+game_object.state),a
    ;
    ld iy,swabby_death                ; Position death anim. object where
    ld ix,swabby                      ; Swabby is now.
    call position_game_object_in_relation_to_game_object
    ;
    ld hl,swabby_death_trigger_init_table ; When the timer in this trigger is
    ld ix,swabby_death_trigger            ; up, the death animation object
    call initialize_trigger               ; will be deactivated.
    ld a,ENABLED
    ld (ix+trigger.state),a
    ;
    ; Deativate Swabby and player control.
    ld ix,swabby
    ld a,GAME_OBJECT_INACTIVE
    ld (ix+game_object.state),a
    ld a,FALSE
    ld (swabby_responds_to_player_input),a  ; Swabby cannot be controlled.
    ld (swabby_can_collide_with_objects),a  ; Swabby cannot be hit.
    ;
    ld ix,swabby_respawn_trigger
    call batch_wipe_all_object_triggers_and_enable_trigger
    ;
  +:
  ;
  ;
  ; -------------------------------------------------------------------------
  ; Handle Swabby sprite and movement:
  ld ix,swabby
  ld a,SWABBY_IDLE_SPRITE           ; Start by resetting sprite to idle.
  ld (ix+game_object.sprite),a
  ;
  ld a,(swabby_responds_to_player_input)  ; If this flag is FALSE then Swabby
  cp TRUE                                 ; will remain idle in place, cannot
  jp nz,skip_swabby_input                 ; move or fire.
  ;
    call is_right_pressed             ; Check if player press right.
    ld a,0
    jp nc,+
      ld a,SWABBY_RIGHT_SPRITE        ; Set sprite.
      ld (ix+game_object.sprite),a
      ld a,(ix+game_object.xspeed)
      cp SWABBY_X_SPEED_MAX           ; Check current speed against max speed.
      jp z,++                         ; If we are already there, skip ahead...
        inc a                         ; If not, then accelerate a bit.
        jp ++                         ; Skip over the dpad-left check below.
    +:
    ld a,0
    call is_left_pressed              ; Check if player press left.
    ld a,0
    jp nc,++
      ld a,SWABBY_LEFT_SPRITE
      ld (ix+game_object.sprite),a
      ld a,(ix+game_object.xspeed)
      cp -(SWABBY_X_SPEED_MAX)
      jp z,++
        dec a
    ++:
    ld (ix+game_object.xspeed),a
    call move_game_object
    ;
    ld a,(ix+game_object.x)         ; Get Swabby X (after movement).
    cp SWABBY_X_MAX                 ; Compare it to max X (right border)
    jp c,+                          ; If Swabby has moved beyond this border,
      ld a,SWABBY_X_MAX             ; set his X to max.
      ld (ix+game_object.x),a
      jp ++
    +:
    cp SWABBY_X_MIN                 ; Do the same test for the left border...
    jp nc,++
      ld a,SWABBY_X_MIN
      ld (ix+game_object.x),a
    ++:
    ; -------------------------------------------------------------------------
    ; Gun and bullets.
    call is_button_1_pressed        ;
    jp c,+                          ; Is the player pressing the fire button?
      ld a,TRUE                     ; No - then set gun flag (to prevent
      ld (gun_released),a           ; auto fire).
    +:
    ; Process gun timer.
    ld a,(gun_timer)                ; If gun_timer is not already zero then
    or a                            ; decrement it.
    jp z,+                          ;
      dec a                         ;
      ld (gun_timer),a              ;
    +:                              ;
    call is_button_1_pressed        ; Test for fire button press...
    jp nc,activate_bullet_end       ; If the fire button is not pressed, skip...
      call get_random_number        ; Re-seed random number generator!
      ld a,(gun_timer)              ; Check gun timer (delay between shots).
      or a                          ;
      jp nz,activate_bullet_end     ; If timer not set, skip...
        ld a,(gun_released)         ; Is gun released? (no autofire!)
        cp TRUE                     ;
        jp nz,activate_bullet_end   ; If not, skip...
          ; If we get here, it is time to reset and activate a new bullet.
          ld a,(gun_delay)          ; Make gun wait a little (load time)!
          ld (gun_timer),a          ;
          ld a,FALSE                ; Lock gun (released on fire button release).
          ld (gun_released),a       ;
          ld ix,bullet
          ld a,BULLET_MAX
          call get_inactive_game_object ; Let IX point to first inactive bullet.
          jp c,activate_bullet_end      ; Skip on no inactive bullets (!).
            call activate_game_object
            push ix                     ; Save pointer to bullet.
              ld ix,swabby
              ld a,(ix+game_object.y)   ; Get Swabby y.
              sub BULLET_Y_OFFSET       ; Apply bullet offset.
              ld b,a
              ld a,(ix+game_object.x)   ; Get Swabby x.
              add a,BULLET_X_OFFSET     ; Apply offset.
            pop ix                      ; Restore pointer to bullet.
            ld (ix+game_object.y),b     ; Set bullet y,x in relation to Swabby.
            ld (ix+game_object.x),a
            ;
            call PSGSFXGetStatus
            cp PSG_PLAYING
            jp z,+
              SELECT_BANK SOUND_BANK    ; Select the sound assets bank.
              ld c,SFX_CHANNELS2AND3
              ld hl,shot_1
              call PSGSFXPlay           ; Play the swabby shot sound effect.
            +:
            ;
    activate_bullet_end:              ; End of bullet activation code.
  skip_swabby_input:
  ld ix,swabby
  call draw_game_object           ; At last! Draw Swabby at final position.
  ;
  ;
  ;
  ; ---------------------------------------------------------------------------
  ; Bullets
  ; ---------------------------------------------------------------------------
  ld ix,bullet
  ld b,BULLET_MAX
  -:
    push bc
    ;
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    jp nz,+
      ; Bullet 'collides' with deactivate zone?
      ld a,BULLET_DEACTIVATE_ZONE_START
      ld b,BULLET_DEACTIVATE_ZONE_END
      call horizontal_zone_deactivate_game_object
      call move_game_object
      call draw_game_object
      ;
    +:
    ld de,_sizeof_game_object
    add ix,de
    pop bc
  djnz -
  ;
  ;
  ;
  ; ---------------------------------------------------------------------------
  ; Asteroids
  ; ---------------------------------------------------------------------------
  ld ix,asteroid_trigger
  call process_trigger
  jp nc,++
    ld b,ASTEROID_MAX
    ld ix,asteroid
    -:
      push bc
        ld a,(ix+game_object.state)
        cp GAME_OBJECT_INACTIVE        ; Search for an inactive asteroid.
        jp nz,+
          ;
          call spawn_game_object_in_invisible_area
          call get_random_number
          and ASTEROID_SPRITE_MASK
          ld b,(new_asteroid_sprite_table_end-new_asteroid_sprite_table)/2
          ld hl, new_asteroid_sprite_table
          call convert_byte
          ld (ix+game_object.sprite),a
          call get_random_number
          and ASTEROID_SPEED_MODIFIER
          jp nz,end_asteroid_speed_setting
            inc a
          end_asteroid_speed_setting:
          ld (ix+game_object.yspeed),a
          call activate_game_object
          ;
          jp ++                        ; Jump out of loop.
        +:
        ld de,_sizeof_game_object
        add ix,de
      pop bc
    djnz -
  ++:
  ;
  ld ix,asteroid
  ld b,ASTEROID_MAX
  process_asteroid:
    push bc
      ld a,(ix+game_object.state)
      cp GAME_OBJECT_ACTIVE
      jp nz,+
        call move_game_object
        push ix
          ld a,ASTEROID_DEACTIVATE_ZONE_START
          ld b,ASTEROID_DEACTIVATE_ZONE_END
          ld hl,asteroid_hits_ground
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
  djnz process_asteroid
  ;
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
