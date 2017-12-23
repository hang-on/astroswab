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
<<<<<<< HEAD
  ; Initialize variables.
  ld a,INITIAL_DIFFICULTY           ; Set difficulty.
  ld (difficulty),a
  ld a,TRUE
  ld (gun_level_flag),a
  ld a,GUN_LEVEL_INIT               ; Reset gun
  ld (gun_level),a

  ;
  call setup_level_graphics
=======
  ld a,ASCII_SPACE
  ld b,TILE_BANK_1
  call reset_name_table
>>>>>>> parent of c875fd5... Add console and sandbox to dev menu
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
  ;
  call load_sat
  ;
  ld a,(gun_level_flag)
  cp TRUE
  call z,print_gun_level
  ;
  ; End of VDP-updating...
  call get_input_ports
  call begin_sprites
  ;
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

  ;
ret
; -----------------------------------------------------------------------------
; Update() tests:

<<<<<<< HEAD
;
;
=======

>>>>>>> parent of c875fd5... Add console and sandbox to dev menu
;
