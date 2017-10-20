; astroswablib.inc
; Functions related specifically to astroswab, or with significant dependencies
; on local/contextual design choices.
;
; -----------------------------------------------------------------------------
.section "Misc functions" free
; -----------------------------------------------------------------------------
  ;
  ;=================================================================================
  ; Quick palette fade
  ;=================================================================================
  ; (c) Kagesan of SMS-Power.
  FadeInScreen:
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
  ;---------------------------------------------------------------------------------
  FadeOutScreen:
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
.ends
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
.section "Game object implementations" free
; -----------------------------------------------------------------------------
  spinner_hits_ground:
      ; IX points to spinner structure.
      call deactivate_game_object
      SELECT_BANK SOUND_BANK
      ld hl,explosion
      ld c,SFX_CHANNELS2AND3
      call PSGSFXPlay
  ret
  ;
  missile_hits_ground:
      ; IX points to missile structure.
      call deactivate_game_object
      ;call PSGStop
      SELECT_BANK SOUND_BANK
      ld hl,explosion
      ld c,SFX_CHANNELS2AND3
      call PSGSFXPlay
  ret
  ;
  handle_bullet_collision:
    ; Placeholder function for handling what happens when bullet collides with
    ; other objects.
    ; TODO: Increment score depending on other object's id.
    ; IX = bullet, IY other object..
    ld a,GAME_OBJECT_INACTIVE
    ld (ix+game_object.state),a
    ld (iy+game_object.state),a
    ;
    ;
    ld a,(iy+game_object.id)
    cp SHARD
    jp nz,+
      ; Do nothing with shards...
      ret
    +:
    cp ASTEROID
    jp nz,+
      push ix
        push iy
        pop ix
        call explode_asteroid_into_shards
        SELECT_BANK SOUND_BANK
        call PSGSFXStop
        ld hl,explosion
        ld c,SFX_CHANNELS2AND3
        call PSGSFXPlay
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
      ret
    +:
    cp BOOSTER
    jp nz,+
      ; Points and stuff here...
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
      call PSGStop
      SELECT_BANK SOUND_BANK
      call PSGSFXStop
      ld hl,explosion
      ld c,SFX_CHANNELS2AND3
      call PSGSFXPlay
    +:

  ret
  handle_swabby_collision:
    ; Placeholder function for handling what happens when swabby collides with
    ; other objects.
    ; TODO: Implement DEATH!
    ; IX = swabby, IY other object..
    ld a,GAME_OBJECT_INACTIVE
    ld (iy+game_object.state),a
    ;
    ld a,(iy+game_object.id)
    cp BOOSTER
    jp nz,+
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
        SELECT_BANK SOUND_BANK
        ld hl,got_booster
        call PSGPlayNoRepeat
        ret
    +:
    ; cp ANOTHER_GAME_OBJECT...
  ret
  ;
  .equ SHARD_X_OFFSET_1 (-3)
  .equ SHARD_Y_OFFSET_1 2
  .equ SHARD_X_OFFSET_2 10
  .equ SHARD_Y_OFFSET_2 16
  .equ SHARD_MOVE_LEFT 0
  .equ SHARD_MOVE_RIGHT 1
  .equ SHARD_XSPEED 1
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

.ends
;
; ---------------------------------------------------------------------------
.section "vblank profiler" free
; ---------------------------------------------------------------------------
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
  reset_vblank_profiler:
    ld a,FIRST_LINE_OF_VBLANK
    ld (vblank_update_finished_line),a
  ret
.ends
;
; -----------------------------------------------------------------------------
.ramsection "random number generation" slot 3
; -----------------------------------------------------------------------------
  rnd_seed dw
.ends
; -----------------------------------------------------------------------------
.section "random number generation functions" free
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
  ;
  randomizer_table:
    .dbrnd 2,1,254
.ends
; -----------------------------------------------------------------------------
; SANDBOX
; -----------------------------------------------------------------------------
; Work- and testbench.
  .equ SANDBOX_LOGGER_START_ROW 19
  ; ---
  ; Score and hiscore related definitions.
  .equ SCORE_ONES 4 ; offsets ind the struct
  .equ SCORE_TENS 3
  .equ SCORE_HUNDREDS 2
  .equ SCORE_THOUSANDS 1
  .equ SCORE_TEN_THOUANDS 0
  .equ SCORE_DIGITS_TOTAL 5
  .equ LETTERS_IN_INITIALS 3
  ;
  .struct score_struct
    ; A five-digit score struct.
    ; Depends on ascii 0-9 set somewhere with asciitable mapping, and also the
    ; constant ASCII_ZERO.
    ten_thousands db
    thousands db
    hundreds db
    tens db
    ones db
    initials dsb LETTERS_IN_INITIALS
  .endst
  ;
  .equ NUMBER_OF_HISCORE_ITEMS 5
  .equ HISCORE_TABLE_ROW 4
  .equ HISCORE_TABLE_COLUMN 8
  ;
; -----------------------------------------------------------------------------
.ramsection "Sandbox variables" slot 3
; -----------------------------------------------------------------------------
  ;
  player_score instanceof score_struct
  ;
  hiscore_item instanceof score_struct NUMBER_OF_HISCORE_ITEMS
  hiscore_workbench instanceof score_struct
  hiscore_table_buffer dsb (_sizeof_score_struct*NUMBER_OF_HISCORE_ITEMS)
  hiscore_table_pointer dw
  hiscore_table_buffer_pointer dw
  ;
.ends
;
.section "Sandbox functions" free
  ; Short text here...
  sandbox_function_1:
    ; ...
  ret
  ;
  print_hiscore_table:
    ; Print the hiscore table starting at a specified (row,column).
    ; Entry: A = Row (vertical).
    ;        B = Column (horizontal).
    ; Exit: None.
    ; Uses: AF, BC, DE, HL, IX
    ;
    ld ix,hiscore_item.1
    ld c,NUMBER_OF_HISCORE_ITEMS
    ld de,_sizeof_score_struct
    -:
      call print_one_item
      inc a
      add ix,de
      ;
      dec c
    jp nz,-
  ret
    ;
    print_one_item:
      SAVE_REGISTERS
      push bc
        call set_cursor
        push ix
        pop hl
        ld b,SCORE_DIGITS_TOTAL
        call put_string_at_cursor
      pop bc
      push af
        ld a,SCORE_DIGITS_TOTAL
        add a,b
        ld b,a
      pop af
      push bc
        call set_cursor
        ld hl,hiscore_item_spacer
        ld b,HISCORE_ITEM_SPACER_SIZE
        call put_string_at_cursor
      pop bc
      push af
        ld a,HISCORE_ITEM_SPACER_SIZE
        add a,b
        ld b,a
      pop af
      call set_cursor
      push ix
      pop hl
      ld de,score_struct.initials
      add hl,de
      ld b,LETTERS_IN_INITIALS
      call put_string_at_cursor
      RESTORE_REGISTERS
      ret
      ;
    .equ HISCORE_ITEM_SPACER_SIZE 5
    hiscore_item_spacer:
      .asc "....."
  ;
  rebuild_hiscore_table:
    ; Rebuilds the hiscore table against a new score item pointed to by HL.
    ; If the new score is high enough to be somewhere on the hiscore list,
    ; then it is copied into the hiscore table and all subsequent items are
    ; moved one position down. The last item on the list is destroyed.
    ; If the new score is not at least equal to the last item on the hiscore
    ; list, then the hiscore table stays unchanged.
    ; Entry: HL = Pointer to new score to build table against
    ; Exit: None.
    ; Uses: All registers!
    ; Copy player score into the workbench for testing.
    ld de,hiscore_workbench
    call copy_score_and_increment_pointers
    ld hl,hiscore_item.1
    ; Point to start of hiscore table and hiscore table buffer.
    ld (hiscore_table_pointer),hl
    ld hl,hiscore_table_buffer
    ld (hiscore_table_buffer_pointer),hl
    ; For each hiscore table item, do...
    ld b,NUMBER_OF_HISCORE_ITEMS
    -:
      ; Is score at hiscore table pointer higher than score at workbench?
      ld hl,(hiscore_table_pointer)
      push hl
      pop ix                          ; Put hiscore table pointer in IX
      ld iy,hiscore_workbench         ; Point IY to score at workbench.
      call compare_scores
      jp c,+
        ; Yes - score in IX (item in hiscore table) is higher.
        ; Copy score item from hiscore table to buffer.
        ld hl,(hiscore_table_buffer_pointer)  ; Load hiscore table buffer into
        ex de,hl                              ; destination position (DE).
        push ix                               ; Load hiscore item into source
        pop hl                                ; position (HL).
        call copy_score_and_increment_pointers
        jp ++
      +:
        ; No - score in IY (item at workbench) is higher or even.
        ; Copy score item from workbench to buffer.
        ld hl,(hiscore_table_buffer_pointer)  ; Load hiscore table buffer into
        ex de,hl                              ; destination position (DE).
        ld hl,hiscore_workbench
        call copy_score_and_increment_pointers
        ld hl,(hiscore_table_pointer)
        ld de,hiscore_workbench
        call copy_score_and_increment_pointers
      ++:
        ; Increment pointers.
        ld de,_sizeof_score_struct
        ld hl,(hiscore_table_pointer)
        add hl,de
        ld (hiscore_table_pointer),hl         ; Inc hiscore table pointer.
        ld hl,(hiscore_table_buffer_pointer)
        add hl,de
        ld (hiscore_table_buffer_pointer),hl  ; Inc hiscore table buffer ptr.
    djnz -
    ; Overwrite the hiscore table with the buffer contents.
    ld hl,hiscore_table_buffer
    ld de,hiscore_item.1
    ld bc,_sizeof_score_struct*NUMBER_OF_HISCORE_ITEMS
    ldir
  ret
  ;
  compare_scores:
    ; Compare two score items to each other, passed to this func in IX and IY.
    ; If score in IY is equal or higher then score in IX, then set carry. If
    ; not, then reset carry.
    ; Entry: IX, IY = Pointers to score structs to compare.
    ; Uses: AF
    ld a,(ix+score_struct.ten_thousands)
    cp (iy+score_struct.ten_thousands)
    jp c,iy_is_equal_or_higher
    jp z,+
    jp ix_is_higher
    +:
      ld a,(ix+score_struct.thousands)
      cp (iy+score_struct.thousands)
      jp c,iy_is_equal_or_higher
      jp z,+
      jp ix_is_higher
      +:
        ld a,(ix+score_struct.hundreds)
        cp (iy+score_struct.hundreds)
        jp c,iy_is_equal_or_higher
        jp z,+
        jp ix_is_higher
        +:
          ld a,(ix+score_struct.tens)
          cp (iy+score_struct.tens)
          jp c,iy_is_equal_or_higher
          jp z,+
          jp ix_is_higher
          +:
            ld a,(ix+score_struct.ones)
            cp (iy+score_struct.ones)
            jp c,iy_is_equal_or_higher
            jp z,iy_is_equal_or_higher
            jp ix_is_higher
            ;
    iy_is_equal_or_higher:
      scf
      ret
    ix_is_higher:
      or a
      ret
  ret
  ;
  copy_score_and_increment_pointers:
    ; Copy the contents of one score struct to another.
    ; Entry: Two score struct pointers:
    ;        HL = Source score
    ;        DE = Destination score.
    ; Exit: Increment HL and DE by the size of one score struct.
    ; Uses: None
    push bc
      ld bc,_sizeof_score_struct
      ldir
    pop bc
  ret
  ;
  copy_hiscore_table:
    ; Copy a table of score items (at the size of the hiscore table) to another
    ; table. It can be used for initializing the hiscore table with ROM data,
    ; or when using the hiscore table buffer to rebuild the hiscore table.
    ; Entry: HL = Source (Pointer to first item in table).
    ;        DE = Destination (Pointer to first item in table).
    ; Uses: None
    SAVE_REGISTERS
    ld b,NUMBER_OF_HISCORE_ITEMS
      -:
        push bc
          ld bc,_sizeof_score_struct
          call copy_score_and_increment_pointers
        pop bc
      djnz -
    RESTORE_REGISTERS
  ret
  ;
.ends
