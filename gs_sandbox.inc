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
  ;call test_rebuild_hiscore_table_against_fine_score
  ;call test_rebuild_against_fine_and_print
  ;
jp main_loop
;
; Tests for the sandbox:
; -----------------------------------------------------------------------------
init_tests:
; -----------------------------------------------------------------------------
  ; Tests below:
  call test_rebuild_against_fine_and_print
  ;
ret
; -----------------------------------------------------------------------------
test_rebuild_against_fine_and_print:
  ld hl,fine_score_init
  ld de,player_score
  call copy_score_and_increment_pointers
  ;
  ld hl,hiscore_init
  ld de,hiscore_item.1
  call copy_hiscore_table
  ;
  ld hl,player_score
  call rebuild_hiscore_table
  ;
  ld a,HISCORE_TABLE_ROW
  ld b,HISCORE_TABLE_COLUMN
  call print_hiscore_table
  ;
ret
;
;
multe_score_init:
  .asc "00001"
  .asc "MUL"
fine_score_init:
  .asc "00010"
  .asc "FIN"
  ;
hiscore_init:
  .asc "00100"
  .asc "LAS"
  .asc "00090"
  .asc "LUN"
  .asc "00080"
  .asc "MOR"
  .asc "00070"
  .asc "FAR"
  .asc "00060"
  .asc "FEL"


;
