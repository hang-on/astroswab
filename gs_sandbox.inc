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
  -:
    call await_frame_interrupt
    in a,(V_COUNTER_PORT)
    cp FIRST_LINE_OF_VBLANK+1
  jp nz,-

  call load_sat
  ; End of VDP-updating...
  call get_input_ports
  call begin_sprites
  ;
  ld b,SANDBOX_LOGGER_START_ROW
  call reset_logger
  ;
  ; Tests below:
  ; call test_3_bullet_collides
  ;call test_4
  ;call test_5
  call test_6
  ;
jp main_loop
;
; Tests for the sandbox:
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
  call add_object_to_collision_table
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
  call add_object_to_collision_table
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
sandbox_batch_draw_table:
  .dw bullet, 5
sandbox_position_table:
  .db 10, 10, 30, 20, 50, 30, 70, 40, 90, 50

  test_6:
    ; Test 6 is a clone of test 5, testing new coll.detect function.
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
    call add_object_to_collision_table
    ;
    ld ix,bullet
    ld b,BULLET_MAX
    -:
      ; Only do this for active bullets!
      push bc
      call v3_detect_collision_using_collision_table
      call c,handle_collision
      pop bc
      ld de,_sizeof_game_object
      add ix,de
    djnz -
    assertEqualsByte temp_byte, TRUE, "Test 6.1 failed"
    assertEqualsWord temp_word, asteroid, "Test 6.2 failed"
  ret
