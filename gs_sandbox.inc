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
  ; Bullet vs. asteroid tests below:
  ld b,SANDBOX_LOGGER_START_ROW
  call reset_logger
  ;
  call test_rect1_overlaps_rect2
  call test_rect1_separate_from_rect2
  call test_rect2_separate_from_rect1
  ;
jp main_loop
;
; Tests for the sandbox:
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
