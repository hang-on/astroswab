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
