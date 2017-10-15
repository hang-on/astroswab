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
  ;call test_all_digits_reset
  ;call test_add_1
  ;call test_add_1_then_9
  ;call test_add_1_then_9_visual
  ;call test_reset_visual
  ;call test_add_1_then_9_visual
  call test_add_1_visual
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
test_add_1_visual:
  ld hl,score_init_data
  ld de,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call add_to_score
  ;
  ld hl,player_score
  call print_score
ret
score_init_data: ; Should be the phenomena, i.e. "killed boss"
  .dw $3aa0
  .asc "00000"

test_add_1_then_9_visual:
  ld hl,score_init_data_example
  ld de,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call add_to_score
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,9
  call add_to_score
  ;
  ld hl,player_score
  call print_score
ret

;
test_add_1_then_9:
  ld hl,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call add_to_score
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,9
  call add_to_score
  ;
  ld a,SCORE_ONES
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"test_add_1_then_9 failed"
  ld a,SCORE_TENS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ONE,"test_add_1_then_9 failed"
ret


test_add_1:
  ld hl,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call add_to_score
  ;
  ld a,SCORE_ONES
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ONE,"test_add_1_failed"
ret
;
test_all_digits_reset:
  ld hl,player_score
  call init_score
  ld a,SCORE_ONES
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_all_digits_reset"
  ld a,SCORE_TENS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_all_digits_reset"
  ld a,SCORE_HUNDREDS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_all_digits_reset"
  ld a,SCORE_THOUSANDS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_all_digits_reset"
  ld a,SCORE_TEN_THOUANDS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_all_digits_reset"
ret
