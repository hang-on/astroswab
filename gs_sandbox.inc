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
  ;call test_print_sorted_and_repositioned_score_array
  ;
jp main_loop
;
; Tests for the sandbox:
; -----------------------------------------------------------------------------
init_tests:
; -----------------------------------------------------------------------------
  call test_compare_hiscore_table_and_player_score
  ;
ret
;
test_compare_hiscore_table_and_player_score:
  ld a,21
  ld b,4
  call set_cursor


  ; Init hiscore table
  ld a,NUMBER_OF_HISCORE_ITEMS
  ld ix,hi_score
  ld hl,hi_score_init_table
  call init_score_array
  ; Init player score
  ld de,player_score
  ld hl,luna_score
  call init_score
  ;
  ; compare scores
  ld iy,player_score
  ld ix,hi_score ; should be hi_score_table
  ld a,NUMBER_OF_HISCORE_ITEMS
  ;ld b,HISCORE_TABLE_TOP_POSITION
  call calculate_address_and_rank_in_score_table
  call insert_score_into_score_table
  ;
  ; Print hiscore table
  ld a,NUMBER_OF_HISCORE_ITEMS
  ld ix,hi_score
  call print_score_array
  ; print player score
  ld a,INCLUDE_INITIALS
  ld hl,player_score
  call print_score

ret

  luna_score:
    .db 3         ; Id.
    .db 0         ; unused
    .dw $3800     ; Name table address for fast printing.
    .asc "00001"  ; Init value.
    .asc "LUN"    ; Initials
    ;
  hi_score_name_table_address_table:
    .dw $3952, $3952+$40, $3952+$80
    ;
  hi_score_init_table:
    ;
    .db 2         ; Id.
    .db 0         ; Unused
    .dw $3952     ; Name table address for fast printing.
    .asc "00003"  ; Init value.
    .asc "LAS"    ; Initials
    ;
    .db 1         ; Id.
    .db 0
    .dw $3952+$40 ; Name table address for fast printing.
    .asc "00002"  ; Init value.
    .asc "MOR"    ; Initials
    ;
    .db 0         ; Id.
    .db 0
    .dw $3952+$80    ; Name table address for fast printing.
    .asc "00001"  ; Init value.
    .asc "FAR"    ; Initials

;
