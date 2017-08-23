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
