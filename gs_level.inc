; gs_level.inc
; .
; ---------------------------------------------------------------------------
; L E V E L                                                        (gameplay)
; ---------------------------------------------------------------------------
; This section contains code for two different game states that handles
; preparing and running a level.
  prepare_level:
    di
    ; Turn off display and frame interrupts.
    ld a,DISPLAY_0_FRAME_0_SIZE_0
    ld b,1
    call set_register
    ; Load a random background depending on difficulty.
    ld a,(difficulty)
    ld b,(difficulty_to_background_bank_table_end-difficulty_to_background_bank_table)/2
    ld hl,difficulty_to_background_bank_table
    call convert_byte
    ld (SLOT_2_CONTROL),a
    ;
    ld a,(difficulty)               ; Get current difficulty as index.
    ld hl,meta_background_table     ; Point HL to base of meta pointer table.
    call get_word_from_table        ; Point HL to base of table of backgrounds.
    push hl                         ; Save this pointer.
      call get_random_number        ; Get random number.
    pop hl                          ; Restore pointer.
    and BACKGROUND_TABLE_MASK       ; Apply mask to random number to get index.
    call get_word_from_table        ; Point HL to random background table at
                                    ; this difficulty.
    ;
    ;
    call load_vram_from_table       ; Load the tiles.
    call load_vram_from_table       ; Load the tilemap.
    ; Print the dummy text under the playfield.
    ld b,DUMMY_TEXT_ROW
    ld c,DUMMY_TEXT_COLUMN
    ld hl,dummy_text
    call print
    ; -------------
    ld hl,gun_level_string
    ld b,3
    call put_string
    ; -------------
    ;
    SELECT_BANK SPRITE_BANK
    ld bc,sprite_tiles_end-sprite_tiles
    ld de,SPRITE_BANK_START
    ld hl,sprite_tiles
    call load_vram
    ;
    call randomize  ; FIXME! Base on player input (titlescreen).
    ; -------------------------------------------------------------------------
    ; Depending on difficulty.
    ld a,(difficulty)                         ; Get current difficulty.
    add a,a                                   ; Word-sized table items.
    ld e,a
    ld d,0
    ld hl,difficulty_to_trigger_init_tables   ; Base of batch init tables.
    add hl,de                                 ; Apply difficulty offset.
    ld a,(hl)                                 ; Read pointer to trigger batch
    inc hl                                    ; init table into HL.
    ld h,(hl)
    ld l,a
    ; Initialize the triggers.
    ld a,(difficulty_0_batch_trigger_init_table_end-difficulty_0_batch_trigger_init_table)/4
    call batch_initialize_triggers
    ;
    ; -------------------------------------------------------------------------
    ; Initialize the various game objects.
    ld a,(batch_game_object_init_table_end-batch_game_object_init_table)/BATCH_INIT_TABLE_ELEMENT_SIZE
    ld hl,batch_game_object_init_table
    call batch_initialize_game_objects
    ; Initialize gun (use the current gun level)
    ld a,(gun_level)
    ld b,a
    ld a,GUN_DELAY_INIT
    sub b
    ld (gun_delay),a
    xor a
    ld (gun_timer),a
    ld a,TRUE
    ld (gun_released),a
    ;
    ; Reset debug meters:
    call reset_vblank_profiler
    ; Wipe sprites.
    call begin_sprites
    call load_sat
    ;
    call PSGSFXStop
    call PSGStop
    ; Turn on screen, frame interrupts and blank left column.
    ld a,DISPLAY_1_FRAME_1_SIZE_0
    ld b,1
    call set_register
    ld a,SCROLL_0__LCB_1_LINE_0_SPRITES_0
    ld b,0
    call set_register
    ei
    call FadeInScreen
    ; When all is set, change the game state.
    ld a,GS_RUN_LEVEL
    ld (game_state),a
    call await_frame_interrupt                ; To avoid profiler failsafe!
    ;
  jp main_loop
  ; ---------------------------------------------------------------------------
  ; ---------------------------------------------------------------------------
  run_level:
    -:
      call await_frame_interrupt
      in a,(V_COUNTER_PORT)
      cp FIRST_LINE_OF_VBLANK+1
    jp nz,-
    call load_sat
    ;
    ld a,(gun_level_flag)         ; Check flag to see if gun level has changed
    cp TRUE                       ; and thus a char update is needed...
    jp nz,+
      ld hl,gun_level_char_data
      call fast_put_char
      ld a,FALSE
      ld (gun_level_flag),a
    +:
    ;
    ; End of VDP-updating...
    .ifdef DEVELOPER_MODE
      call profile_vblank
    .endif
    ;
    call get_input_ports
    call begin_sprites
    ;
    ; ---------------------------------------------------------------------------
    ; Resolve possible bullet collisions:
    ld a,(frame_counter)
    ld ix,bullet
    ld b,BULLET_MAX
    bullet_collision_loop:
      push bc
      ld a,(ix+game_object.state)
      cp GAME_OBJECT_INACTIVE
      jp z,+
        ; Test this bullet instance.
        call detect_collision_using_collision_table
        call c,handle_bullet_collision
      +:
      ld de,_sizeof_game_object
      add ix,de
      pop bc
    djnz bullet_collision_loop
    ; Resolve possible swabby collisions.
    ld ix,swabby
    call detect_collision_using_collision_table
    call c,handle_swabby_collision
    ; End of collision tests for this frame.
    ; -------------------------------------------------------------------------
    call begin_collision_tables       ; Restart filling the coll.tables.
    ;
    ; Handle Swabby sprite and movement:
    ld ix,swabby
    ld a,SWABBY_IDLE_SPRITE           ; Start by resetting sprite to idle.
    ld (ix+game_object.sprite),a
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
    call draw_game_object           ; At last! Draw Swabby at final position.
    ; -------------------------------------------------------------------------
    ; Gun and bullets.
    call is_button_1_pressed        ;
    jp c,+                          ; Is the player pressing the fire button?
      ld a,TRUE                     ; No - then set gun flag (to prevent
      ld (gun_released),a           ; auto fire).
      add a,(hl)
      ld (hl),a
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
    ; Process all bullets.
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
    ; Asteroid generator
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
        ; -----------------------------
          call spawn_game_object_in_invisible_area
          call get_random_number
          and ASTEROID_SPRITE_MASK
          ld hl,asteroid_sprite_table
          ld d,0
          ld e,a
          add hl,de
          ld a,(hl)
          ld (ix+game_object.sprite),a
          call get_random_number
          and ASTEROID_SPEED_MODIFIER
          jp nz,end_asteroid_speed_setting
            inc a
          end_asteroid_speed_setting:
          ld (ix+game_object.yspeed),a
          call activate_game_object
          ; --------------------------------
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
          ld a,ASTEROID_DEACTIVATE_ZONE_START
          ld b,ASTEROID_DEACTIVATE_ZONE_END
          call horizontal_zone_deactivate_game_object
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
    ; ---------------------------------------------------------------------------
    ; Shard generator
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
          ; -----------------------------
          ; TODO? This might be better with a shard sprite table like astero.
          ld a,SHARD_YELLOW_SPRITE
          ld b,a
          call get_random_number
          and %00000011
          add a,b
          ld (ix+game_object.sprite),a
          ; ------------------------------
          call get_random_number
          and SHARD_SPEED_MODIFIER
          inc a
          ld b,SHARD_FREEFALLING_XSPEED
          ld (ix+game_object.yspeed),a
          ld (ix+game_object.xspeed),b
          call activate_game_object
          jp ++                        ; Jump out of loop.
        +:
        ld de,_sizeof_game_object
        add ix,de
        pop bc
      djnz -
    ++:
    ld ix,shard
    ld b,SHARD_MAX
    process_shards:
      push bc
      ld a,(ix+game_object.state)
      cp GAME_OBJECT_ACTIVE
      jp nz,+
        call move_game_object              ; Move shard.
        ld a,SHARD_DEACTIVATE_ZONE_START
        ld b,SHARD_DEACTIVATE_ZONE_END
        call horizontal_zone_deactivate_game_object
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
    ; ---------------------------------------------------------------------------
    ld ix,spinner
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
      ld a,SPINNER_DEACTIVATE_ZONE_START
      ld b,SPINNER_DEACTIVATE_ZONE_END
      ld hl,spinner_hits_ground
      call horizontal_zone_send_to_handler
      ;
      call animate_game_object
      call draw_game_object              ; Put it in the SAT.
      call add_game_object_to_collision_table
    +:
    ; ---------------------------------------------------------------------------
    ; Handle booster and trigger.
    ld ix,booster
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
    ; Handle missile and trigger. -----------------------------------------------
    ld ix,missile
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
      ld a,ASTEROID_DEACTIVATE_ZONE_START
      ld b,ASTEROID_DEACTIVATE_ZONE_END
      ld hl,missile_hits_ground
      call horizontal_zone_send_to_handler
      call draw_game_object              ; Put it in the SAT.
      call add_game_object_to_collision_table
    missile_loop_end:
    ; -------------------------------------------------------------------------
    ;
    call PSGSFXFrame
    call PSGFrame
    ;
    ld hl,frame_counter
    inc (hl)
    .ifdef DEVELOPER_MODE
      call is_reset_pressed
      jp nc,+
        call PSGSFXStop
        call PSGStop
        call FadeOutScreen
        ld a,GS_PREPARE_DEVMENU
        ld (game_state),a
        jp ++
      +:
        ld ix,bee
        call mark_scanline_with_game_object
      ++:
    .endif
  jp main_loop
