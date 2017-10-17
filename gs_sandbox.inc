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
  ;call test_print_score_w_initials
  call test_print_unsorted_score_table

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
test_print_unsorted_score_table:
  ;
  ld de,score_1
  ld hl,score_init_example_1
  call init_score
  ld de,score_2
  ld hl,score_init_example_2
  call init_score
  ;
  ld a,2
  ld hl,my_score_table
  call calc_vram_addresses_from_item_order


  ld a,2 ; score structs in table
  ld hl,my_score_table
  call print_score_table
ret
  my_score_table:
  .dw $0000, score_1, score_2
;
;
