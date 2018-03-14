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
  ; Put a metasprite in the SAT.
  ; Entry: IX = Pointer to meta sprite data block, where IX+0 is Y, IX+1 is X
  ;             and IX+2 is the character code.
  ;        HL = Pointer to layout block
  ;         A = Number of hardware sprites in metasprite.
  ; Uses: AF, BC, DE, HL
  ; Dependencies: add_sprite (in bluelib), and a spritesheet formatted as
  ; 2x21 16x16 images, loaded into SPRITE_BANK_START.
  ld d,a
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
;
  regular_meta_sprite_table:
    .db 0, 0, 0
    .db 0, 8, 1
    .db 8, 0, 4
    .db 8, 8, 5
  ;
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
  wide_meta_sprite_table:
    .db 0, 0, 0
    .db 0, 8, 1
    .db 0, 16, 2
    .db 0, 24, 3
    ;
    .db 8, 0, 4
    .db 8, 8, 5
    .db 8, 16, 6
    .db 8, 24, 7
  ;
  ;
  arrange_vdp:
    ; Set registers and load various assets into the VDP
    ; according to switches and parameter data passed in HL.
    ; Swithes (bits in a byte): 000c lwbr
    ; r = Reset name table: char (byte) + tile bank (byte).
    ; b = Set border color: Color
    ; w = Wipe sprites
    ; l = Load one or more blocks of data into vram (tiles, tilemap, etc.).
    ; -------------------------------------------------------------------------
    ; Example parameter string:
      ; .db ALL_SWITCHES_ON               ; Switches.
      ; .db ASCII_SPACE, TILE_BANK_1      ; Wipe tilebank with character.
      ; .db DARK_PURPLE                   ; Border color
      ;
      ; .db 3                             ; Number of blocks to load into vram
      ; .db FONT_BANK                     ; Bank
      ; .dw alternative_font_table        ; Table in the format used by load_vram_from_table.
      ; .db TITLESCREEN_BANK              ; Bank... etc.
      ; .dw titlescreen_tiles_table
      ; .db TITLESCREEN_BANK
      ; .dw titlescreen_tilemap_table
    ; -------------------------------------------------------------------------
    ; Entry:  HL = Pointer to parameter string.
    ; Exit:
    ; Uses: HL, possibly changes bank in slot 2,
    ; Depends on other functions in astroswab.lib.
    ;
    ld a,(hl)                     ; Load the switches into A.
    inc hl
    bit 0,a                       ; Should we reset the name table?
    jp z,+
      push af
        ld a,(hl)
        inc hl
        ld b,(hl)
        inc hl
        call reset_name_table
      pop af
    +:
    bit 1,a                       ; Should we set the border color?
    jp z,+
      push af
        ld a,(hl)
        inc hl
        ld b,REGISTER_7
        call set_register
      pop af
    +:
    bit 2,a                       ; Should we wipe the sprites?
    jp z,+
      push af
      push hl
        call begin_sprites
        call load_sat
      pop hl
      pop af
    +:
    bit 3,a                       ; Should we load some blocks now?
    ret z                         ; Return if not.
      ld b,(hl)                   ; Get amount of blocks from parameters.
      inc hl
      -:
        push bc
          ld a,(hl)                     ; Get bank from parameters.
          inc hl
          SELECT_BANK_IN_REGISTER_A
          ld e,(hl)                     ; Get pointer to vram load table.
          inc hl                        ; LSB...
          ld d,(hl)
          inc hl
          push hl
            ex de,hl
            call load_vram_from_table
          pop hl
        pop bc                          ; Restore loop counter.
      djnz -
  ret
  ;
  ;
  asteroid_hits_ground:
    ; What happens when an asteroid hits the ground?
    ; Entry: IX points to asteroid object.
    call deactivate_game_object
    ld hl,player_score
    ld a,SCORE_TENS
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
  booster_hits_ground:
    ; IX points to object.
    call deactivate_game_object
  ret
  ;
  ;
  bullet_hits_deactivate_zone:
    call deactivate_game_object
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
    SAVE_REGISTERS
    ; Disable display and frame interrupts.
    ld a,DISPLAY_0_FRAME_0_SIZE_0
    ld b,REGISTER_1
    call set_register
    ; Disable sound.
    call PSGSFXStop
    call PSGStop
    call PSGSilenceChannels
    RESTORE_REGISTERS
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
 -:
     push bc
     call PSGFrame
     call PSGSFXFrame
     pop bc
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
 -:
     push bc
     call PSGFrame
     call PSGSFXFrame
     pop bc
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
 -:
    push bc
    call PSGFrame
    call PSGSFXFrame
    pop bc
    halt
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
 -:
 push bc
 call PSGFrame
 call PSGSFXFrame
 pop bc
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
  handle_bullet_collision:
    ; Function for handling what happens when bullet collides with other
    ; objects.
    ; Entry: IX = bullet.
    ;        IY = game object that collides with bullet.
    ld a,GAME_OBJECT_INACTIVE     ; Start by deactivating bullet.
    ld (ix+game_object.state),a
    ;
    ld a,(iy+game_object.id)      ; Do different stuff depending on id.
    cp SHARD                      ; Does bullet collide with a shard?
    jp nz,+                       ; No? - skip forward to next id test.
      ld a,GAME_OBJECT_INACTIVE
      ld (iy+game_object.state),a
      ld hl,player_score          ; Yes? - award the player some points.
      ld a,SCORE_HUNDREDS
      ld b,2
      call add_to_score
      ret
    +:
    cp ASTEROID
    jp nz,+
      ld a,GAME_OBJECT_INACTIVE
      ld (iy+game_object.state),a
      push ix
        call get_random_number
        cp ASTEROIDS_INTO_SHARDS_CHANCE
        jp c,skip_explode
          push iy
          pop ix
          call explode_asteroid_into_shards
        skip_explode:
        call sound_explosion
        ;
        ld hl,player_score
        ld a,SCORE_HUNDREDS
        ld b,1
        call add_to_score
      pop ix
      ret
    +:
    cp SPINNER
    jp nz,+
      ld a,GAME_OBJECT_INACTIVE
      ld (iy+game_object.state),a
      call PSGStop
      SELECT_BANK SOUND_BANK
      call sound_explosion
      ;
      ld hl,player_score
      ld a,SCORE_HUNDREDS
      ld b,4
      call add_to_score
      ;
      ret
    +:
    cp BOOSTER
    jp nz,+
      ld a,GAME_OBJECT_INACTIVE
      ld (iy+game_object.state),a
      ; Points and stuff here...
      ld hl,player_score
      ld a,SCORE_HUNDREDS
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
      ld a,GAME_OBJECT_INACTIVE
      ld (iy+game_object.state),a
      ; Points and stuff here...
      ld hl,player_score
      ld a,SCORE_HUNDREDS
      ld b,8
      call add_to_score
      ;
      call PSGStop
      call sound_explosion
      ret
    +:
    cp UFO
    jp nz,+
      ld a,SPRITE_23
      ld (ufo.sprite),a
      ld a,NOT_MOVING
      ld (ufo.xspeed),a
      ld a,ENABLED
      ld (ufo.timer_state),a
      ; Points and stuff here...
      ld hl,player_score
      ld a,SCORE_THOUSANDS
      ld b,1
      call add_to_score
      ;
      call PSGStop
      call sound_explosion
      ret
    +:
    cp UFO_BOMB
    jp nz,+
      ; Bullet is destroyed but the ufo bombs keeps falling down!
      ret
    +:

  ret
  ;
  ;
  handle_swabby_collision:
    ; Function for handling what happens when Swabby collides with
    ; other objects.
    ; IX = Swabby, IY other object..
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
      ld a,SCORE_THOUSANDS
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
      ld hl,player_score
      ld a,SCORE_HUNDREDS
      ld b,2
      call subtract_from_score
      ;
      call start_rumble
      SELECT_BANK SOUND_BANK
      ld hl,explosion
      ld c,SFX_CHANNELS2AND3
      call PSGSFXPlay
      ;
      ld a,TRUE
      ld (kill_swabby_flag),a
    ret
    +:
    cp MISSILE
    jp nz,+
      ld hl,player_score
      ld a,SCORE_HUNDREDS
      ld b,2
      call subtract_from_score
      ;
      call start_rumble
      SELECT_BANK SOUND_BANK
      ld hl,explosion
      ld c,SFX_CHANNELS2AND3
      call PSGSFXPlay
      ;
      ld a,TRUE
      ld (kill_swabby_flag),a
    ret
    +:
    ; cp ANOTHER_GAME_OBJECT..
    ; All other game objects means death to Swabby!
    ; Except if cheat mode is enabled!
    ;
    ld a,(cheat_mode_enabled)
    cp TRUE
    jp nz,++
      ; Cheat mode is enabled!
      ld a,(cheat_mode_shield)        ; Get shield strength.
      cp CHEAT_MODE_SHIELD_MINIMUM    ; Is is below minimum value?
      jp c,+                          ; Yes ? - skip forward.
        sub CHEAT_MODE_SHIELD_STEP    ; No ? - decrement shield strength.
        ld (cheat_mode_shield),a      ; (The shield wear down effect).
      +:
      ld b,a                          ; Store shield strength in B.
      call get_random_number          ; Roll the dice 0-255.
      cp b                            ; Compare to shield strength.
      ret c                           ; Skip death if dice roll < shield strength.
    ++:
    ; Kill Swabby and deduct from score.
    ld hl,player_score
    ld a,SCORE_HUNDREDS
    ld b,1
    call subtract_from_score
    ;
    call sound_explosion
    ;
    ld a,TRUE
    ld (kill_swabby_flag),a
  ret
  ;
  ;
  handle_swabby_death:
    ld ix,swabby_death_trigger
    call process_trigger
    jp nc,+
      ld a,DISABLED                   ; Switch off trigger.
      ld (ix+trigger.state),a
      ;
      ld ix,swabby_death              ; Remove the death animation object.
      ld a,GAME_OBJECT_INACTIVE
      ld (ix+game_object.state),a
      ; Decrement lives
      ld a,(lives)
      dec a
      ld (lives),a
      cp -(1)
      jp nz,not_game_over
        ld a,GS_PREPARE_GAME_OVER
        call transition_to_gamestate
      not_game_over:
      ld a,FLAG_SET
      ld (lives_flag),a
    +:
    ; Swabby death game object: The death animation (little explosion).
    ld ix,swabby_death
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    jp nz,+
      call animate_game_object
      call draw_game_object              ; Put it in the SAT.
    +:
    ;
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
      ; Stop UFO or other spinner.
      call PSGStop
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
  .db "(c) Hang-On Entertainment 2018"
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
    ;
    ld a,LIVES_INIT
    ld (lives),a
  ret
  ;
  ;
  initialize_variables_once_per_gaming_session:
    ; A gaming session consists of one or more games.
    ; Should be run on boot (and possibly on reset?).
    ; Variables and structs that are reset once per gaming session.
    ;
    call load_sprites
    ;
    ld a,FALSE
    ld (pause_flag),a                 ; Reset pause flag.
    ;
    ; The initial game state specified in the header.
    ; --> if not warning!
    ld a,(game_state)
    cp GS_PREPARE_WARNING
    jp z,+
      ld a,INITIAL_GAME_STATE
      ld (game_state),a
    +:
    ;
    ld ix,ufo_gun_trigger
    ld hl,ufo_gun_trigger_init_table
    call initialize_trigger
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
    ld (lives_flag),a
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
    ; If the player has fiddled with the pause button during break.
    ld a,FALSE
    ld (pause_flag),a                 ; Reset pause flag.
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
  print_lives:
    ld hl,LIVES_NAMETABLE_ADDRESS
    call set_vram_address
    ld a,(lives)
    add a,ASCII_ZERO
    out (DATA_PORT),a
    ;
    ld a,FALSE
    ld (lives_flag),a
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
  respawn_game_object:
    ; IX = Game object (first instance if array).
    ; IY = Pointer to parameter string.
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
      ld a,(iy+9) ; random horizontal position?
      cp TRUE
      jp nz,+
        call get_random_number
        ld l,(iy+10)
        ld h,(iy+11)
        call get_byte_from_table
        ld (ix+game_object.x),a
      +:
      ld a,(iy+12) ; random vertical position?
      cp TRUE
      jp nz,+
        call get_random_number
        ld l,(iy+13)
        ld h,(iy+14)
        call get_byte_from_table
        ld (ix+game_object.y),a
      +:
      ld a,(iy+15) ; random sprite?
      cp TRUE
      jp nz,+
        call get_random_number
        ld l,(iy+16)
        ld h,(iy+17)
        call get_byte_from_table
        ld (ix+game_object.sprite),a
      +:
      ;
      call activate_game_object
  ret
  ;
  ;
  respawn_swabby:
  ; IX = Pointer to trigger
  ; FIXME: Use initialize_variables_once_per_life instead!
    ld a,DISABLED
    ld (ix+trigger.state),a
    ld ix,swabby
    ld hl,swabby_init_table
    call initialize_game_object
    ld a,TRUE
    ld (swabby_responds_to_player_input),a
    ld (swabby_can_collide_with_objects),a
    call init_triggers_according_to_difficulty
  ret
  ;
  ;
  shard_hits_ground:
    ; IX points to shard object.
    call deactivate_game_object
    ld hl,player_score
    ld a,SCORE_HUNDREDS
    ld b,1
    call subtract_from_score
  ret
  ;
  ;
  shard_hits_blanked_column:
    call deactivate_game_object
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
  sound_explosion:
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
  test_multiple_keys_AND:
    ; A = Number of keys.
    ; HL = Pointer to string containing pointers to input test functions.
    ; Uses AF, B, HL
    ld b,a
    -:
      ld e,(hl)
      inc hl
      ld d,(hl)
      inc hl
      ex de,hl
      call hl_
      jp nc,not_all_keys_match
      ex de,hl
    djnz -
    ; All keys match:
      scf
      ret
    not_all_keys_match:
      or a                ; Reset carry flag.
      ret
  ;
  ;
  test_multiple_keys_OR:
    ; A = Number of keys.
    ; HL = Pointer to string containing pointers to input test functions.
    ; Uses AF, B, HL
    ld b,a
    -:
      ld e,(hl)
      inc hl
      ld d,(hl)
      inc hl
      ex de,hl
      call hl_
      jp c,+
      ex de,hl
    djnz -
    ; If we fall through to here, no matching keys was found.
      or a                ; Reset carry flag.
      ret
    +:
      ; At least one key match!
      scf
      ret
  ;
  ;
  transition_to_gamestate:
    ; Fade-out transition to a gamestate passed in A.
    ; Entry: A = Game state.
    ; Exit: None
    ; Uses: ?
    push af
      call FadeOutScreen
    pop af
    ld (game_state),a
    call PSGSFXStop
    call PSGStop

  ret
  ;
  ;
  ufo_hits_blanked_column:
    ld a,(swabby_respawn_trigger.state)
    cp ENABLED
    ret nz
      call deactivate_game_object
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
    ld a,(lives_flag)
    cp TRUE
    call z,print_lives
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
; -----------------------------------------------------------------------------
.section "Empty section" free
; -----------------------------------------------------------------------------
  ;
.ends
;.ramsection "Sandbox and console variables" slot 3
  ;
  ;
;.ends
