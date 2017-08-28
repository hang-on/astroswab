; gs_level.inc
;
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
    ;
    SELECT_BANK BACKGROUND_BANK
    ld a,(level)                          ; Multiply level with 16, because
    rla                                   ; the background table elements are
    rla                                   ; 3+3+2 words wide.
    rla
    rla
    ld d,0
    ld e,a
    ld hl,background_table
    add hl,de
    call load_vram_from_table             ; Load the tiles.
    call load_vram_from_table             ; Load the tilemap.
    ; Print the dummy text under the playfield.
    ld b,DUMMY_TEXT_ROW
    ld c,DUMMY_TEXT_COLUMN
    ld hl,dummy_text
    call print
    ;
    SELECT_BANK SPRITE_BANK
    ld bc,sprite_tiles_end-sprite_tiles
    ld de,SPRITE_BANK_START
    ld hl,sprite_tiles
    call load_vram
    ;
    call randomize  ; FIXME! Base on player input (titlescreen).
    ; -------------------------------------------------------------------------
    ; Initialize the various game objects.
    ld a,(batch_game_object_init_table_end-batch_game_object_init_table)/BATCH_INIT_TABLE_ELEMENT_SIZE
    ld hl,batch_game_object_init_table
    call batch_initialize_game_objects
    ;
    ; Initialize the triggers.
    ld a,(trigger_batch_init_table_end-trigger_batch_init_table)/4
    ld hl,trigger_batch_init_table
    call batch_initialize_triggers
    ;
    ; Initialize gun
    ld a,GUN_DELAY_INIT
    ld (gun_delay),a
    xor a
    ld (gun_timer),a
    ld a,TRUE
    ld (gun_released),a
    ;
    ; Reset debug meters:
    call reset_profilers
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
    ; When all is set, change the game state.
    ld a,GS_RUN_LEVEL
    ld (game_state),a
    call await_frame_interrupt                ; To avoid profiler failsafe!
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
    ; End of VDP-updating...
    ; Set debug meter for profiling the amount of lines consumed by functions
    ; operating on the graphics and expecting to work with the screen blanked.
    ; Make sure this meter shows a line number within the vblank period!
    call profile_vblank
    ;
    call get_input_ports
    call begin_sprites
    ; ---------------------------------------------------------------------------
    ; Resolve current state of objects before moving etc.
    ; Bullets vs. other game objects collision tests:
    ld ix,bullet
    ld bc,BULLET_MAX
    bullet_collision_loop:
      push bc
      ld a,(ix+game_object.state)
      cp GAME_OBJECT_INACTIVE
      jp z,+
        ; Test this bullet instance.
        ; Bullet 'collides' with deactivate zone?
        ld a,BULLET_DEACTIVATE_ZONE_START
        ld b,BULLET_DEACTIVATE_ZONE_END
        call horizontal_zone_deactivate_game_object
        ld a,(ix+game_object.state)
        cp GAME_OBJECT_INACTIVE
        jp z,+
        ;
        ld hl,bullet_collision_table
        ld a,(bullet_collision_table_end-bullet_collision_table)/(2*2)
        call batch_collision_detect
        call c,handle_bullet_collision
      ; Finished testing this bullet. Proceed to next...
      +:
      ld de,_sizeof_game_object
      add ix,de
      pop bc
      dec bc
      ld a,b
      or c
    jp nz,bullet_collision_loop
    ; Swabby vs. relevant other game objects.
    ld ix,swabby
    ld hl,swabby_collision_table
    ld a,(swabby_collision_table_end-swabby_collision_table)/(2*2)
    call batch_collision_detect
    call c,handle_swabby_collision

    ;
    ; ---------------------------------------------------------------------------
    ; Handle Swabby sprite and movement:
    ld ix,swabby
    ld a,SWABBY_IDLE_SPRITE           ; Start by resetting sprite to idle.
    call set_game_object_sprite
    call is_right_pressed             ; Check if player press right.
    ld a,0
    jp nc,+
      ld a,SWABBY_RIGHT_SPRITE        ; Set sprite.
      call set_game_object_sprite
      call get_game_object_xspeed
      cp SWABBY_X_SPEED_MAX           ; Check current speed against max speed.
      jp z,++                         ; If we are already there, skip ahead...
        inc a                         ; If not, then accelerate a bit.
        jp ++                         ; Skip over the dpad-left check below.
    +:
    call is_left_pressed              ; Check if player press left.
    ld a,0
    jp nc,++
      ld a,SWABBY_LEFT_SPRITE
      call set_game_object_sprite
      call get_game_object_xspeed
      cp -(SWABBY_X_SPEED_MAX)
      jp z,++
        dec a
    ++:
    ld b,a
    xor a
    call set_game_object_speed
    call move_game_object
    call draw_game_object
    ; ---------------------------------------------------------------------------
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
            push ix
            ld ix,swabby
            call get_game_object_position
            pop ix
            sub BULLET_Y_OFFSET
            ld c,a
            ld a,b
            add a,BULLET_X_OFFSET
            ld b,a
            ld a,c
            call set_game_object_position
            ;
            SELECT_BANK SOUND_BANK    ; Select the sound assets bank.
            ld c,SFX_CHANNEL2
            ld hl,shot_1
            call PSGSFXPlay           ; Play the swabby shot sound effect.
            ;
    activate_bullet_end:              ; End of bullet activation code.
    ; Process all bullets.
    ld ix,bullet
    ld bc,BULLET_MAX
    bullet_loop:
      push bc
      ;
      call move_game_object
      call draw_game_object
      ;
      ld de,_sizeof_game_object
      add ix,de
      pop bc
      dec bc
      ld a,b
      or c
    jp nz,bullet_loop
    ; ---------------------------------------------------------------------------
    ld ix,asteroid
    ld b,ASTEROID_MAX
    process_asteroids:
      push bc
      call get_game_object_state
      cp GAME_OBJECT_INACTIVE
      jp nz,+
        ld hl,frame_counter                   ; Only consider reactivation on
        bit 0,(hl)                            ; even frames.
        jp nz,+
          call get_random_number
          cp ASTEROID_REACTIVATE_VALUE
          jp nc,+
            ; Activate asteroid.
            call spawn_game_object_in_invisible_area
            call get_random_number
            and ASTEROID_SPRITE_MASK
            ld hl,asteroid_sprite_table
            ld d,0
            ld e,a
            add hl,de
            ld a,(hl)
            call set_game_object_sprite
            call get_random_number
            and ASTEROID_SPEED_MODIFIER
            inc a
            ld b,0
            call set_game_object_speed
            call activate_game_object
      +:
      call move_game_object              ; Move asteroid downwards.
      ; Deactivate asteroid if it is within the deactivate zone.
      ld a,ASTEROID_DEACTIVATE_ZONE_START
      ld b,ASTEROID_DEACTIVATE_ZONE_END
      call horizontal_zone_deactivate_game_object
      ;
      call draw_game_object              ; Put it in the SAT.
      ;
      ld de,_sizeof_game_object
      add ix,de
      pop bc
    djnz process_asteroids
    ; ---------------------------------------------------------------------------
    ; Shard generator
      ld ix,shard_trigger               ;
      call process_trigger
      jp nc,++
        ld b,SHARD_MAX
        ld ix,shard
        -:
          push bc
          call get_game_object_state
          cp GAME_OBJECT_INACTIVE        ; Search for an inactive shard.
          jp nz,+
            call spawn_game_object_in_invisible_area
            ld a,SHARD_YELLOW_SPRITE
            ld b,a
            call get_random_number
            and %00000011
            add a,b
            call set_game_object_sprite
            call get_random_number
            and SHARD_SPEED_MODIFIER
            inc a
            ld b,SHARD_FREEFALLING_XSPEED
            call set_game_object_speed
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
      call move_game_object              ; Move shard.
      ld a,SHARD_DEACTIVATE_ZONE_START
      ld b,SHARD_DEACTIVATE_ZONE_END
      call horizontal_zone_deactivate_game_object
      ;
      call draw_game_object              ; Put it in the SAT.
      ;
      ld de,_sizeof_game_object
      add ix,de
      pop bc
    djnz process_shards
    ; ---------------------------------------------------------------------------
    ld ix,spinner
    call get_game_object_state           ; If spinner is already out, skip!
    cp GAME_OBJECT_ACTIVE
    jp z,+
      ld ix,spinner_trigger               ;
      call process_trigger
      jp nc,+
        ; If spinner_generator_timer is up, do...
        ; Activate a new spinner.
        ld ix,spinner
        call spawn_game_object_in_invisible_area
        ld hl,spinner_anim_table
        call load_animation_game_object
    +:
    ;
    ld ix,spinner
    call move_game_object              ; Move
    ld a,SPINNER_DEACTIVATE_ZONE_START
    ld b,SPINNER_DEACTIVATE_ZONE_END
    call horizontal_zone_deactivate_game_object
    call animate_game_object
    call draw_game_object              ; Put it in the SAT.
    ; ---------------------------------------------------------------------------
    ; Handle danish and trigger.
    ld ix,danish
    call get_game_object_state           ; If danish is already out, skip!
    cp GAME_OBJECT_ACTIVE
    jp z,+
      ld ix,danish_trigger               ;
      call process_trigger
      jp nc,+
        ; If danish_trigger generates a trigger event - activate a new danish.
        ld ix,danish
        call spawn_game_object_in_invisible_area
        call get_random_number
        and DANISH_SPRITE_MASK
        ld hl,danish_sprite_table
        ld d,0
        ld e,a
        add hl,de
        ld a,(hl)
        call set_game_object_sprite
        call activate_game_object
    +:
    ;
    ld ix,danish
    call move_game_object              ; Move
    ld a,ASTEROID_DEACTIVATE_ZONE_START
    ld b,ASTEROID_DEACTIVATE_ZONE_END
    call horizontal_zone_deactivate_game_object
    call draw_game_object              ; Put it in the SAT.
    ; Handle missile and trigger. -----------------------------------------------
    ld ix,missile
    call get_game_object_state           ; If missile is already out, skip!
    cp GAME_OBJECT_ACTIVE
    jp z,+
      ld ix,missile_trigger              ; Only process trigger if it is enabled.
      call get_trigger_state
      cp ENABLED
      jp nz,+
        call process_trigger
        jp nc,+
          ; If missile_generator_timer is up, activate a new missile.
          ld ix,missile
          call spawn_game_object_in_invisible_area
          call activate_game_object
    +:
    ;
    ld ix,missile
    ; TODO: Make missile track/follow Swabby.
    call get_game_object_x
    ; get player x, and then compare 1) somewhere above, 2) right 3) left
    call move_game_object              ; Move
    ld a,ASTEROID_DEACTIVATE_ZONE_START
    ld b,ASTEROID_DEACTIVATE_ZONE_END
    call horizontal_zone_deactivate_game_object
    call draw_game_object              ; Put it in the SAT.
    ; ---------------------------------------------------------------------------
    call PSGSFXFrame
    call PSGFrame
    ;
    ld hl,frame_counter
    inc (hl)
    .ifdef DEVELOPER_MODE
      call is_reset_pressed
      jp nc,+
        call PSGSFXStop
        ld a,GS_PREPARE_DEVMENU
        ld (game_state),a
      +:
      call profile_loop
    .endif
  jp main_loop
