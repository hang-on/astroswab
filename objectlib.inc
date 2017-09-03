; objectlib.inc
; -----------------------------------------------------------------------------
; GAME OBJECTS
; -----------------------------------------------------------------------------
; Functions that work on the Astroswab game_object structure.
; What Astroswab game objects are assumed to have in common:
; 1. ...
; 2. ...
.struct game_object
  y db
  x db
  sprite db
  yspeed db
  xspeed db
  state db
  shape db
  hitbox_offset db  ;xxxx yyyy DEPRECATED - treat as dead byte
  hitbox_offset_y db
  hitbox_offset_x db
  hitbox_width db
  hitbox_height db
  anim_counter db
  anim_pointer dw
  id db
.endst
;
.equ GAME_OBJECT_ACTIVE 1
.equ GAME_OBJECT_INACTIVE 0
.equ GAME_OBJECT_SHAPE_REGULAR 0
.equ GAME_OBJECT_SHAPE_TALL 1
.equ GAME_OBJECT_SHAPE_WIDE 2
.equ GAME_OBJECT_SHAPE_SMALL 3
; -----------------------------------------------------------------------------
.section "Game objects" free
  activate_game_object:
    ; Entry: IX = Pointer to struct.
    ; Registers used: None.
    push af
    ld a,GAME_OBJECT_ACTIVE
    ld (ix+game_object.state),a
    pop af
  ret
  ;
  animate_game_object:
    ; Entry: IX = Pointer to struct.
    SAVE_REGISTERS
    ld a,(ix+game_object.anim_counter)
    inc a
    ld (ix+game_object.anim_counter),a
    ld d,0
    ld e,a
    ld l,(ix+game_object.anim_pointer)
    ld h,(ix+game_object.anim_pointer+1)
    add hl,de
    ld a,(hl)
    ld (ix+game_object.sprite),a
    RESTORE_REGISTERS
  ret
  ;
  ;
  batch_initialize_game_objects:
    ; Initialize a batch of game objects using a table.
    ; HL = Pointer to batch init table
    ; A = number of objects to initialize.
    ; Example of 6 byte wide batch init table:
    ;     .dw swabby, swabby_init_table, 1
    ;     .dw bullet, bullet_init_table, BULLET_MAX
    ; Uses: AF, BC, DE, HL, IX
    ld b,a
    --:
      push bc
        ; Point IX to ram structure.
        ld e,(hl)
        inc hl
        ld d,(hl)
        inc hl
        push de
        pop ix
        ; Point HL to init table.
        ld e,(hl)
        inc hl
        ld d,(hl)
        inc hl
        ld a,(hl)
        ld b,a
        inc hl
        inc hl
        push hl
          push de
          pop hl
          ld de,_sizeof_game_object
            -:
            call initialize_game_object
            add ix,de
            djnz -
        pop hl
      pop bc
    djnz --
  ret
  batch_draw_game_objects:
    ; Batch draw a number of game objects.
    ; HL = table with game objects.
    ; A = number of game objects to draw.
    ;       Example of batch draw table:
    ;       .dw asteroid, 3, swabby, 1
    ;       The number makes it possible to draw arrayed objects, i.e. bullets.
    ; Uses: AF, BC, DE, HL, IX
    ld b,a
    --:
      push bc
        ld e,(hl)
        inc hl
        ld d,(hl)
        inc hl
        push de
        pop ix
        ld b,(hl)
        inc hl
        inc hl
        ld de,_sizeof_game_object
        -:
          push bc
            call draw_game_object
            add ix,de
          pop bc
        djnz -
      pop bc
    djnz --
  ret
  ;
  draw_game_object:
    ; Entry: IX = Pointer to struct.
    ; Registers used: None.
    SAVE_REGISTERS                ; Because add_metasprite mess with registers!
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    jp nz,draw_game_object_end
      ; Put active objects in the SAT buffer.
      ld a,(ix+game_object.sprite)
      cp FIRST_8x8_SPRITE
      jp nc,small_shape
      ld a,(ix+game_object.shape)
      cp GAME_OBJECT_SHAPE_REGULAR
      jp z,regular_shape
      cp GAME_OBJECT_SHAPE_TALL
      jp z,tall_shape
    draw_game_object_end:
    RESTORE_REGISTERS
  ret
  small_shape:
    ld c,(ix+game_object.x)
    ld b,(ix+game_object.y)
    ld a,(ix+game_object.sprite)
    call add_sprite
  jp draw_game_object_end
  regular_shape:
    call add_metasprite
  jp draw_game_object_end
  tall_shape:
    call add_tall_metasprite
  jp draw_game_object_end
  ;
  deactivate_game_object:
    ; Entry: IX = Pointer to struct.
    ; Registers used: None.
    push af
    ld a,GAME_OBJECT_INACTIVE
    ld (ix+game_object.state),a
    pop af
  ret
  ;
  get_game_object_hitbox:
    ; Entry: IX = pointer to struct.
    ld a,(ix+game_object.shape)
    cp GAME_OBJECT_SHAPE_SMALL
    jp nz,+
      ld a,8
      ret
    +:
    ld a,16
  ret
  ;
  get_game_object_position:
    ; Entry: IX = pointer to struct.
    ; Exit: A = object Y and B = object X.
    ld a,(ix+game_object.y)
    ld b,(ix+game_object.x)
  ret
  ;
  get_game_object_state:
    ; Entry: IX = pointer to struct.
    ; Exit: Returns in A state of the enemy object, which should be either
    ; GAME_OBJECT_INACTIVE or GAME_OBJECT_ACTIVE.
    ld a,(ix+game_object.state)
  ret
  ;
  get_game_object_x:
    ; Entry: IX = pointer to struct.
    ; Exit: Returns in A x of the enemy object.
    ld a,(ix+game_object.x)
  ret
  ;
  get_game_object_xspeed:
    ; Entry: IX = pointer to struct.
    ; Exit: Returns in A  game object xspeed.
    ld a,(ix+game_object.xspeed)
  ret
  ;
  get_inactive_game_object:
    ; Entry: IX = Pointer to first instance of struct.
    ;        A = Instances of this struct.
    ; Exit:  If no inactive object is found, then carry is set.
    ;        Else: IX = Pointer to inactive game object and carry is cleared.
    ld b,a
    ld de,_sizeof_game_object
    -:
      call get_game_object_state
      cp GAME_OBJECT_INACTIVE
      jp z,+
      add ix,de
    djnz -
    ; Here we come if no inactive objects are found...
    ; Set carry to indicate error and return.
    scf
    ret
    +:
      or a        ; Clear carry.
  ret             ; Return with ix pointing...
  ;
  ;
  horizontal_zone_deactivate_game_object:
    ; Test enemy object y to see if it is within the horizontal zone defined
    ; the borders passed in A and B. If true, then deactivate enemy object.
    ; Entry: IX = Pointer to struct.
    ;        A = Zone start. B = Zone end.
    ; Uses: AF, BC.
    ld c,b
    ld b,a
    ld a,(ix+game_object.y)
    cp b
    ret c
    cp c
    ret nc
    call deactivate_game_object
  ret
  ;
  initialize_game_object:
    ; Entry: IX = Pointer to struct.
    ;        HL = Pointer to game object init data:
    ;        (sprite, yspeed, xspeed, shape, y, x, state
    ;        hitbox_offset, hitbox_width, hitbox_height).
    ; Example: asteroid_setup_table:
    ;         .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    ;         .db 0, 0, GAME_OBJECT_INACTIVE, %00000000, 16, 16
    ; Uses: A - but HL is preserved.
    push hl
    ld a,(hl)
    ld (ix+game_object.sprite),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.yspeed),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.xspeed),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.shape),a
    ;
    inc hl
    ld a,(hl)
    ld (ix+game_object.y),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.x),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.state),a
    ;
    inc hl
    ld a,(hl)
    ld (ix+game_object.hitbox_offset),a
    inc hl
    ;
    ld a,(hl)
    ld (ix+game_object.hitbox_offset_y),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.hitbox_offset_x),a
    inc hl
    ;
    ld a,(hl)
    ld (ix+game_object.hitbox_width),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.hitbox_height),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.id),a
    pop hl
  ret
  ;
  load_animation_game_object:
    ; Entry: IX = Pointer to struct.
    ;        HL = Pointer to animation table.
    ld (ix+game_object.anim_pointer),l
    ld (ix+game_object.anim_pointer+1),h
  ret
  ;
  move_game_object:
    ; Apply yspeed to y and xspeed to x.
    ; Entry: IX = Pointer to instance of enemy object.
    ld a,(ix+game_object.yspeed)
    ld b,a
    ld a,(ix+game_object.y)
    add a,b
    ld (ix+game_object.y),a
    ;
    ld a,(ix+game_object.xspeed)
    ld b,a
    ld a,(ix+game_object.x)
    add a,b
    ld (ix+game_object.x),a
  ret
  ;
  position_multiple_game_object_instances:
    ; Entry: IX = Pointer to game object with multiple instances.
    ;        A = number of instances to position.
    ;        HL = pointer to position table, containing Y,X pairs.
    ; Uses: AF, B, DE, HL, IX
    ld b,a
    ld de,_sizeof_game_object
    -:
      ld a,(hl)
      ld (ix+game_object.y),a
      inc hl
      ld a,(hl)
      ld (ix+game_object.x),a
      inc hl
      add ix,de
    djnz -
  ret
  ;
  spawn_game_object_in_invisible_area:
    ; Entry: IX = Pointer to enemy object.
    ; Object gets a random position and is activated!
    ld a,INVISIBLE_AREA_BOTTOM_BORDER-8
    ld (ix+game_object.y),a
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
    ld (ix+game_object.x),a  ; x = 8 - 229.
    call activate_game_object
  ret
  ;
  ;
  set_game_object_position:
    ; Entry: IX = pointer to struct.
    ;        A = Y, B = X.
    ld (ix+game_object.y),a
    ld (ix+game_object.x),b
  ret
  set_game_object_speed:
    ; IX = Pointer to enemy object.
    ; A = Yspeed, B = Xspeed.
    ld (ix+game_object.yspeed),a
    ld (ix+game_object.xspeed),b
  ret
  ;
  set_game_object_sprite:
    ; Entry: IX = Pointer to enemy object.
    ;        A = Sprite index.
    ld (ix+game_object.sprite),a
  ret
  ;
.ends
