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
  ld a,21
  ld b,1
  call set_cursor
  ld a,(temp_byte)
  call print_register_a
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
test_1:
  ld hl,sandbox_game_object_init_table
  ld a,6
  call batch_initialize_game_objects

  ld hl,sandbox_bullet_position_table
  ld a,5 ; check!
  ld ix,bullet
  call position_multiple_game_object_instances

  ld hl,sandbox_shard_position_table
  ld a,8 ; check!
  ld ix,shard
  call position_multiple_game_object_instances

  ld hl,sandbox_batch_draw_table
  ld a,6 ; check!
  call batch_draw_game_objects

  in a,(V_COUNTER_PORT)
  ld b,a
  ld ix,bullet
  push bc
    ld b,3
    -:
      push bc
        ld hl,sandbox_batch_collision_test_table
        ld a,4
        call batch_collision_detect
        ld de,_sizeof_game_object
        add ix,de
      pop bc
    djnz -
    assertCarryReset "Test_1 failed"
  pop bc
  in a,(V_COUNTER_PORT)
  sub b
  ld (temp_byte),a



ret
