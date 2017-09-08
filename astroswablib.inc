; astroswablib.inc
; Functions related specifically to astroswab, or with significant dependencies
; on local/contextual design choices.
;
; -----------------------------------------------------------------------------
.section "Misc functions" free
; -----------------------------------------------------------------------------
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
.section "swabby and bullet functions" free
; -----------------------------------------------------------------------------
  handle_bullet_collision:
    ; Placeholder function for handling what happens when bullet collides with
    ; other objects.
    ; TODO: Increment score depending on other object's id.
    ; IX = bullet, IY other object..
    ld a,GAME_OBJECT_INACTIVE
    ld (ix+game_object.state),a
    ld (iy+game_object.state),a
  ret
  handle_swabby_collision:
    ; Placeholder function for handling what happens when swabby collides with
    ; other objects.
    ; TODO: Implement DEATH!
    ; IX = bullet, IY other object..
    ld a,GAME_OBJECT_INACTIVE
    ld (iy+game_object.state),a
  ret
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

;
; -----------------------------------------------------------------------------
.ramsection "Sandbox variables" slot 3
; -----------------------------------------------------------------------------
  ;
.ends
.section "Sandbox functions" free
  ; Short text here...
  sandbox_function_1:
    ; ...
  ret
  ;
  .equ SHARD_X_OFFSET_1 (-3)
  .equ SHARD_Y_OFFSET_1 2
  .equ SHARD_X_OFFSET_2 10
  .equ SHARD_Y_OFFSET_2 16
  ;
  explode_asteroid_into_shards:
    ;
    ; Entry: IX = Pointer to asteroid to explode.
    ; Exit: Carry flag set if all shards are currently active.
    ; Uses: AF, IX, IY.

    ; TODO: X) Find next inactive shard.
    ; x) Make constants. - place them right above this func?
    ; x) Apply offset function.
    ; 3) Comment and clean up this func.
    ; 4) transfer game_object speed (asteroid) to shard.
    ; x) DRY!
    ; -- > the pull it into master. And then add vertical column deactivate zone
    push ix
    pop hl
    .rept 2
      ld ix,shard
      ld a,SHARD_MAX
      call get_inactive_game_object
      ld a,GAME_OBJECT_ACTIVE
      ld (ix+game_object.state),a
      push ix
    .endr
    push hl
    pop ix
    ;
    ld hl,offset_table
    .rept 2
      pop iy
      ld a,(hl)
      inc hl
      ld (iy+game_object.xspeed),a
      ld b,(hl)
      inc hl
      ld c,(hl)
      inc hl
      call position_game_object_in_relation_to_game_object
    .endr
  ret
    offset_table:
      .db -1, SHARD_Y_OFFSET_1, SHARD_X_OFFSET_1
      .db 1, SHARD_Y_OFFSET_2, SHARD_X_OFFSET_2

  ;
  position_game_object_in_relation_to_game_object:
    ; IX = Game object 1, IY = Game object 2, B = Y offset, C=X offset.
    ld a,(ix+game_object.y)
    add a,b
    ld (iy+game_object.y),a
    ;
    ld a,(ix+game_object.x)
    add a,c
    ld (iy+game_object.x),a
  ret



.ends
