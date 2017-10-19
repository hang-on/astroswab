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
  call test_copy_score
  call test_init_hiscore
  call test_increment_pointers
  ;
jp main_loop
;
; Tests for the sandbox:
; -----------------------------------------------------------------------------
init_tests:
; -----------------------------------------------------------------------------
  ;
ret
;
test_copy_score:
  ld hl,score_1_init
  ld de,score_1
  call copy_score
  ld ix,score_1
  ld a,(ix+score_struct.ones)
  assertEqualsA ASCII_ONE,"copy_score failed"
ret
test_init_hiscore:
  ld b,NUMBER_OF_HISCORE_ITEMS
  ld hl,hiscore_init
  ld de,hiscore_item
  ld bc,_sizeof_score_struct
  -:
    call copy_score
  djnz -
  ;
  ld ix,hiscore_item.1
  ld a,(ix+score_struct.ones)
  assertEqualsA ASCII_ZERO+4,"init_hiscore failed (1)"
  ;
  ld ix,hiscore_item.3
  ld a,(ix+score_struct.ones)
  assertEqualsA ASCII_ZERO+2,"init_hiscore failed (2)"
ret
;
test_increment_pointers:
  ld hl,hiscore_item
  ld (hiscore_table_pointer),hl
  assertEqualsHL hiscore_item,"Increment pointers failed (1)"
  ld hl,hiscore_buffer
  ld (hiscore_buffer_pointer),hl
  assertEqualsHL hiscore_buffer,"Increment pointers failed (2)"
ret

  ;
  score_1_init:
    .asc "00001"
    .asc "FAR"
    ;
  hiscore_init:
    .asc "00004"
    .asc "LAS"
    .asc "00003"
    .asc "LUN"
    .asc "00002"
    .asc "MOR"

;
