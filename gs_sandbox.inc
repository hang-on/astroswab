; gs_sandbox.inc
; ---------------------------------------------------------------------------
; S A N D B O X
; ---------------------------------------------------------------------------
prepare_sandbox:
  di
  ; Turn off display and frame interrupts.
  ld a,DISPLAY_0_FRAME_0_SIZE_0
  ld b,1
  call set_register
  ;
  ld a,ASCII_SPACE
  ld b,TILE_BANK_1
  call reset_name_table
  ;
  SELECT_BANK SPRITE_BANK
  ld bc,sprite_tiles_end-sprite_tiles
  ld de,SPRITE_BANK_START
  ld hl,sprite_tiles
  call load_vram
  ; Wipe sprites.
  call begin_sprites
  call load_sat
  ; Stop music and sound effects.
  call PSGSFXStop
  call PSGStop
  ; ----------------------------------------------------------
  ; ----------------------------------------------------------
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,1
  call set_register
  ei
  ; When all is set, change the game state.
  ld a,GS_RUN_SANDBOX
  ld (game_state),a
jp main_loop
; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
run_sandbox:
  ;
  call await_frame_interrupt
  call load_sat
  ; End of VDP-updating...
  call get_input_ports
  call begin_sprites
  ;
  ld b,SANDBOX_LOGGER_START_ROW
  call reset_logger
  ;
  ; Tests below:
  ;call test_batch_collision_dectection
  ;call test_2_no_collision
  ;call test_3_multiple_asteroids_no_collision
  call test_4_multiple_asteroids_and_collision
  ;
jp main_loop
;
; Tests for the sandbox:
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
