; astroswablib.inc
; Functions related specifically to astroswab, or with significant dependencies
; on local/contextual design choices.
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
  ; Definitions for referencing the sprites on the spritesheet.
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
.ends

; -----------------------------------------------------------------------------
; ENEMY OBJECTS
; -----------------------------------------------------------------------------
.struct enemy_object
  y db
  x db
  sprite db
  yspeed db
  state db
.endst

.equ ENEMY_OBJECT_ACTIVE 1
.equ ENEMY_OBJECT_INACTIVE 0

.section "Enemy objects" free
  ; Functions that work on the enemy_object structure.

  init_enemy_object:
    ; HL = init table ptr.
    ; IX = enemy object ptr.
    push ix
    pop de
    ld bc,_sizeof_enemy_object
    ldir
  ret
  ;
  move_enemy_object_vertically:
    ld a,(ix+enemy_object.yspeed)
    add a,(ix+enemy_object.y)
    ld (ix+enemy_object.y),a
  ret
  draw_enemy_object:
    ; Entry: IX = Pointer to struct.
    ; No registers destroyed.
    ld a,(ix+enemy_object.state)
    cp ENEMY_OBJECT_ACTIVE
    call z,add_metasprite
  ret
  ;
  activate_enemy_object:
    ; Entry: IX = Pointer to struct.
    push af
    ld a,ENEMY_OBJECT_ACTIVE
    ld (ix+enemy_object.state),a
    pop af
  ret
  ;
  deactivate_enemy_object:
    ; Entry: IX = Pointer to struct.
    push af
    ld a,ENEMY_OBJECT_INACTIVE
    ld (ix+enemy_object.state),a
    pop af
  ret

.ends
