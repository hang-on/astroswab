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
  ;call test_print_unsorted_score_table
  call test_print_sorted_and_repositioned_score_array
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
test_print_sorted_and_repositioned_score_array:
  ; Init
  ld a,NUMBER_OF_HISCORE_ITEMS
  ld ix,hi_score
  ld hl,hi_score_init_table
  call init_score_array
  ; TODO

  ;
  ; Reposition and sort.
  ld a,NUMBER_OF_HISCORE_ITEMS
  ld b,HISCORE_TABLE_TOP_POSITION
  ld hl,hi_score_name_table_address_table
  ld ix,hi_score
  call set_score_name_table_addresses_from_table_sort_by_rank
  ; Print
  ld a,NUMBER_OF_HISCORE_ITEMS
  ld ix,hi_score
  call print_score_array
ret
  hi_score_name_table_address_table:
    .dw $3952, $3952+$40, $3952+$80
  ;
  hi_score_init_table:
    .db 0         ; Id.
    .db 3
    .dw $3800     ; Name table address for fast printing.
    .asc "00001"  ; Init value.
    .asc "FAR"    ; Initials
    .db 1         ; Id.
    .db 2
    .dw $3840     ; Name table address for fast printing.
    .asc "00002"  ; Init value.
    .asc "MOR"    ; Initials
    .db 2         ; Id.
    .db 1
    .dw $3880     ; Name table address for fast printing.
    .asc "00003"  ; Init value.
    .asc "LAS"    ; Initials

;
