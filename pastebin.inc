; 2017-09-02

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
  call add_object_to_collision_table
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
  call add_object_to_collision_table
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
  call add_object_to_collision_table
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
