; astroswablib.inc
; Functions related specifically to astroswab, or with significant dependencies
; on local/contextual design choices.

; Definitions for referencing the sprites on the spritesheet.
; Used by functions drawing objects.
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


; -----------------------------------------------------------------------------
.section "astroswablib" free
; -----------------------------------------------------------------------------


  batch_print:
    ; Batch print strings at different screen positions. Handy for making a
    ; menu, or for setting up text in various places. Strings and positions are
    ; stored in a table in the following format:
    ; pointer to string (word), row (byte), column (byte).
    ; Entry: IX = pointer to table.
    ;        A = number of strings to print.
    ; Uses: AF, BC, HL.
    ; Depends on print (in bluelib.inc), and on #-terminated strings.
    -:
      push af
      ld l,(ix+0)
      ld h,(ix+1)
      ld b,(ix+2)
      ld c,(ix+3)
      call print
      .rept 4
        inc ix
      .endr
      pop af
      dec a
    jp nz,-
  ret

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
.ends

; -----------------------------------------------------------------------------
.section "Random number generation" free
; -----------------------------------------------------------------------------
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
; ENEMY OBJECTS
; -----------------------------------------------------------------------------
.struct enemy_object
  y db
  x db
  sprite db
  yspeed db
  xspeed db
  state db
.endst

.equ ENEMY_OBJECT_ACTIVE 1
.equ ENEMY_OBJECT_INACTIVE 0

.section "Enemy objects" free
  ; Functions that work on the enemy_object structure.
  ;
  reset_enemy_object_position:
    ; In Astroswab, all enemy objects share the feature that they spawn
    ; in the bottom of the invisible area (at x = 8-229). Thus when an enemy
    ; object gets its position reset, y is fixed and x is randomized.
    ; Entry: IX = Pointer to enemy object.
    ld a,INVISIBLE_AREA_BOTTOM_BORDER-8
    ld (ix+enemy_object.y),a
    call get_random_number
    and %01111111             ; rnd(128).
    ld b,a
    call get_random_number
    and %00111111             ; rnd(64).
    add a,b
    ld b,a
    call get_random_number
    and %00011111             ; rnd(32).
    add a,b
    add a,8                   ; x = (0-127) + (0-63) + (0-31) + 8.
    ld (ix+enemy_object.x),a  ; x = 8 - 229.
  ret
  ;
  set_enemy_object_sprite:
    ; Entry: IX = Pointer to enemy object.
    ;        A = Sprite index.
    ld (ix+enemy_object.sprite),a
  ret
  ;
  set_enemy_object_speed:
    ; IX = Pointer to enemy object.
    ; A = Yspeed, B = Xspeed.
    ld (ix+enemy_object.yspeed),a
    ld (ix+enemy_object.xspeed),b
  ret
  ;
  move_enemy_object:
    ; Apply yspeed to y and xspeed to x.
    ; Entry: IX = Pointer to instance of enemy object.
    ld a,(ix+enemy_object.yspeed)
    ld b,a
    ld a,(ix+enemy_object.y)
    add a,b
    ld (ix+enemy_object.y),a
    ;
    ld a,(ix+enemy_object.xspeed)
    ld b,a
    ld a,(ix+enemy_object.x)
    add a,b
    ld (ix+enemy_object.x),a
  ret
  ;
  draw_enemy_object:
    ; Entry: IX = Pointer to struct.
    ; Registers used: None.
    SAVE_REGISTERS                ; Because add_metasprite mess with registers!
    ld a,(ix+enemy_object.state)
    cp ENEMY_OBJECT_ACTIVE
    call z,add_metasprite
    RESTORE_REGISTERS
  ret
  ;
  activate_enemy_object:
    ; Entry: IX = Pointer to struct.
    ; Registers used: None.
    push af
    ld a,ENEMY_OBJECT_ACTIVE
    ld (ix+enemy_object.state),a
    pop af
  ret
  ;
  deactivate_enemy_object:
    ; Entry: IX = Pointer to struct.
    ; Registers used: None.
    push af
    ld a,ENEMY_OBJECT_INACTIVE
    ld (ix+enemy_object.state),a
    pop af
  ret
  ;
  get_enemy_object_state:
    ; Entry: IX = pointer to struct.
    ; Exit: Returns in A state of the enemy object, which should be either
    ; ENEMY_OBJECT_INACTIVE or ENEMY_OBJECT_ACTIVE.
    ld a,(ix+enemy_object.state)
  ret
  ;
.ends

; -----------------------------------------------------------------------------
; PLAYER OBJECTS
; -----------------------------------------------------------------------------
.struct player_object
  y db
  x db
  sprite db
  yspeed db
  xspeed db
  state db
.endst

.equ PLAYER_OBJECT_ACTIVE 1
.equ PLAYER_OBJECT_INACTIVE 0

.section "Player objects" free
  ; Functions that work on the player_object structure.
  ;
  initialize_player_object:
    ; Entry: HL = Pointer to init table that matches the player object struct.
    ;        IX = Pointer to instance of player object.
    ld bc,_sizeof_player_object
    push ix
    pop de
    ldir
  ret
  ;
  move_player_object:
    ; Apply yspeed to y and xspeed to x.
    ; Entry: IX = Pointer to instance of player object.
    ld a,(ix+player_object.yspeed)
    ld b,a
    ld a,(ix+player_object.y)
    add a,b
    ld (ix+player_object.y),a
    ;
    ld a,(ix+player_object.xspeed)
    ld b,a
    ld a,(ix+player_object.x)
    add a,b
    ld (ix+player_object.x),a
  ret
  ;
  draw_player_object:
    ; Put a 16x16 metasprite representing the player into the SAT.
    ; Entry: IX = Pointer to meta sprite data block, where IX+0 is Y, IX+1 is X
    ;             and IX+2 is the character code.
    ; Uses: AF, BC, DE, HL
    ; Dependencies: add_sprite (in bluelib), and a spritesheet formatted as
    ; 2x21 16x16 images, loaded into SPRITE_BANK_START.
    ld d,4
    ld hl,player_object_table
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
  player_object_table:
    .db 0, 0, 0
    .db 0, 8, 1
    .db 8, 0, 4
    .db 8, 8, 5

.ends
