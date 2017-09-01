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
  call test_1_asteroid_in_top_table
  call test_2_asteroid_in_bottom_table

  ;
jp main_loop
;
; Tests for the sandbox:
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
