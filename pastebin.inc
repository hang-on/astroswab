; 2017-09-06
get_trigger_state:
  ; Entry: IX = Pointer to trigger struct.
  ; Exit: Trigger state (ENABLED/DISABLED bluelib constants) in A.
  ld a,(ix+trigger.state)
ret
;


;2017-09-05
test_1:
  ld ix,bee
  ld hl,bee_init_table
  call initialize_game_object
  ;
  ld a,COLLISION_TABLE_DIVIDER
  call wait_for_scanline
  in a,(V_COUNTER_PORT)
  assertEqualsA COLLISION_TABLE_DIVIDER, "Test 1 failed"
  ;
  ld ix,bee
  call mark_scanline
ret

;2017-09-03
;
handle_collision:
  ld a,TRUE
  ld (temp_byte),a
  push iy
  pop hl
  ld (temp_word),hl
ret

sandbox_swabby_init_table:
.db SWABBY_IDLE_SPRITE, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
.db 135, 80, GAME_OBJECT_ACTIVE
.db %00000001, 0, 1, 15, 14, SWABBY
sandbox_asteroid_init_table:
.db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
.db 20, 20, GAME_OBJECT_ACTIVE
.db %00000000, 0, 0, 16, 16, ASTEROID
sandbox_bullet_init_table:
.db SPRITE_44, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_SMALL
.db 25, 24, GAME_OBJECT_ACTIVE
.db %00100000, 2, 0, 4, 8, BULLET
sandbox_game_object_init_table:
.dw bullet, sandbox_bullet_init_table, 5
.dw asteroid, sandbox_asteroid_init_table, 3
sandbox_batch_draw_table:
.dw bullet, 5
.dw asteroid, 3
sandbox_bullet_position_table:
.db 10, 10, 30, 20, 50, 30, 70, 40, 90, 50
sandbox_asteroid_position_table:
.db 150, 20, 100, 20, 100, 50

test_6:
; Test 6 is a clone of test 5, testing new coll.detect function.
ld ix,swabby
ld hl,sandbox_swabby_init_table
call initialize_game_object
call draw_game_object
;
;
ld hl,sandbox_game_object_init_table
ld a,2
call batch_initialize_game_objects
;
ld hl,sandbox_bullet_position_table
ld a,5
ld ix,bullet
call position_multiple_game_object_instances
;
call begin_collision_tables
;
ld hl,sandbox_asteroid_position_table
ld a,3
ld ix,asteroid
call position_multiple_game_object_instances
ld a,3
ld ix,asteroid
call add_multiple_game_object_instances_to_collision_table

ld a,2
ld hl,sandbox_batch_draw_table
call batch_draw_game_objects

;
ld ix,bullet
ld b,BULLET_MAX
-:
  ; Only do this for active bullets!
  push bc
  call detect_collision_using_collision_table
  call c,handle_collision
  pop bc
  ld de,_sizeof_game_object
  add ix,de
djnz -
assertEqualsByte temp_byte, TRUE, "Test 6.1 failed"
assertEqualsWord temp_word, asteroid, "Test 6.2 failed"
ret


  load_buffer_from_game_objects:
    ; Load the collision buffer with hitbox data from the two game objects.
    ; Entry: IX & IY = Pointer to game objects.
    ;
    ; Exit:
    ; Uses: All registers, including IX & IY, collision_buffer.
    ld hl,collision_buffer
    call load_one_object    ; Load x, y, width and height from first object.
    push iy                 ; Load pointer to second object into IX.
    pop ix
    call load_one_object    ; Load hitbox data from second object.
  ret
  ;
    load_one_object:
      ; Load 4 bytes of hitbox data from object ram structure to buffer.
      ld a,(ix+game_object.x)               ; Get object's x-position.
      ld b,a
      ; Calculate and load hitbox x.
      ld a,(ix+game_object.hitbox_offset)   ; Get hitbox x-offset.
      and %11110000                         ; Apply mask to eliminate
      rra                                   ; y-offset (offset is 4 bits!).
      rra                                   ; Rotate x-offset bits into
      rra                                   ; position.
      rra
      add a,b                               ; Add offset to object's x-pos.
      ld (hl),a                             ; Load value into buffer.
      inc hl                                ; Advance buffer pointer.
      ; Calculate and load hitbox y.
      ld b,(ix+game_object.y)               ; Get and apply y-offset in a
      ld a,(ix+game_object.hitbox_offset)   ; similar way...
      and %00001111
      add a,b
      ld (hl),a
      inc hl
      ; Load hitbox width and height.
      ld a,(ix+game_object.hitbox_width)    ; Just copy to the buffer.
      ld (hl),a
      inc hl
      ld a,(ix+game_object.hitbox_height)
      ld (hl),a
      inc hl
    ret
    ;
  ;



detect_collision_using_buffer:
  ; Simple form of collision detection: Axis-aligned bounding box.
  ; If (rect1.x < rect2.x + rect2.width &&  (coll1)
  ; rect1.x + rect1.width > rect2.x &&      (coll2)
  ; rect1.y < rect2.y + rect2.height &&     (coll3)
  ; rect1.height + rect1.y > rect2.y)       (coll4)
  ;  --> collision detected!
  ; We perform these four tests on at a time. If any one test fails,
  ; there is no collision, and we return with carry reset. If no test fails
  ; then we have collision, and return with carry set.
  ; Uses: AF, B, collision_buffer
  ld ix,collision_buffer
  call coll1
  ret nc
  call coll2
  ret nc
  call coll3
  ret nc
  call coll4
  ret nc
  ; Collision! Return with carry set.
ret
  coll1:
    ; rect1.x < rect2.x + rect2.width
    ld a,(ix+4)
    ld b,(ix+6)
    add a,b
    ld b,a
    ld a,(ix+0)
    cp b
  ret
  coll2:
    ; rect1.x + rect1.width > rect2.x
    ld a,(ix+0)
    ld b,(ix+2)
    add a,b
    ld b,a
    ld a,(ix+4)
    sub b
  ret
  coll3:
    ; rect1.y < rect2.y + rect2.height
    ld a,(ix+5)
    ld b,(ix+7)
    add a,b
    ld b,a
    ld a,(ix+1)
    cp b
  ret
  coll4:
    ; rect1.height + rect1.y > rect2.y
    ld a,(ix+3)
    ld b,(ix+1)
    add a,b
    ld b,a
    ld a,(ix+5)
    cp b
  ret
  ;


detect_collision:
  ; Entry: IX = Pointer to game object 1
  ;        IY = Pointer to game object 2
  ; Exit: Carry flag is set or reset depending on overlap between the
  ;       hitboxes of the two game objects.
  ; Uses: Everything but IX.
  push ix
    call load_buffer_from_game_objects
    call detect_collision_using_buffer
  pop ix
ret



batch_collision_detect:
  ; Collision test a game object, pointed to by IX, against a table of other
  ; game objects in a table pointed to by HL. Return relevant info in
  ; registers that allows for further processing in case of a collision.
  ; Entry: HL = Batch collision test table
  ;        A  = Number of objects in collision test table.
  ;        IX = Pointer to game object
  ;
  ; Exit: 1) Carry set on collision, and (in case of collision):
  ;       2) IY points to object of collision.
  ;       3) Holds ID of the object (same as iy+game_object.id).
  ld b,a                                ; For each object in collision table.
  --:
    push bc                             ; Save loop counter.
      ld e,(hl)                         ; Get object pointer from table and
      inc hl                            ; save it in IY...
      ld d,(hl)
      inc hl
      push de
      pop iy
      ld b,(hl)                         ; How many instances of this object
      inc hl                            ; to test?
      inc hl                            ; Skip 1 byte in the collision table.
      -:                                ; For each instance, do...
        push bc                         ; Save counter.
          push iy                       ; Save pointer to object.
            ld a,(iy+game_object.state)
            cp GAME_OBJECT_ACTIVE
            jp nz,+
              push hl
              call detect_collision     ; Perform the collision test.
              pop hl
              jp nc,+                   ; Returns carry on collision.
                ld a,(iy+game_object.id); Load object id into A.
                pop iy                  ; Restore IY to point to obj.
                pop bc                  ; Align the stack...
                pop bc
                ret                       ; Exit func with carry set.
            +:                          ; Else - no collision here...
          pop iy                        ; Restore pointer to object.
          ld de,_sizeof_game_object     ; Point to next instance.
          add iy,de
        pop bc                          ; Restore instance loop counter.
      djnz -                            ; Loop through all instances...
    pop bc                              ; Restore object counter.
  djnz --                               ; Loop through all listed objects.
ret


detect_collision_using_collision_table:
  ; Simple form of collision detection: Axis-aligned bounding box.
  ; If (rect1.x < rect2.x + rect2.width &&  (coll1)
  ; rect1.x + rect1.width > rect2.x &&      (coll2)
  ; rect1.y < rect2.y + rect2.height &&     (coll3)
  ; rect1.height + rect1.y > rect2.y)       (coll4)
  ;  --> collision detected!
  ; We perform these four tests on at a time. If any one test fails,
  ; there is no collision, and we return with carry reset. If no test fails
  ; then we have collision, and return with carry set.
  ; Uses: AF, B, IY
  ;
  ; Determine relevant collision table.
  ld a,(ix+game_object.hitbox_offset)
  and %00001111
  ld b,(ix+game_object.y)
  add a,b
  cp COLLISION_TABLE_DIVIDER
  jp nc,+
    ld hl,top_collision_table
    jp ++
  +:
    ld hl,bottom_collision_table
  ++:
  push hl
  pop iy
  ;
  call coll1_2
  ret nc
  call coll2_2
  ret nc
  call coll3_2
  ret nc
  call coll4_2
  ret nc
  ; Collision! Return with carry set, and with pointer to object in table
  ; passed in IY:
  ld l,(iy+4)
  ld h,(iy+5)
  push hl
  pop iy
ret
  coll1_2:
    ; rect1.x < rect2.x + rect2.width
    ld a,(iy+1) ;
    ld b,(iy+2) ; !
    add a,b
    ld b,a
    ld a, (ix+game_object.hitbox_offset)
    and %11110000
    rra
    rra
    rra
    rra
    ld c,(ix+game_object.x)
    add a,c
    cp b
  ret
  coll2_2:
    ; rect1.x + rect1.width > rect2.x
    ld c,(ix+game_object.x)
    ld a,(ix+game_object.hitbox_offset)
    and %11110000
    rra
    rra
    rra
    rra
    add a,c
    ld b,(ix+game_object.hitbox_width)
    add a,b
    ld b,a
    ld a,(iy+1)
    sub b
  ret
  coll3_2:
    ; rect1.y < rect2.y + rect2.height
    ld a,(iy+0)
    ld b,(iy+3)
    add a,b
    ld b,a
    ld c,(ix+game_object.y)
    ld a,(ix+game_object.hitbox_offset)
    and %00001111
    add a,c
    cp b
  ret
  coll4_2:
    ; rect1.height + rect1.y > rect2.y
    ld c,(ix+game_object.y)
    ld a,(ix+game_object.hitbox_offset)
    and %00001111
    add a,c
    ld b,(ix+game_object.hitbox_height)
    add a,b
    ld b,a
    ld a,(iy+0)
    cp b
  ret
  ;


; 2017-09-02

test_4:

  ld ix,swabby
  ld hl,sandbox_swabby_init_table
  call initialize_game_object
  call draw_game_object
  ;
  ld ix,bullet
  ld hl,sandbox_bullet_init_table
  call initialize_game_object
  call draw_game_object
  ;
  call begin_collision_tables
  ;
  ld ix,asteroid
  ld hl,sandbox_asteroid_init_table
  call initialize_game_object
  call draw_game_object
  ld ix,asteroid
  call add_game_object_to_collision_table
  ;
  ld ix,bullet
  call detect_collision_using_collision_table
  push iy
  pop hl
  assertCarrySet "Test 4.1 failed"
  assertEqualsHL asteroid, "Test 4.2 failed"
ret


test_5:
  ld ix,swabby
  ld hl,sandbox_swabby_init_table
  call initialize_game_object
  call draw_game_object
  ;
  ;
  ld hl,sandbox_game_object_init_table
  ld a,1
  call batch_initialize_game_objects
  ld hl,sandbox_position_table
  ld a,5
  ld ix,bullet
  call position_multiple_game_object_instances
  ld a,5
  ld hl,sandbox_batch_draw_table
  call batch_draw_game_objects
  ;
  call begin_collision_tables
  ;
  ld ix,asteroid
  ld hl,sandbox_asteroid_init_table
  call initialize_game_object
  call draw_game_object
  ld ix,asteroid
  call add_game_object_to_collision_table
  ;
  ld ix,bullet
  ld b,BULLET_MAX
  -:
    ; Only do this for active bullets!
    push bc
    call detect_collision_using_collision_table
    call c,handle_collision
    pop bc
    ld de,_sizeof_game_object
    add ix,de
  djnz -
  assertEqualsByte temp_byte, TRUE, "Test 5.1 failed"
  assertEqualsWord temp_word, asteroid, "Test 5.2 failed"
ret


sandbox_shard_init_table:
  .db SPRITE_45, 1, 0, GAME_OBJECT_SHAPE_SMALL
  .db 0, 0, GAME_OBJECT_ACTIVE
  .db %00010001, 7, 7, SHARD           ; TODO: Might need adjustment!
sandbox_missile_init_table:
  .db SPRITE_4, 2, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
  .db 20, 10, GAME_OBJECT_ACTIVE
  .db %00010001, 14, 14, MISSILE     ; Might need adjustment!
sandbox_danish_init_table:
  .db SPRITE_18, 1, 0, GAME_OBJECT_SHAPE_REGULAR
  .db 30, 90, GAME_OBJECT_ACTIVE
  .db %00000000, 16, 16, DANISH           ; TODO: Might need adjustment!
sandbox_spinner_init_table:
  .db SPRITE_12, 1, 0, GAME_OBJECT_SHAPE_TALL
  .db 120, 180, GAME_OBJECT_ACTIVE
  .db %00000000, 16, 24, SPINNER

  sandbox_game_object_init_table:
    .dw asteroid, sandbox_asteroid_init_table, 1
  sandbox_asteroid_init_table_2:
    .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 90, 40, GAME_OBJECT_ACTIVE
    .db %00000000, 16, 16, ASTEROID
  sandbox_game_object_init_table_2:
    .dw asteroid, sandbox_asteroid_init_table_2, 1
    .dw bullet, sandbox_bullet_init_table, 1
    .dw swabby, sandbox_swabby_init_table, 1
    .dw danish, sandbox_danish_init_table, 1

    ;
    sandbox_batch_draw_table:
      .dw bullet, 1
      .dw shard, 2
      .dw swabby, 1
    sandbox_bullet_position_table:
      .db 50, 80, 90, 130
    sandbox_shard_position_table:
      .db 40, 70, 40, 90, 100, 100, 100, 140
    sandbox_batch_collision_test_table:
      .dw shard, 4
    ;
    sandbox_game_object_init_table_3:
      .dw asteroid, sandbox_asteroid_init_table_3, 1
      .dw bullet, sandbox_bullet_init_table, 1
      .dw swabby, sandbox_swabby_init_table, 1
    sandbox_asteroid_init_table_3:
      .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
      .db 0, 0, GAME_OBJECT_ACTIVE
      .db %00000000, 15, 15, ASTEROID


test_3_bullet_collides:
  ld hl,sandbox_game_object_init_table_2
  ld a,3
  call batch_initialize_game_objects

  ld ix,swabby
  call draw_game_object
  ;
  ld ix,bullet
  call draw_game_object
  ;
  call begin_collision_tables
  ;
  ld ix,asteroid
  call draw_game_object
  call add_game_object_to_collision_table
  ;
  ld ix,bullet
  ld hl,bottom_collision_table
  call test_game_object_against_collision_table
  assertCarrySet "Test 3.1 failed"
  ;
  ;
  ;
ret

; 2017-09-01
test_1_asteroid_in_top_table:
  ld hl,sandbox_game_object_init_table
  ld a,1
  call batch_initialize_game_objects

  ld ix,asteroid
  ;call draw_game_object
  ;
  call begin_collision_tables
  ld a,(objects_in_top_collision_table)
  assertEqualsA 0, "Test 1.0 failed"
  ;
  call add_game_object_to_collision_table
  ;
  ;
  call await_frame_interrupt
  ld a,(top_collision_table)
  assertEqualsA 40, "Test 1.1 failed"
  ld a,(top_collision_table+2)
  assertEqualsA 16, "Test 1.2 failed"

ret

test_2_asteroid_in_bottom_table:
  ld hl,sandbox_game_object_init_table_2
  ld a,1
  call batch_initialize_game_objects

  ld ix,asteroid
  call draw_game_object
  ;
  call begin_collision_tables
  ld a,(objects_in_bottom_collision_table)
  assertEqualsA 0, "Test 2.0 failed"
  ;
  call add_game_object_to_collision_table
  ;
  ;
  call await_frame_interrupt
  ld a,(bottom_collision_table)
  assertEqualsA 90, "Test 2.1 failed"
  ld a,(bottom_collision_table+2)
  assertEqualsA 16, "Test 2.2 failed"
ret


ld hl,sandbox_bullet_position_table
ld a,2 ; check!
ld ix,bullet
call position_multiple_game_object_instances

ld hl,sandbox_shard_position_table
ld a,4 ; check!
ld ix,shard
call position_multiple_game_object_instances

ld hl,sandbox_batch_draw_table
ld a,3 ; check!
call batch_draw_game_objects


; 2017-08-28

test_batch_collision_dectection:
  ld hl,sandbox_game_object_init_table
  ld a,2
  call batch_initialize_game_objects
  ld hl,sandbox_batch_draw_table
  ld a,2
  call batch_draw_game_objects
  ld ix,bullet
  ld hl,sandbox_batch_collision_test_table
  ld a,1
  call batch_collision_detect
  push iy
  pop hl
  assertCarrySet "Test 1.1 failed"
  assertEqualsHL asteroid, "Test 1.2 failed"
ret
;
test_2_no_collision:
  ld hl,sandbox_game_object_init_table_2
  ld a,2
  call batch_initialize_game_objects
  ld hl,sandbox_batch_draw_table_2
  ld a,2
  call batch_draw_game_objects
  ld ix,bullet
  ld hl,sandbox_batch_collision_test_table_2
  ld a,1
  call batch_collision_detect
  assertCarryReset "Test 2.1 failed"
ret

test_3_multiple_asteroids_no_collision:
  ld hl,sandbox_game_object_init_table_3
  ld a,2
  call batch_initialize_game_objects

  ld hl,sandbox_position_table_3
  ld a,3 ; check!
  ld ix,asteroid
  call position_multiple_game_object_instances

  ld a,2 ; check!
  ld hl,sandbox_batch_draw_table_3
  call batch_draw_game_objects

  ld ix,bullet
  ld hl,sandbox_batch_collision_test_table_3
  ld a,1
  call batch_collision_detect
  assertCarryReset "Test 3.1 failed"
ret


test_4_multiple_asteroids_and_collision:
  ld hl,sandbox_game_object_init_table_3
  ld a,2
  call batch_initialize_game_objects

  ld hl,sandbox_position_table_4
  ld a,3 ; check!
  ld ix,asteroid
  call position_multiple_game_object_instances

  ld a,2 ; check!
  ld hl,sandbox_batch_draw_table_3
  call batch_draw_game_objects

  ld ix,bullet
  ld hl,sandbox_batch_collision_test_table_3
  ld a,1
  call batch_collision_detect
  assertCarrySet "Test 4.1 failed"
  assertEqualsA ASTEROID, "Test 4.2 failed"
ret


sandbox_game_object_init_table:
  .dw bullet, sandbox_bullet_init_table, 1
  .dw asteroid, sandbox_asteroid_init_table, 1
sandbox_batch_draw_table:
  .dw bullet, 1
  .dw asteroid, 1
sandbox_batch_collision_test_table:
  .dw asteroid, 1
sandbox_bullet_init_table:
  .db SPRITE_44, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_SMALL
  .db 7, 7, GAME_OBJECT_ACTIVE
  .db %00100000, 4, 8
sandbox_asteroid_init_table:
  .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
  .db 4, 4, GAME_OBJECT_ACTIVE
  .db %00000000, 16, 16
;
sandbox_game_object_init_table_2:
.dw bullet, sandbox_bullet_init_table_2, 1
.dw asteroid, sandbox_asteroid_init_table_2, 1
sandbox_batch_draw_table_2:
.dw bullet, 1
.dw asteroid, 1
sandbox_batch_collision_test_table_2:
.dw asteroid, 1
sandbox_bullet_init_table_2:
.db SPRITE_44, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_SMALL
.db 7, 7, GAME_OBJECT_ACTIVE, BULLET
.db %00100000, 4, 8
sandbox_asteroid_init_table_2:
.db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
.db 24, 24, GAME_OBJECT_ACTIVE
.db %00000000, 16, 16, ASTEROID


sandbox_game_object_init_table_3:
  .dw bullet, sandbox_bullet_init_table_2, 1
  .dw asteroid, sandbox_asteroid_init_table_2, 3
sandbox_batch_draw_table_3:
  .dw bullet, 1
  .dw asteroid, 3
sandbox_batch_collision_test_table_3:
  .dw asteroid, 3
sandbox_position_table_3:
  .db 20, 10, 20, 30, 20, 50
sandbox_position_table_4:
  .db 20, 10, 5, 5, 20, 50



; ---
test_rect1_separate_from_rect2:
  ld ix,rect1
  ld hl,rect1_init_data_2
  call init_rect
  ld ix,rect2
  ld hl,rect2_init_data_2
  call init_rect
  ld ix,rect1
  ld iy,rect2
  call detect_collision
  assertCarryReset "Test 2 failed"
ret
test_rect2_under_rect1:
  ld ix,rect1
  ld hl,rect1_init_data_3
  call init_rect
  ld ix,rect2
  ld hl,rect2_init_data_3
  call init_rect
  ld ix,rect1
  ld iy,rect2
  call detect_collision
  assertCarryReset "Test 3 failed"
ret
test_rect2_overlaps_rect1:
  ld ix,rect1
  ld hl,rect1_init_data_4
  call init_rect
  ld ix,rect2
  ld hl,rect2_init_data_4
  call init_rect
  ld ix,rect1
  ld iy,rect2
  call detect_collision
  assertCarrySet "Test 4 failed"
ret
test_rect1_overlaps_rect2_version2:
  ld hl,test5_data
  call load_buffer
  call detect_collision_using_buffer
  assertCarrySet "Test 5 failed"
ret
test_rect1_separate_from_rect2_version2:
  ld hl,test6_data
  call load_buffer
  call detect_collision_using_buffer
  assertCarryReset "Test 6 failed"
ret
test_rect1_overlaps_rect2:
  ld ix,rect1
  ld hl,rect1_init_data
  call init_rect
  ld ix,rect2
  ld hl,rect2_init_data
  call init_rect
  ld ix,rect1
  ld iy,rect2
  call detect_collision
  assertCarrySet "Test 1 failed"
ret

test_rect1_overlaps_rect2:
  ld ix,bullet
  ld hl,sandbox_bullet_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_BULLET_Y
  ld b,SANDBOX_BULLET_X
  call set_game_object_position
  call activate_game_object
  ;call draw_game_object

  ld ix,asteroid
  ld hl,sandbox_asteroid_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_ASTEROID_Y
  ld b,SANDBOX_ASTEROID_X
  call set_game_object_position
  call activate_game_object
  ;call draw_game_object

  ld ix,bullet
  ld iy,asteroid
  call detect_collision
  assertCarrySet "Test 1 failed"
ret
;
test_rect1_separate_from_rect2:
  ld ix,bullet
  ld hl,sandbox_bullet_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_BULLET_Y
  ld b,SANDBOX_BULLET_X
  call set_game_object_position
  call activate_game_object
  ;call draw_game_object

  ld ix,asteroid
  ld hl,sandbox_asteroid_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_ASTEROID_Y_2
  ld b,SANDBOX_ASTEROID_X_2
  call set_game_object_position
  call activate_game_object
  ;call draw_game_object

  ld ix,bullet
  ld iy,asteroid
  call detect_collision
  assertCarryReset "Test 2 failed"
ret
;
test_rect2_separate_from_rect1:
  ld ix,bullet
  ld hl,sandbox_bullet_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_BULLET_Y
  ld b,SANDBOX_BULLET_X_2
  call set_game_object_position
  call activate_game_object
  call draw_game_object

  ld ix,asteroid
  ld hl,sandbox_asteroid_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_ASTEROID_Y
  ld b,SANDBOX_ASTEROID_X
  call set_game_object_position
  call activate_game_object
  call draw_game_object

  ld ix,bullet
  ld iy,asteroid
  call detect_collision
  assertCarryReset "Test 3 failed"
ret


rect1_init_data:
  .db 0, 0, 8, 8
rect2_init_data:
  .db 4, 0, 8, 8
rect1_init_data_2:
  .db 0, 0, 8, 8
rect2_init_data_2:
  .db 9, 0, 8, 8
rect1_init_data_3:
  .db 0, 0, 8, 8
rect2_init_data_3:
  .db 0, 9, 8, 8
rect1_init_data_4:
  .db 4, 4, 8, 8
rect2_init_data_4:
  .db 0, 0, 8, 8
test5_data:
  .db 0, 0, 8, 8
  .db 4, 0, 8, 8
test6_data:
  .db 0, 0, 8, 8
  .db 9, 0, 8, 8

  rect1 instanceof rect
  rect2 instanceof rect

  .struct rect
    x db
    y db
    width db
    height db
  .endst


  bullet_init_data:
    .db 0, 0
  asteroid_init_data:
    .db 4, 0
  bullet_init_data_2:
    .db 0, 0
  asteroid_init_data_2:
    .db 9, 0


    detect_collision:
      call coll1
      ret nc
      call coll2
      ret nc
      call coll3
      ret nc
      call coll4
      ret nc
      ; return with carry set.
    ret
      coll1:
        ; rect1.x < rect2.x + rect2.width
        ld a,(iy+0)
        ld b,(iy+2)
        add a,b
        ld b,a
        ld a,(ix+0)
        cp b
      ret
      coll2:
        ; rect1.x + rect1.width > rect2.x
        ld a,(ix+0)
        ld b,(ix+2)
        add a,b
        ld b,a
        ld a,(iy+0)
        sub b
      ret
      coll3:
        ; rect1.y < rect2.y + rect2.height
        ld a,(iy+1)
        ld b,(iy+3)
        add a,b
        ld b,a
        ld a,(ix+1)
        cp b
      ret
      coll4:
        ; rect1.height + rect1.y > rect2.y
        ld a,(ix+3)
        ld b,(ix+1)
        add a,b
        ld b,a
        ld a,(iy+1)
        cp b
      ret

    init_rect:
      ; ix = rect, HL = init data
      ld a,(hl)
      ld (ix+0),a
      inc hl
      ld a,(hl)
      ld (ix+1),a
      inc hl
      ld a,(hl)
      ld (ix+2),a
      inc hl
      ld a,(hl)
      ld (ix+3),a
      inc hl
    ret
    ;
    load_buffer:
      ; hl = pointer to buffer
      ld de,collision_buffer
      ld bc,8
      ldir
    ret
