; astroswablib.inc
; Functions related specifically to astroswab, or with significant dependencies
; on local/contextual design choices, i.e. works on instants of structs and
; game-specific variables.
;
; -----------------------------------------------------------------------------
.ramsection "Astroswab library variables" slot 3
; -----------------------------------------------------------------------------
  rnd_seed dw         ; Random number generation.
.ends
;
; -----------------------------------------------------------------------------
.section "Misc functions" free
; -----------------------------------------------------------------------------
add_metasprite:
  ; Put a 16x16 metasprite in the SAT.
  ; Entry: IX = Pointer to meta sprite data block, where IX+0 is Y, IX+1 is X
  ;             and IX+2 is the character code.
  ; Uses: AF, BC, DE, HL
  ; Dependencies: add_sprite (in bluelib), and a spritesheet formatted as
  ; 2x21 16x16 images, loaded into SPRITE_BANK_START.
  ld d,4
  ld hl,meta_sprite_table
  -:
    ld a,(ix+0)
    add a,(hl)
    ld b,a
    inc hl
    ld a,(ix+1)
    add a,(hl)
    ld c,a
    inc hl
    ld a,(hl)
    ld e,a
    ld a,(ix+2)
    add a,e
    inc hl
    call add_sprite
    dec d
  jp nz,-
ret
meta_sprite_table:
  .db 0, 0, 0
  .db 0, 8, 1
  .db 8, 0, 4
  .db 8, 8, 5
;
add_tall_metasprite:
  ; Put a 16x32 metasprite in the SAT.
  ; Entry: IX = Pointer to meta sprite data block, where IX+0 is Y, IX+1 is X
  ;             and IX+2 is the character code.
  ; Uses: AF, BC, DE, HL
  ; Dependencies: add_sprite (in bluelib), and a spritesheet formatted as
  ; 2x21 16x16 images, loaded into SPRITE_BANK_START.
  ld d,8
  ld hl,tall_meta_sprite_table
  -:
    ld a,(ix+0)
    add a,(hl)
    ld b,a
    inc hl
    ld a,(ix+1)
    add a,(hl)
    ld c,a
    inc hl
    ld a,(hl)
    ld e,a
    ld a,(ix+2)
    add a,e
    inc hl
    call add_sprite
    dec d
  jp nz,-
ret
tall_meta_sprite_table:
  .db 0, 0, 0
  .db 0, 8, 1
  .db 8, 0, 4
  .db 8, 8, 5
  ;
  .db 16, 0, 6
  .db 16, 8, 7
  .db 24, 0, 10
  .db 24, 8, 11
  ;
  ;
  asteroid_hits_ground:
    ; What happens when an asteroid hits the ground?
    ; Entry: IX points to asteroid object.
    call deactivate_game_object
    ld hl,player_score
    ld a,SCORE_ONES
    ld b,5
    call subtract_from_score
  ret
  ;
  ;
  batch_wipe_all_object_triggers_and_enable_trigger:
    ; Enable specific trigger passed in IX.
    ; Wipe and disable a set list of object respawn triggers (controlling
    ; asteroids, shards, etc.)
    ; Entry: IX = Pointer to trigger to enable.
    ; Exit: None.
    ; Uses:
    .equ SHARD_X_OFFSET_1 (-3)
    .equ SHARD_Y_OFFSET_1 2
    .equ SHARD_X_OFFSET_2 10
    .equ SHARD_Y_OFFSET_2 16
    .equ SHARD_MOVE_LEFT 0
    .equ SHARD_MOVE_RIGHT 1
    .equ SHARD_XSPEED 1
    ;
    ld a,ENABLED
    ld (ix+trigger.state),a
    ;
    ld a,(batch_wipe_and_disable_all_triggers_table_end-batch_wipe_and_disable_all_triggers_table)/4
    ld hl,batch_wipe_and_disable_all_triggers_table
    call batch_initialize_triggers
  ret
  ;
  ;
  detect_and_handle_bullet_collisions:
    ; For each bullet, test it against all other objects in the relevant
    ; collision table. If a collision happens, then run a handler according to
    ; the ID of the colliding object.
    ld ix,bullet
    ld b,BULLET_MAX
    -:
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
    djnz -
  ret
  ;
  ;
  detect_and_handle_swabby_collision:
    ; Test the Swabby game object against all other objects in the relevant
    ; collision table. If a collision is detected, call a handler that performs
    ; different tasks depending on ID of the colliding object.
    ld a,(swabby_can_collide_with_objects)
    cp TRUE
    ret nz
      ld ix,swabby
      call detect_collision_using_collision_table
      call c,handle_swabby_collision
  ret
  ;
  ;
  difficulty_to_random_background:
    ; Load a random background depending on difficulty.
    ; Entry: None. (Uses the difficulty global variable).
    ; Exit: None.
    ; Uses: Everything except index registers.
    ;
    ld a,(difficulty)
    ld b,(difficulty_to_background_bank_table_end-difficulty_to_background_bank_table)/2
    ld hl,difficulty_to_background_bank_table
    call convert_byte
    ld (SLOT_2_CONTROL),a           ; Page-in the correct background bank.
    ;
    ld a,(difficulty)               ; Get current difficulty as index.
    ld hl,meta_background_table     ; Point HL to base of meta pointer table.
    call get_word_from_table        ; Point HL to base of table of backgrounds.
    push hl                         ; Save this pointer.
      call get_random_number        ; Get random number.
    pop hl                          ; Restore pointer.
    and BACKGROUND_TABLE_MASK       ; Apply mask to random number to get index.
    call get_word_from_table        ; Point HL to random background table at
    ;                               ; this difficulty.
    call load_vram_from_table       ; Load the tiles.
    call load_vram_from_table       ; Load the tilemap.
  ret
  ;
  ;
  disable_display_and_sound:
    ; Disable display and frame interrupts.
    ld a,DISPLAY_0_FRAME_0_SIZE_0
    ld b,REGISTER_1
    call set_register
    ; Disable sound.
    call PSGSFXStop
    call PSGStop
  ret
  ;
  ;
  explode_asteroid_into_shards:
    ; Deactivate the asteroid pointed to by IX. Create two shards relative to
    ; the asteroid's position, and transfer the asteroid's yspeed to the two
    ; shards. One shard moves left. The other moves right. Fixed xspeed = 1.
    ; Entry: IX = Pointer to asteroid to explode.
    ; Exit: None.
    ; Uses: AF, BC, HL, IY.
    ld a,GAME_OBJECT_INACTIVE         ; Deactivate the asteroid.
    ld (ix+game_object.state),a
    ;
    push ix                           ; Temporarily store pointer to asteroid
    pop hl                            ; in HL.
    .rept 2                           ; Get two deactivated shards...
      ld ix,shard
      ld a,SHARD_MAX
      call get_inactive_game_object   ; Return with IX = Pointer to shard.
      ld a,GAME_OBJECT_ACTIVE         ; Activate this shard.
      ld (ix+game_object.state),a
      push ix                         ; And store it on the stack.
    .endr                             ; Get one more shard.
    push hl
    pop ix                            ; Restore pointer to asteroid.
    ;
    ld hl,shard_offset_table          ; Point to the shard offset table.
    .rept 2                           ; For both shards, do...
      pop iy                          ; Retrieve pointer to shard from stack.
      ld a,(hl)                       ; Get horizontal direction from table.
      inc hl
      ;
      cp SHARD_MOVE_RIGHT             ; Move downwards and right...?
      jp nz,+
        ld a,(ix+game_object.yspeed)  ; Get asteroid yspeed.
        ld (iy+game_object.yspeed),a  ; Copy asteroid's yspeed to the shard.
        ld a,SHARD_XSPEED             ; Get fixed xspeed.
        ld (iy+game_object.xspeed),a  ; And apply it to the shard.
        jp ++
      +:                              ; Move downwards and left!
        ld a,(ix+game_object.yspeed)  ; Just like above...
        ld (iy+game_object.yspeed),a
        ld a,-(SHARD_XSPEED)          ; Negated xspeed for left movement.
        ld (iy+game_object.xspeed),a
      ++:
      ld b,(hl)                       ; Get y-offset from table
      inc hl
      ld c,(hl)                       ; Get x-offset from table.
      inc hl
      call position_game_object_in_relation_to_game_object
      push hl
        ld a,(ix+game_object.sprite)
        ld b,(shard_conversion_table_end-shard_conversion_table)/2
        ld hl,shard_conversion_table
        call convert_byte
        ld (iy+game_object.sprite),a
      pop hl
    .endr
  ret
    shard_offset_table:
      .db -1, SHARD_Y_OFFSET_1, SHARD_X_OFFSET_1
      .db 1, SHARD_Y_OFFSET_2, SHARD_X_OFFSET_2
      ;
      ;
  FadeInScreen:
    ; (c) Kagesan of SMS-Power.
    halt                   ; wait for Vblank
    ;
    xor a
    out ($bf),a            ; palette index (0)
    ld a,$c0
    out ($bf),a            ; palette write identifier
    ;
    ld b,32                ; number of palette entries: 32 (full palette)
    ld hl,pico8_palette_sms    ; source
 -: ld a,(hl)              ; load raw palette data
    and %00101010          ; modify color values: 3 becomes 2, 1 becomes 0
    srl a                  ; modify color values: 2 becomes 1
    out ($be),a            ; write modified data to CRAM
    inc hl
    djnz -
    ;
    ld b,4                 ; delay 4 frames
 -: halt
    djnz -
    ;
    ld b,32                ; number of palette entries: 32 (full palette)
    ld hl,pico8_palette_sms    ; source
 -: ld a,(hl)              ; load raw palette data
    and %00101010          ; modify color values: 3 becomes 2, 1 becomes 0
    out ($be),a            ; write modified data to CRAM
    inc hl
    djnz -
    ;
    ld b,4                 ; delay 4 frames
 -: halt
    djnz -
    ;
    ld b,32                ; number of palette entries: 32 (full palette)
    ld hl,pico8_palette_sms    ; source
 -: ld a,(hl)              ; load raw palette data
    out ($be),a            ; write unfodified data to CRAM, palette load complete
    inc hl
    djnz -
    ;
  ret
  ;
  ;
  FadeOutScreen:
  ; (c) Kagesan of SMS-Power.
    halt                   ; wait for Vblank
    xor a
    out ($bf),a            ; palette index (0)
    ld a,$c0
    out ($bf),a            ; palette write identifier
    ;
    ld b,32                ; number of palette entries: 32 (full palette)
    ld hl,pico8_palette_sms    ; source
 -: ld a,(hl)              ; load raw palette data
    and %00101010          ; modify color values: 3 becomes 2, 1 becomes 0
    out ($be),a            ; write modified data to CRAM
    inc hl
    djnz -
    ;
    ld b,4                 ; delay 4 frames
 -: halt
    djnz -
    ;
    ld b,32                ; number of palette entries: 32 (full palette)
    ld hl,pico8_palette_sms    ; source
 -: ld a,(hl)              ; load raw palette data
    and %00101010          ; modify color values: 3 becomes 2, 1 becomes 0
    srl a                  ; modify color values: 2 becomes 1
    out ($be),a            ; write modified data to CRAM
    inc hl
    djnz -
    ;
    ld b,4                 ; delay 4 frames
 -: halt
    djnz -
    ;
    ld b, 32               ; number of palette entries: 32 (full palette)
    xor a                  ; we want to blacken the palette, so a is set to 0
 -: out ($be), a           ; write zeros to CRAM, palette fade complete
    djnz -
    ;
  ret
  ;
  ;
  fast_print_score:
    ; Print the digits in a score object to the name table.
    ; Entry: HL = VRAM address.
    ;        IX = Score object.
    ; Exit: None.
    ; Uses: ?
    ;
    ld a,l
    out (CONTROL_PORT),a
    ld a,h
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    push ix
    pop hl
    ld b,SCORE_DIGITS_TOTAL
    -:
      ld a,(hl)
      inc hl
      out (DATA_PORT),a           ; Write it to name table.
      ld a,%00000000              ; Select background palette for this char.
      out (DATA_PORT),a           ; Write 2nd byte to name table.
    djnz -
  ret
  ;
  ;
  get_random_number:
    ; SMS-Power!
    ; Returns an 8-bit pseudo-random number in a
    push hl
    ld hl,(rnd_seed)
    ld a,h         ; get high byte
    rrca           ; rotate right by 2
    rrca
    xor h          ; xor with original
    rrca           ; rotate right by 1
    xor l          ; xor with low byte
    rrca           ; rotate right by 4
    rrca
    rrca
    rrca
    xor l          ; xor again
    rra            ; rotate right by 1 through carry
    adc hl,hl      ; add RandomNumberGeneratorWord to itself
    jr nz,+
      ld hl,$733c  ; if last xor resulted in zero then re-seed.
    +:
    ld a,r         ; r = refresh register = semi-random number
    xor l          ; xor with l which is fairly random
    ld (rnd_seed),hl
    pop hl
  ret              ; return random number in a
  ;
  ;
  halt_and_process_psg:
    ; Skip a specified number of frames while sound is playing.
    ; Entry: A = frames to halt while processing.
    ld b,a
    -:
      halt
      call PSGFrame
      call PSGSFXFrame
    djnz -
  ret
  ;
  ;
  ;
  ;
  handle_bullet_collision:
    ; Function for handling what happens when bullet collides with other
    ; objects.
    ; Entry: IX = bullet.
    ;        IY = game object that collides with bullet.
    ld a,GAME_OBJECT_INACTIVE     ; Start by deactivating both items.
    ld (ix+game_object.state),a
    ld (iy+game_object.state),a
    ;
    ld a,(iy+game_object.id)      ; Do different stuff depending on id.
    cp SHARD                      ; Does bullet collide with a shard?
    jp nz,+                       ; No? - skip forward to next id test.
      ld hl,player_score          ; Yes? - award the player some points.
      ld a,SCORE_TENS             ; FIXME: Varies with difficulty.
      ld b,2
      call add_to_score
      ret
    +:
    cp ASTEROID
    jp nz,+
      push ix
        call get_random_number
        cp ASTEROIDS_INTO_SHARDS_CHANCE
        jp c,skip_explode
          push iy
          pop ix
          call explode_asteroid_into_shards
        skip_explode:
        SELECT_BANK SOUND_BANK
        call PSGSFXStop
        ld hl,explosion
        ld c,SFX_CHANNELS2AND3
        call PSGSFXPlay
        ;
        ld hl,player_score
        ld a,SCORE_TENS
        ld b,1
        call add_to_score

      pop ix
      ret
    +:
    cp SPINNER
    jp nz,+
      call PSGStop
      SELECT_BANK SOUND_BANK
      call PSGSFXStop
      ld hl,explosion
      ld c,SFX_CHANNELS2AND3
      call PSGSFXPlay
      ;
      ld hl,player_score
      ld a,SCORE_TENS
      ld b,4
      call add_to_score
      ;
      ret
    +:
    cp BOOSTER
    jp nz,+
      ; Points and stuff here...
      ld hl,player_score
      ld a,SCORE_TENS
      ld b,5
      call subtract_from_score
      ;
      call PSGGetStatus
      cp PSG_PLAYING
      ret z
        SELECT_BANK SOUND_BANK
        ld hl,shot_booster
        ;ld c,SFX_CHANNELS2AND3
        call PSGPlayNoRepeat
        ret
    +:
    cp MISSILE
    jp nz,+
      ; Points and stuff here...
      ld hl,player_score
      ld a,SCORE_TENS
      ld b,8
      call add_to_score
      ;
      call PSGStop
      SELECT_BANK SOUND_BANK
      call PSGSFXStop
      ld hl,explosion
      ld c,SFX_CHANNELS2AND3
      call PSGSFXPlay
    +:
  ret
  ;
  ;
  handle_swabby_collision:
    ; Placeholder function for handling what happens when swabby collides with
    ; other objects.
    ; IX = swabby, IY other object..
    ld a,GAME_OBJECT_INACTIVE
    ld (iy+game_object.state),a
    ;
    ld a,(iy+game_object.id)
    cp BOOSTER
    jp nz,+
      SELECT_BANK SOUND_BANK
      ld hl,got_booster
      call PSGPlayNoRepeat
      ;
      ld hl,player_score
      ld a,SCORE_HUNDREDS
      ld b,1
      call add_to_score
      ;
      ld a,(gun_level)
      cp GUN_LEVEL_MAX
      ret z
        inc a
        ld (gun_level),a
        ld hl,gun_delay
        dec (hl)
        ld hl,gun_level_char_data+FAST_PUT_CHAR_OFFSET
        inc (hl)
        ld a,TRUE               ; Signal that we should update the counter.
        ld (gun_level_flag),a
        ret
    +:
    cp SPINNER
    jp nz,+
      call start_rumble
      ; Fall through to kill, sound effect etc.
    +:
    ; cp ANOTHER_GAME_OBJECT...
    ;
    ; All other game objects means death to Swabby!
    SELECT_BANK SOUND_BANK
    ld hl,explosion
    ld c,SFX_CHANNELS2AND3
    call PSGSFXPlay
    ;
    ld a,TRUE
    ld (kill_swabby_flag),a
  ret
  ;
  ;
  handle_swabby_death:
    ; First, see if the collision tests resulted in flipping the kill switch.
    ld a,(kill_swabby_flag)             ; Test flag - should we kill Swabby?
    cp TRUE
    ret nz
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
  ret
  ;
  ;
  handle_swabby_gun:
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
    +:
    ld a,(swabby_responds_to_player_input)
    cp TRUE
    ret nz
    ;
    call is_button_1_pressed        ; Test for fire button press...
    ret nc                          ; If the fire button is not pressed, skip...
      call get_random_number        ; Re-seed random number generator!
      ld a,(gun_timer)              ; Check gun timer (delay between shots).
      or a                          ;
      ret nz                        ; If timer not set, skip...
        ld a,(gun_released)         ; Is gun released? (no autofire!)
        cp TRUE                     ;
        ret nz                      ; If not, skip...
          ; If we get here, it is time to reset and activate a new bullet.
          ld a,(gun_delay)          ; Make gun wait a little (load time)!
          ld (gun_timer),a          ;
          ld a,FALSE                ; Lock gun (released on fire button release).
          ld (gun_released),a       ;
          ld ix,bullet
          ld a,BULLET_MAX
          call get_inactive_game_object ; Let IX point to first inactive bullet.
          ret c                         ; Skip on no inactive bullets (!).
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
            ret z
              SELECT_BANK SOUND_BANK    ; Select the sound assets bank.
              ld c,SFX_CHANNELS2AND3
              ld hl,shot_1
              call PSGSFXPlay           ; Play the swabby shot sound effect.
  ret
  ;
  ;
  handle_swabby_sprite_and_movement:
  ; Handle Swabby sprite and movement:
  ld ix,swabby
  ld a,SWABBY_IDLE_SPRITE           ; Start by resetting sprite to idle.
  ld (ix+game_object.sprite),a
  ;
  ld a,(swabby_responds_to_player_input)  ; If this flag is FALSE then Swabby
  cp TRUE                                 ; will remain idle in place, cannot
  jp nz, draw_swabby_and_exit
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
      jp draw_swabby_and_exit
    +:
    cp SWABBY_X_MIN                 ; Do the same test for the left border...
    jp nc,draw_swabby_and_exit
      ld a,SWABBY_X_MIN
      ld (ix+game_object.x),a
    ;
    draw_swabby_and_exit:
      ld ix,swabby
      call draw_game_object           ; Draw Swabby at final position.
    ret
  ;
  ;
  init_triggers_according_to_difficulty:
    ; Depending on the value stored in the variable [difficulty], calculate a
    ; pointer to a relevant batch trigger init table, and use this pointer to
    ; batch initialize some triggers!
    ; Entry: None.
    ; Exit:
    ; Uses:
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
    ; Initialize the difficulty-dependant triggers.
    ld a,(difficulty_0_batch_trigger_init_table_end-difficulty_0_batch_trigger_init_table)/4
    call batch_initialize_triggers
  ret
  ;
  ;
  initialize_variables_once_per_game:
    ; A game consists of Swabby in action until he looses all his lives.
    ; Should be run when preparing the first level (i.e. coming from titlescreen).
    ; Could be divided into NON-VRAM and VRAM?
    ; Variables and structs that are reset once per game.
    ;
    ld a,INITIAL_DIFFICULTY           ; Set difficulty.
    ld (difficulty),a
    ;
    ld a,GUN_LEVEL_INIT               ; Reset gun
    ld (gun_level),a
    ;
    call reset_scores                 ; Reset player and peak score.
  ret
  ;
  ;
  initialize_variables_once_per_gaming_session:
    ; A gaming session consists of one or more games.
    ; Should be run on boot (and possibly on reset?).
    ; Variables and structs that are reset once per gaming session.
    ;    ;
    ld a,FALSE
    ld (pause_flag),a                 ; Reset pause flag.
    ;
    ; The initial game state specified in the header.
    ld a,INITIAL_GAME_STATE
    ld (game_state),a
    ;
    .ifdef DEVELOPER_MODE
      ; Reset debug meters:
      call reset_vblank_profiler
    .endif
  ret
  ;
  ;
  initialize_variables_once_per_life:
    ; Variables and structs that are reset once per life.
    ; Should be called with every Swabby respawn, including the first.
    ;
    ; Init swabby death explosion
    ld ix,swabby_death
    ld hl,swabby_death_anim_table
    call load_animation_game_object
    ;
    ld a,TRUE
    ld (swabby_responds_to_player_input),a
    ld (swabby_can_collide_with_objects),a
    ;
    ld a,FALSE
    ld (kill_swabby_flag),a
    ;
    ld hl,swabby_triggers_batch_init_table
    ld a,(swabby_triggers_batch_init_table_end-swabby_triggers_batch_init_table)/TRIGGER_BATCH_INIT_TABLE_ITEM_SIZE
    call batch_initialize_triggers
    ;
    ; The spinner-hit-rumble-effect.
    xor a
    ld (vscroll_value),a
    ld a,FLAG_RESET
    ld (rumble_flag),a
    ld ix,rumble_end_trigger
    ld hl,rumble_end_trigger_init_table
    call initialize_trigger
    ;
  ret
  ;
  ;
  initialize_variables_once_per_level:
    ;
    ; -------------------------------------------------------------------------
    ; Variables that are set differently depending on difficulty:
    ; Depending on difficulty.
    call init_triggers_according_to_difficulty
    ; Set transition score (when to transition to next difficulty).
    ld a,(difficulty)
    ld hl,transition_score_init_table
    call get_word_from_table
    ld de,transition_score
    call copy_score_and_increment_pointers
    ; -------------------------------------------------------------------------
    ;
    ld ix,transition_trigger
    ld hl,transition_trigger_init_table
    call initialize_trigger
    ;
    ; Reset all game objects (Swabby, asteroids, etc.)
    ld a,(batch_game_object_init_table_end-batch_game_object_init_table)/BATCH_INIT_TABLE_ELEMENT_SIZE
    ld hl,batch_game_object_init_table
    call batch_initialize_game_objects
    ;
    ; Initialize gun (use the current gun level)
    ld a,TRUE
    ld (gun_level_flag),a
    ld a,(gun_level)
    ld b,a
    ld a,GUN_DELAY_INIT
    sub b
    ld (gun_delay),a
    xor a
    ld (gun_timer),a
    ld a,TRUE
    ld (gun_released),a
  ret
  ;
  ;
  load_sprites:
    ; Load the sprite sheet into vram.
    SELECT_BANK SPRITE_BANK
    ld bc,sprite_tiles_end-sprite_tiles
    ld de,SPRITE_BANK_START
    ld hl,sprite_tiles
    call load_vram
  ret
  ;
  ;
  missile_hits_ground:
    ; What happens when a missile hits ground?
    ; Entry: IX points to missile structure.
    call deactivate_game_object
    ;call PSGStop
    SELECT_BANK SOUND_BANK
    ld hl,explosion
    ld c,SFX_CHANNELS2AND3
    call PSGSFXPlay
  ret
  ;
  ;
  move_and_draw_asteroids:
    ;
    ld ix,asteroid
    ld b,ASTEROID_MAX
    -:
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
    djnz -
  ret
  ;
  ;
  move_and_draw_bullets:
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
  ret
  ;
  ;
  print_gun_level:
    ; Print the ASCII version of the current gun level (POW:) to the name table.
    ; If we are at max gun level, print the word defined in gun_level_max_string.
    ; It is set up to be "MAX" but can be changed.
    ; Entry: None.
    ; Exit: None.
    ; Uses: ?
    ;
    ld a,(gun_level)
    cp GUN_LEVEL_MAX
    jp nz,+                     ; If gun max level is reached, print "MAX".
      ld hl,gun_level_max_string
      ld b,((gun_level_max_string_end-gun_level_max_string)-2)/2
      call put_string
      ret
    +:                          ; Else, just print the new gun level.
    ld hl,GUN_LEVEL_NAMETABLE_ADDRESS
    call set_vram_address
    ld a,(gun_level)
    add a,ASCII_ZERO
    out (DATA_PORT),a
    ;
    ld a,FALSE
    ld (gun_level_flag),a
  ret
  ;
  ;
  profile_vblank:
    ; Set debug meter for profiling the amount of lines consumed by functions
    ; operating on the graphics and expecting to work with the screen blanked.
    ; Make sure this meter shows a line number within the vblank period!
    in a,(V_COUNTER_PORT)                   ; Get current line number.
    .ifdef DEVELOPER_MODE
      cp 185                                  ; Failsafe, go into an endless
      jp nc,+                                 ; loop if we are in the active
        -:                                    ; display (both NTSC and PAL)
          nop
        jp -
      +:                                      ; We are somewhere in vblank...
    .endif
    ld b,a                                  ; Store line number in B.
    ld a,(vblank_update_finished_line)      ; Get highest line number yet.
    cp b                                    ; Is the current line higher?
    ret nc                                  ; No, skip forward.
    ld a,b                                  ; Yes, save current line number as
    ld (vblank_update_finished_line),a      ; the new 'high score'.
  ret                                       ;
  ;
  ;
  randomize:
    ; FIXME: Randomize based on player input instead!
    ld hl,randomizer_table
    ld a,(rnd_seed)
    add a,(hl)
    ld (rnd_seed),a
    ld hl,randomizer_table+1
    ld a,(rnd_seed+1)
    add a,(hl)
    ld (rnd_seed+1),a
  ret
    randomizer_table:
      .dbrnd 2,1,254
      ;
      ;
  reset_scores:
    ; Take the player_score_init string (located in the footer) and use it to
    ; overwrite both player score and peak score.
    ld hl,player_score_init
    ld de,player_score
    call copy_score_and_increment_pointers
    ld hl,player_score_init
    ld de,peak_score
    call copy_score_and_increment_pointers
    ld a,FALSE
    ld (new_hiscore),a
  ret
  ;
  ;
  reset_vblank_profiler:
    ld a,FIRST_LINE_OF_VBLANK
    ld (vblank_update_finished_line),a
  ret
  ;
  ;
  setup_level_graphics:
    ; Load various assets into vram and clears SAT.
    ; Entry: None - uses consts and globals.
    ; Exit: None.
    ; Uses: ?
    ;
    call difficulty_to_random_background
    call load_sprites
    ; Set border color.
    ld a,DARK_BLUE
    ld b,REGISTER_7
    call set_register
    ;
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
    SELECT_BANK SPRITE_BANK
    ld bc,sprite_tiles_end-sprite_tiles
    ld de,SPRITE_BANK_START
    ld hl,sprite_tiles
    call load_vram
    ;
    call wipe_sprites
  ret
  ;
  ;
  shard_hits_ground:
    ; IX points to shard object.
    call deactivate_game_object
    ld hl,player_score
    ld a,SCORE_TENS
    ld b,1
    call subtract_from_score
  ret
  ;
  ;
  spinner_hits_ground:
    ; What happens when a flaming asteroid (a so-called spinner) hit ground?
    ; Entry: IX points to spinner structure.
    call deactivate_game_object
    ;
    ld a,(swabby_respawn_trigger.state)
    cp ENABLED
    ret z
    ;
    ld a,TRUE
    ld (kill_swabby_flag),a
    ;
    call start_rumble
    ;
    ld hl,player_score
    ld a,SCORE_HUNDREDS
    ld b,1
    call subtract_from_score
    ;
    SELECT_BANK SOUND_BANK
    ld hl,explosion
    ld c,SFX_CHANNELS2AND3
    call PSGSFXPlay
  ret
  ;
  ;
  start_rumble:
    ; Set the rumble flag and enable the rumble end trigger.
    ld a,FLAG_SET
    ld (rumble_flag),a
    ld a,ENABLED
    ld (rumble_end_trigger.state),a
  ret
  ;
  ;
  transition_to_gamestate:
    ; Fade-out transition to a gamestate passed in A.
    ; Entry: A = Game state.
    ; Exit: None
    ; Uses: ?
    push af
      call PSGSFXStop
      call PSGStop
      call FadeOutScreen
    pop af
    ld (game_state),a
  ret
  ;
  ;
  update_dashboard:
    ; Collection of calls to functions that update/refresh info on the dashboard.
    ; I.e. score, peak score, lives, gun level...
    ; Entry: None.
    ; Exit: None.
    ; Uses: ?
    ;
    ; Gun level.
    ld a,(gun_level_flag)
    cp TRUE
    call z,print_gun_level
    ; Score and peak score.
    ld hl,SCORE_VRAM_ADDRESS
    ld ix,player_score
    call fast_print_score
    ld hl,PEAK_SCORE_VRAM_ADDRESS
    ld ix,peak_score
    call fast_print_score
  ret
  ;
  ;
  wipe_sprites:
    ; Wipe sprites.
    call begin_sprites
    call load_sat
  ret
  ;
  ;
.ends
;
;
;

;
; -----------------------------------------------------------------------------
.section "Empty section" free
; -----------------------------------------------------------------------------

  update_spinner_parameters:
  ; Special handler for when the game object is in a specified horizontal zone?
  .db TRUE
  .dw spinner_hits_ground
  .db SPINNER_GROUND_ZONE_START, SPINNER_GROUND_ZONE_END
  ; Special handler for when the game object is in a specified vertical zone?
  .db FALSE
  .dw $0000
  .db 0, 0
  ; Animate object.
  .db TRUE
  ;
  update_asteroid_parameters:
  ; Special handler for when the game object is in a specified horizontal zone?
  .db TRUE
  .dw asteroid_hits_ground
  .db ASTEROID_GROUND_ZONE_START, ASTEROID_GROUND_ZONE_END
  ; Special handler for when the game object is in a specified vertical zone?
  .db FALSE
  .dw $0000
  .db 0, 0
  ; Animate object
  .db FALSE
  ;
  update_shard_parameters:
  ; Special handler for when the game object is in a specified horizontal zone?
  .db TRUE
  .dw shard_hits_ground
  .db SHARD_GROUND_ZONE_START, SHARD_GROUND_ZONE_END
  ; Special handler for when the game object is in a specified vertical zone?
  .db TRUE
  .dw shard_hits_blanked_column
  .db BLANKED_COLUMN_START, BLANKED_COLUMN_END
  ; animate?
  .db FALSE

  shard_hits_blanked_column:
    call deactivate_game_object
  ret

  within_range_send_to_handler:
    ; Entry: IX = Pointer to struct.
    ;        HL = Pointer to handler.
    ;        A = number. B = Range start. C = Range end.
    cp b
    ret c
    cp c
    ret nc
      jp (hl)
  ret

  update_game_object:
    ; Perform various per-frame actions on an active game object.
    ; IX = Pointer to game object.
    ; IY = Pointer to parameter string.
    ;
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    ret nz
      ;
      ld a,(iy+0)       ; Test game object against horizontal zone?
      cp TRUE
      jp nz,+
        SAVE_REGISTERS
          ld l,(iy+1)
          ld h,(iy+2)
          ld b,(iy+3)
          ld c,(iy+4)
          ld a,(ix+game_object.y)
          call within_range_send_to_handler
        RESTORE_REGISTERS
      +:
      ;
      ld a,(iy+5)
      cp TRUE           ; Test game object against vertical zone?
      jp nz,+
        SAVE_REGISTERS
          ld l,(iy+6)
          ld h,(iy+7)
          ld b,(iy+8)
          ld c,(iy+9)
          ld a,(ix+game_object.x)
          call within_range_send_to_handler
        RESTORE_REGISTERS
      +:
      ld a,(iy+10)
      cp TRUE
      call z,animate_game_object
      ;
      call move_game_object
      call draw_game_object
      call add_game_object_to_collision_table
  ret

  ; abstracted respawn handling...
  ld ix,shard_trigger
  call process_trigger ; NB: Save the registers!
  jp nc,+
    ld ix,shard
    ;ld iy,shard_respawn_parameters
    call c,respawn_game_object
  +:

  respawn_game_object:
    ; Get number of instantations from paramter string...
    ld a,(iy+0)
    call get_inactive_game_object ; NB! Save IY or other relevant registers here
    ret c
      ld l,(iy+1)
      ld h,(iy+2)
      call initialize_game_object
      ld a,(iy+3) ; random yspeed?
      cp TRUE
      jp nz,+
        call get_random_number
        ld l,(iy+4)
        ld h,(iy+5)
        call get_byte_from_table
        ld (ix+game_object.yspeed),a
      +:
      ld a,(iy+6) ; random xspeed?
      cp TRUE
      jp nz,+
        call get_random_number
        ld l,(iy+7)
        ld h,(iy+8)
        call get_byte_from_table
        ld (ix+game_object.xspeed),a
      +:

      ;
      call activate_game_object
      ; alle rnd er tables, a 256 items, både x-pos, (reelt 64 forskellige),
      ; og sprites og speed. Først flash object med initialize., og så parameters,
      ; random x? no (keep init), yes (get from LUT)
  ret
  ;
  asteroid_respawn_parameters:
    .db ASTEROID_MAX
    .dw asteroid_init_table
    .db TRUE  ; random yspeed?
    .dw asteroid_yspeed_table
    .db FALSE ; random xspeed?
    .dw $0000
    .db TRUE ; random horizontal position?
    .dw horizontal_position_table ; pointer to 256 bytes table for rnd xpos.
  ;
  asteroid_yspeed_table:
    .rept 120
      .db 1
    .endr
    .rept 80
      .db 2
    .endr
    .rept 56
      .db 3
    .endr


  ;
  horizontal_position_table:
    .db 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72
    .db 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136
    .db 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196
    .db 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 240, 72, 136, 196, 244

  handle_asteroid_spawning:
  ld ix,asteroid_trigger
  call process_trigger
  ret nc
    ld b,ASTEROID_MAX
    ld ix,asteroid
    -:
      push bc
        ld a,(ix+game_object.state)
        cp GAME_OBJECT_INACTIVE        ; Search for an inactive asteroid.
        jp nz,next_asteroid
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
          jp nz,+
            inc a
          +:
          ld (ix+game_object.yspeed),a
          call activate_game_object
          ;
          pop bc                      ; Align stack.
          ret                         ; Jump out of loop.
        next_asteroid:
        ld de,_sizeof_game_object
        add ix,de
      pop bc
    djnz -
  ret



.ends
;
; ---------------------------------------------------------------------------
.section "Empty section 2" free
; ---------------------------------------------------------------------------
  ;

  ;

.ends
;
;
;
; -----------------------------------------------------------------------------
; SANDBOX AND CONSOLE
; -----------------------------------------------------------------------------
; Work- and testbench. Here we can develop and test new functions before they
; are integrated into the astroswablib or other libraries.
; -----------------------------------------------------------------------------
  ;

  ;
  ;
; -----------------------------------------------------------------------------
.ramsection "Sandbox and console variables" slot 3
  ;
  ;
.ends
;
;
; -----------------------------------------------------------------------------
.section "Sandbox and console functions" free
  ;
  sandbox_function_1:
    ; ...
  ret
  ;
.ends
