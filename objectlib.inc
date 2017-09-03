; objectlib.inc
; ....
; -----------------------------------------------------------------------------
; GAME OBJECTS
; -----------------------------------------------------------------------------
.struct game_object
  y db
  x db
  sprite db
  yspeed db
  xspeed db
  state db
  shape db
  hitbox_offset db  ;xxxx yyyy (deprecated)
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
;
.equ BATCH_INIT_TABLE_ELEMENT_SIZE 6
; -----------------------------------------------------------------------------
.section "Game objects" free
  ; Functions that work on the Astroswab game_object structure.
  ; What Astroswab game objects are assumed to have in common:
  ; 1. ...
  ; 2. ...
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
;  get_game_object_position:
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
  ; Collision. FIXME - better layotu.
  add_multiple_game_object_instances_to_collision_table:
    ; Entry: IX = Pointer to game object with multiple instances.
    ;        A = number of instances to position.
    ; Uses: AF, B, DE, HL, IX
    ld b,a
    ld de,_sizeof_game_object
    -:
      push bc
      call add_object_to_collision_table
      add ix,de
      pop bc
    djnz -
  ret

  add_object_to_collision_table:
    ; IX = Pointer to object.
    ; Uses: AF, BC, DE, HL
    ld a,(ix+game_object.y)
    add a,(ix+game_object.hitbox_offset_y)

    cp COLLISION_TABLE_DIVIDER
    jp nc,+
      ld hl,objects_in_top_collision_table
      ld a,(hl)
      inc (hl)
      ld hl,top_collision_table
      jp ++
    +:
      ld hl,objects_in_bottom_collision_table
      ld a,(hl)
      inc (hl)
      ld hl,bottom_collision_table
    ++:
    ;
    cp 0
    jp z,+
    ld b,a
    xor a
    -:
      add a,COLLISION_TABLE_ELEMENT_SIZE ; Table element size here!
    djnz -
    ld d,0
    ld e,a
    add hl,de
    +:
    ;
    ld a,(ix+game_object.y) ; Apply hitbox offset.
    add a,(ix+game_object.hitbox_offset_y)
    ld (hl),a
    inc hl
    ld a,(ix+game_object.x)
    add a,(ix+game_object.hitbox_offset_x)
    ld (hl),a
    inc hl
    ld a,(ix+game_object.hitbox_width)
    ld (hl),a
    inc hl
    ld a,(ix+game_object.hitbox_height)
    ld (hl),a
    inc hl
    ;
    ; Add pointer to object
    push ix
    pop de
    ld (hl),e
    inc hl
    ld (hl),d
  ret
  begin_collision_tables:
    xor a
    ld (objects_in_top_collision_table),a
    ld (objects_in_bottom_collision_table),a
    ;
  ret
;
  ;
  detect_collision_using_collision_table:
    ; Version 3 notes:
    ; Assumes game objects to be initialized with
    ; separate bytes for y and x hitbox offset.
    ;
    ; Simple form of collision detection: Axis-aligned bounding box.
    ; If (rect1.x < rect2.x + rect2.width &&  (coll1)
    ; rect1.x + rect1.width > rect2.x &&      (coll2)
    ; rect1.y < rect2.y + rect2.height &&     (coll3)
    ; rect1.height + rect1.y > rect2.y)       (coll4)
    ;  --> collision detected!
    ; We perform these four tests one at a time. If any one test fails,
    ; there is no collision, and we return with carry reset. If no test fails
    ; then we have collision, and return with carry set.
    ; Uses: AF, B, IY
    ;
    ; Determine relevant collision table.
    ld a,(ix+game_object.hitbox_offset_y)
    add a,(ix+game_object.y)
    cp COLLISION_TABLE_DIVIDER
    jp nc,+
      ld hl,top_collision_table
      ld a,(objects_in_top_collision_table)
      or a
      ret z
      jp ++
    +:
      ld hl,bottom_collision_table
      ld a,(objects_in_bottom_collision_table)
      or a
      ret z
    ++:
    push hl
    pop iy
    ld b,a
    -:
      push bc
        ;
        call collision_subtest_1
        jp nc,+
        call collision_subtest_2
        jp nc,+
        call collision_subtest_3
        jp nc,+
        call collision_subtest_4
        jp nc,+
          ; Fall through: Collision!
          ; Return with carry set, and with pointer in IY.
          ld l,(iy+4)
          ld h,(iy+5)
          push hl
          pop iy
          pop bc ; clean stack
          ret
        +:
        ld de,COLLISION_TABLE_ELEMENT_SIZE
        add iy,de
      pop bc
    djnz -

  ret
    collision_subtest_1:
      ; rect1.x < rect2.x + rect2.width
      ld a,(iy+1) ;
      add a,(iy+2)
      ld b,a
      ld a,(ix+game_object.hitbox_offset_x)
      add a,(ix+game_object.x)
      cp b
    ret
    collision_subtest_2:
      ; rect1.x + rect1.width > rect2.x
      ld a,(ix+game_object.x)
      add a,(ix+game_object.hitbox_offset_x)
      add a,(ix+game_object.hitbox_width)
      ld b,a
      ld a,(iy+1)
      cp b
    ret
    collision_subtest_3:
      ; rect1.y < rect2.y + rect2.height
      ld a,(iy+0)
      ld b,(iy+3)
      add a,b
      ld b,a
      ld a,(ix+game_object.y)
      add a,(ix+game_object.hitbox_offset_y)
      cp b
    ret
    collision_subtest_4:
      ; rect1.height + rect1.y > rect2.y
      ld a,(ix+game_object.y)
      add a,(ix+game_object.hitbox_offset_y)
      add a,(ix+game_object.hitbox_height)
      ld b,a
      ld a,(iy+0)
      cp b
    ret
    ;

.ends
