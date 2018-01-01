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
    ;
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
    ; Should be called every time we prepare for a new level and with every
    ; Swabby respawn.
    ;
    ld hl,general_triggers_batch_init_table
    ld a,(general_triggers_batch_init_table_end-general_triggers_batch_init_table)/TRIGGER_BATCH_INIT_TABLE_ITEM_SIZE
    call batch_initialize_triggers
    ; -------------------------------------------------------------------------
    ; Depending on difficulty.
    call init_triggers_according_to_difficulty
    ; Set transition score (when to transition to next difficulty).
    ld a,(difficulty)
    ld hl,transition_score_init_table
    call get_word_from_table
    ld de,transition_score
    call copy_score_and_increment_pointers
    ; -------------------------------------------------------------------------
    ; Initialize the various game objects.
    ld a,(batch_game_object_init_table_end-batch_game_object_init_table)/BATCH_INIT_TABLE_ELEMENT_SIZE
    ld hl,batch_game_object_init_table
    call batch_initialize_game_objects
    ; Init swabby death explosion
    ld ix,swabby_death
    ld hl,swabby_death_anim_table
    call load_animation_game_object
    ; Inititalize misc. Swabby dashboard variables.
    ld a,TRUE
    ld (swabby_responds_to_player_input),a
    ld (swabby_can_collide_with_objects),a
    ld a,FALSE
    ld (kill_swabby_flag),a
    ; The spinner-hit-rumble-effect.
    ld a,FLAG_RESET
    ld (rumble_flag),a
    ld ix,rumble_end_trigger
    ld hl,rumble_end_trigger_init_table
    call initialize_trigger
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
    ;
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
; -----------------------------------------------------------------------------
; Metasprites
; -----------------------------------------------------------------------------
; Definitions for referencing the sprites on the spritesheet.
; The astroswablib meta sprite code builds on the sprite routines in bluelib
; and assumes that metasprites are drawn using the specific astroswab approach
; with small, regular, tall and wide sizes.
.equ SPRITE_1 0
.equ SPRITE_2 2
.equ SPRITE_3 8
.equ SPRITE_4 10
.equ SPRITE_5 16
.equ SPRITE_6 18
.equ SPRITE_7 24
.equ SPRITE_8 26
.equ SPRITE_9 32
.equ SPRITE_10 34
.equ SPRITE_11 40
.equ SPRITE_12 42
.equ SPRITE_13 48
.equ SPRITE_14 50
.equ SPRITE_15 56
.equ SPRITE_16 58
.equ SPRITE_17 64
.equ SPRITE_18 66
.equ SPRITE_19 72
.equ SPRITE_20 74
.equ SPRITE_21 80
.equ SPRITE_22 82

.equ SPRITE_23 88
.equ SPRITE_24 90
.equ SPRITE_25 96
.equ SPRITE_26 98
.equ SPRITE_27 104
.equ SPRITE_28 106


; . . .
.equ FIRST_8x8_SPRITE 168
.equ SPRITE_43 FIRST_8x8_SPRITE
.equ SPRITE_44 169
.equ SPRITE_45 170
.equ SPRITE_46 171
.equ SPRITE_47 172
.equ SPRITE_48 173
.equ SPRITE_49 174

.equ SPRITE_50 175
.equ SPRITE_51 176
.equ SPRITE_52 177
.equ SPRITE_53 178
.equ SPRITE_54 179
.equ SPRITE_55 180
.equ SPRITE_56 181
.equ SPRITE_57 182
.equ SPRITE_58 183
.equ SPRITE_59 184
.equ SPRITE_60 185
.equ SPRITE_61 186
.equ SPRITE_62 187
.equ SPRITE_63 188
.equ SPRITE_64 189
.equ SPRITE_65 190
.equ SPRITE_66 191 ; Last 8x8 sprite
; . . . .
.section "metasprites" free
; ---------------------------------------------------------------------------
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
.ends
;
; -----------------------------------------------------------------------------
.section "Empty section" free
; -----------------------------------------------------------------------------
  ; For future use..?!
  ;

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
