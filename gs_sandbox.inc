; gs_sandbox.inc
; .
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
  call init_tests
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
  call test_1
  ;
jp main_loop
;
; Tests for the sandbox:
init_tests:
  ;
  ld hl,sandbox_game_object_init_table
  ld a,2
  call batch_initialize_game_objects
  ld a,2
  ld ix, shard
  ld hl,sandbox_shard_position_table
  call position_multiple_game_object_instances
  ld hl,sandbox_trigger_init_table
  ld ix,sandbox_trigger
  call initialize_trigger
  ld a,ENABLED
  ld (ix+trigger.state),a
ret
test_1:
  ld ix,sandbox_trigger
  ld a,(ix+trigger.state)
  cp ENABLED
  jp nz,++
    call process_trigger
    jp nc,+
      ;
      push ix
        ld ix,asteroid
        ld a,DISABLED
        ld (ix+game_object.state),a
      pop ix
      ld a,DISABLED
      ld (ix+trigger.state),a
    +:
  ++:

  ;
  ld b,2
  ld ix,shard
  -:
    push bc
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    jp nz,+
      call move_game_object              ; Move shard.
      ld a,SHARD_DEACTIVATE_ZONE_START
      ld b,SHARD_DEACTIVATE_ZONE_END
      ;call horizontal_zone_deactivate_game_object
      ;
      call draw_game_object              ; Put it in the SAT.
      ;call add_game_object_to_collision_table
    ;
    +:
    ld de,_sizeof_game_object
    add ix,de
    pop bc
  djnz -

  ld ix,asteroid
  ld a,(ix+game_object.state)
  cp GAME_OBJECT_ACTIVE
  jp nz,+
    call draw_game_object
  +:

  call is_reset_pressed
  jp nc,+
    ld a,GS_PREPARE_SANDBOX
    ld (game_state),a
  +:

ret
  sandbox_game_object_init_table:
    .dw shard, sandbox_shard_init_table, 2
    .dw asteroid, sandbox_asteroid_init_table, 1
  sandbox_shard_init_table:
    .db SPRITE_45, 1, 0, GAME_OBJECT_SHAPE_SMALL
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00010001, 7, 7, SHARD
  sandbox_asteroid_init_table:
    .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 50, 50, GAME_OBJECT_ACTIVE
    .db %00000000, 15, 15, ASTEROID
  sandbox_shard_position_table:
    .db 35, 70, 45, 80
  sandbox_trigger_init_table:
    .db 255, 100
