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
  id db
  anim_counter db
  anim_pointer dw
  timer db
  timer_handler dw
  timer_state db
  ;reserved_word dw
  ;reserved_byte db
.endst
;
.equ GAME_OBJECT_ACTIVE 1
.equ GAME_OBJECT_INACTIVE 0
.equ GAME_OBJECT_SHAPE_REGULAR 0
.equ GAME_OBJECT_SHAPE_TALL 1
.equ GAME_OBJECT_SHAPE_WIDE 2
.equ GAME_OBJECT_SHAPE_SMALL 3
.equ NUMBER_OF_SPRITES_IN_WIDE_METASPRITE 8
.equ NUMBER_OF_SPRITES_IN_TALL_METASPRITE 8
.equ NUMBER_OF_SPRITES_IN_REGULAR_METASPRITE 4
;
.equ BATCH_INIT_TABLE_ELEMENT_SIZE 6
;
.equ COLLISION_TABLE_MAX_OBJECTS 28
.equ COLLISION_TABLE_ELEMENT_SIZE 6             ; y,x,w,h,pointer (word).
; -----------------------------------------------------------------------------
.section "Game objects" free
  ; Functions that work on the Astroswab game_object structure.
  ; What Astroswab game objects are assumed to have in common:
  ; 1. ...
  ; 2. ...
  ; ---------------------------------------------------------------------------
  ; ACTIVATION, DEACTIVATION AND INITIALIZATION OF GAME OBJECTS.
  activate_game_object:
    ; Entry: IX = Pointer to struct.
    ; Registers used: None.
    push af
    ld a,GAME_OBJECT_ACTIVE
    ld (ix+game_object.state),a
    pop af
  ret
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
  get_inactive_game_object:
    ; Entry: IX = Pointer to first instance of struct.
    ;        A = Instances of this struct.
    ; Exit:  If no inactive object is found, then carry is set.
    ;        Else: IX = Pointer to inactive game object and carry is cleared.
    ld b,a
    ld de,_sizeof_game_object
    -:
      ld a,(ix+game_object.state)
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
    ; Exit:
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
  horizontal_zone_send_to_handler:
    ; Test enemy object y to see if it is within the horizontal zone defined
    ; the borders passed in A and B. If true, then jump to handler.
    ; SAVE IX!!!!!!
    ; Entry: IX = Pointer to struct.
    ;        A = Zone start. B = Zone end.
    ;        HL = Pointer to handler.
    ; Uses: AF, BC.
    ld c,b
    ld b,a
    ld a,(ix+game_object.y)
    cp b
    ret c
    cp c
    ret nc
      jp (hl)
  ;
  toggle_game_object_state:
    ; Toggle game object state: active --> inactive and vice versa.
    ; Entry: IX = Pointer to struct.
    ; Exit:
    ; Uses:
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    jp nz,+
      ld a,GAME_OBJECT_INACTIVE
      ld (ix+game_object.state),a
      ret
    +:
    ld a,GAME_OBJECT_ACTIVE
    ld (ix+game_object.state),a
  ret
  ;
  vertical_zone_deactivate_game_object:
    ; Test game object x to see if it is within the vertical zone defined
    ; the borders passed in A and B. If true, then deactivate enemy object.
    ; Entry: IX = Pointer to struct.
    ;        A = Zone start. B = Zone end.
    ; Uses: AF, BC.
    ld c,b
    ld b,a
    ld a,(ix+game_object.x)
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
    inc hl
    ld a,(hl)
    ld (ix+game_object.anim_counter),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.anim_pointer),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.anim_pointer+1),a
    ;
    inc hl
    ld a,(hl)
    ld (ix+game_object.timer),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.timer_handler),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.timer_handler+1),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.timer_state),a
    pop hl
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
  ;
  process_game_object_timer:
    ; Timer is enabled. Count it down. If it reaches zero send to handler.
    ; IX = Pointer to object.
    ; IY Pointer to update parameters.
    ;
    dec (ix+game_object.timer)
    ret nz
        ld l,(ix+game_object.timer_handler)
        ld h,(ix+game_object.timer_handler+1)
        SAVE_REGISTERS
          call hl_
        RESTORE_REGISTERS
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
  update_game_object:
    ; Perform various per-frame actions on an active game object.
    ; IX = Pointer to game object.
    ; IY = Pointer to parameter string.
    ;
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    ret nz
      ;
      ld a,(ix+game_object.timer_state)
      cp ENABLED
      call z,process_game_object_timer
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
      ld a,(iy+11)
      cp TRUE
      call z,add_game_object_to_collision_table
  ret
  ;
  ; ---------------------------------------------------------------------------
  ; DRAWING AND ANIMATING.
  animate_game_object:
    ; Entry: IX = Pointer to struct.
    ; Assumes that the game object's animation pointer points to a 256 byte
    ; table, where each byte is a SPRITE_X in the format expected by
    ; draw_game_object.
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
      cp GAME_OBJECT_SHAPE_WIDE
      jp z,wide_shape
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
    ld hl,regular_meta_sprite_table
    ld a,NUMBER_OF_SPRITES_IN_REGULAR_METASPRITE
    call add_metasprite
  jp draw_game_object_end
  tall_shape:
    ld hl,tall_meta_sprite_table
    ld a,NUMBER_OF_SPRITES_IN_TALL_METASPRITE
    call add_metasprite
  jp draw_game_object_end
  wide_shape:
    ld hl,wide_meta_sprite_table
    ld a,NUMBER_OF_SPRITES_IN_WIDE_METASPRITE
    call add_metasprite
  jp draw_game_object_end
  ;
  load_animation_game_object:
    ; Entry: IX = Pointer to struct.
    ;        HL = Pointer to animation table.
    ld (ix+game_object.anim_pointer),l
    ld (ix+game_object.anim_pointer+1),h
  ret
  ;
  ; ---------------------------------------------------------------------------
  ; MOVING AND POSITIONING
  move_game_object:
    ; Apply yspeed to y and xspeed to x.
    ; Entry: IX = Pointer to instance of enemy object.
    push af
    push bc
      ld b,(ix+game_object.y)
      ld a,(ix+game_object.yspeed)
      bit 7,a                       ; Test sign bit (negative speed?)
      jp z,+
        neg
        ld b,a
        ld a,(ix+game_object.y)
        sub b
        jp ++
      +:
        add a,b
      ++:
      ld (ix+game_object.y),a
      ;
      ld b,(ix+game_object.x)
      ld a,(ix+game_object.xspeed)
      bit 7,a                       ; Test sign bit (negative speed?)
      jp z,+
        neg
        ld b,a
        ld a,(ix+game_object.x)
        sub b
        jp ++
      +:
        add a,b
      ++:
      ld (ix+game_object.x),a
    pop bc
    pop af
  ret
  ;
  position_game_object_in_relation_to_game_object:
    ; Set one game object's y,x position offset from another game object's
    ; y,x position.
    ; Entry: IX = Pointer to anchor game object
    ;        IY = Pointer to game object to be positioned.
    ;         B = y-offset, C = x-offset.
    ld a,(ix+game_object.y)   ; Get anchor object's y-pos.
    add a,b                   ; Add y-offset.
    ld (iy+game_object.y),a   ; Position other object.
    ld a,(ix+game_object.x)   ; Get anchor object's x-pos.
    add a,c                   ; Add x-offset.
    ld (iy+game_object.x),a   ; Position other object.
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
  set_game_object_xspeed_depending_on_other_game_object_x:
    ; IX = Pointer to game object
    ; IY = Pointer to other object.
    ; A = Speed
    ; Uses: B
    ld b,a
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    ret nz
    ;
      ld a,(ix+game_object.x)
      sub (iy+game_object.x)
      call get_absolute_value
      cp b
      jp nc,+
        xor a
        ld (ix+game_object.xspeed),a
        ret
      +:
      ;
      ld a,(ix+game_object.x)
      cp (iy+game_object.x)
      jp nc,+
        ld a,b
        ld (ix+game_object.xspeed),a
        ret
      +:
      ld a,b
      neg
      ld (ix+game_object.xspeed),a
      ret
  ret
  ;
  mark_scanline_with_game_object:
    ; Mark scanline by drawing a game object on it.
    ; Entry: IX = Pointer to game object to use as marker.
    ; Uses: AF.
    in a,(V_COUNTER_PORT)
    cp CRT_BOTTOM_BORDER
    ret nc
    ld (ix+game_object.y),a
    call draw_game_object
  ret
  ; ---------------------------------------------------------------------------
  ; COLLISION DETECTION
  add_game_object_to_collision_table:
    ; Add game object y,x and hitbox data to the collision table, so it is
    ; included in collision tests by "detect_collision_using_collision_table".
    ; Entry: IX = Pointer to game object.
    ; Uses: None.
    SAVE_REGISTERS
    ;ld a,(ix+game_object.y)                   ; Get game object's y-position.
    ;add a,(ix+game_object.hitbox_offset_y)    ; Apply hitbox offset.
    ;cp COLLISION_TABLE_DIVIDER                ; Compare to divider value.
    ;jp nc,+                                   ; On carry, game object must be

      ;add a,(ix+game_object.hitbox_height)
      ;cp COLLISION_TABLE_DIVIDER
      ;jp nc,+
      ; If this happens - object is in fact in both coll. tables!

      ld hl,objects_in_collision_table    ; above the divider (top half of
      ld a,(hl)                               ; screen). Increment top coll.
      inc (hl)                                ; table index and point HL to
      ld hl,collision_table               ; top collision table.
      jp ++
    +:
      ;ld hl,objects_in_bottom_collision_table ; Same with bottom coll. table.
      ;ld a,(hl)
      ;inc (hl)
      ;ld hl,bottom_collision_table
    ++:
    ;                                         ; Prepare to calculate offset...
    cp 0                                      ; Special case first game object,
    jp z,+                                    ; don't apply offset.
      ld b,a
      xor a
      -:
        add a,COLLISION_TABLE_ELEMENT_SIZE    ; Multiply to calculate offset.
      djnz -
      ld d,0
      ld e,a
      add hl,de                               ; Classic offset calc. move.
    +:                                        ; Now HL points to next 'empty'
    ;                                         ; slot in coll. table.
    ld a,(ix+game_object.y)                   ; Get game object y.
    add a,(ix+game_object.hitbox_offset_y)    ; Apply hitbox offset.
    ld (hl),a                                 ; Write it to coll. table.
    inc hl                                    ;
    ld a,(ix+game_object.x)                   ; Get game object x.
    add a,(ix+game_object.hitbox_offset_x)    ; ... and write it, along with
    ld (hl),a                                 ; hitbox data, to coll. table.
    inc hl
    ld a,(ix+game_object.hitbox_width)
    ld (hl),a
    inc hl
    ld a,(ix+game_object.hitbox_height)
    ld (hl),a
    inc hl
    ;                                         ; Last word in table element is
    push ix                                   ; a pointer to the game object's
    pop de                                    ; struct in ram.
    ld (hl),e
    inc hl
    ld (hl),d
    RESTORE_REGISTERS
  ret
  ;
  add_multiple_game_object_instances_to_collision_table:
    ; Batch version of add_game_object_to_collision_table.
    ; Entry: IX = Pointer to game object with multiple instances.
    ;        A = number of instances to position.
    ; Uses: AF, B, DE, HL, IX
    ld b,a
    ld de,_sizeof_game_object
    -:
      push bc
      call add_game_object_to_collision_table
      add ix,de
      pop bc
    djnz -
  ret
  ;
  begin_collision_tables:
    ; Place before the first call to "add_game_object_to_collision_table". The
    ; collision table index is reset.
    ; Entry: None.
    ; Uses: AF.
    xor a
    ld (objects_in_collision_table),a
  ret
  ;
  detect_collision_using_collision_table:
    ; Assumes game objects to be initialized with separate bytes for y and x
    ; hitbox offset. Test object passed in IX against all objects added to
    ; the collision table (top or bottom table depending on object in IX's y
    ; position).
    ;
    ; Simple form of collision detection: Axis-aligned bounding box.
    ; If (rect1.x < rect2.x + rect2.width &&  (collision_subtest_1)
    ; rect1.x + rect1.width > rect2.x &&      (collision_subtest_2)
    ; rect1.y < rect2.y + rect2.height &&     (collision_subtest_3)
    ; rect1.height + rect1.y > rect2.y)       (collision_subtest_4)
    ;  --> collision detected!
    ; We perform these four tests one at a time. If any one test fails,
    ; there is no collision, and we return with carry reset. If no test fails
    ; then we have collision, and return with carry set.
    ;
    ; Entry: IX = Pointer to game object.
    ; Exit:  IY = Pointer to object colliding, carry set/reset.
    ; Uses: AF, B, HL, IY.
    ld iy,collision_table                 ; Point IY to start of table.
    ld a,(objects_in_collision_table)     ; Quick exit if table is empty.
    or a                                      ; (top collision table).
    ret z
      ld b,a                                      ; Loop through all game objects
      -:                                          ; in the collision table.
        push bc                                   ; Save loop counter.
          ;
          call collision_subtest_1                ; Perform the four tests...
          jp nc,+                                 ; If any one fails, then skip
          call collision_subtest_2                ; to next object in table.
          jp nc,+
          call collision_subtest_3
          jp nc,+
          call collision_subtest_4
          jp nc,+
            ; Fall through: Collision!
            ld l,(iy+4)                           ; Return with carry set, and
            ld h,(iy+5)                           ; with pointer in IY...
            push hl
            pop iy
            pop bc                                ; Align the stack.
            ret
          +:
          ld de,COLLISION_TABLE_ELEMENT_SIZE      ; Apply offset to get next item
          add iy,de                               ; in table.
        pop bc                                    ; Restore loop counter.
      djnz -                                      ; Loop back.
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
    ; -------------------------------------------------------------------------
    ; Misc.
    ;

.ends
