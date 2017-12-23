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
  ; Initialize variables.
  ld a,INITIAL_DIFFICULTY           ; Set difficulty.
  ld (difficulty),a
  ld a,TRUE
  ld (gun_level_flag),a
  ld a,GUN_LEVEL_INIT               ; Reset gun
  ld (gun_level),a

  ;
  call setup_level_graphics
  ;
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
  ;
  call FadeInScreen
  ;
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
  call is_reset_pressed
  jp nc,+
    ld a,GS_PREPARE_DEVMENU
    call transition_to_gamestate
  +:
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

difficulty_to_random_background:
  ; Load a random background depending on difficulty.
  ; Entry: None. (Uses the difficulty global variable).
  ; Exit: None.
  ; Uses: Everything except index registers.
  ;
  ld a,(difficulty)
  ld b,(difficulty_to_background_bank_table_end-difficulty_to_background_bank_table)/2
  ld hl,difficulty_to_background_bank_table
  call convert_byte
  ld (SLOT_2_CONTROL),a           ; Page-in the correct background bank.
  ;
  ld a,(difficulty)               ; Get current difficulty as index.
  ld hl,meta_background_table     ; Point HL to base of meta pointer table.
  call get_word_from_table        ; Point HL to base of table of backgrounds.
  push hl                         ; Save this pointer.
    call get_random_number        ; Get random number.
  pop hl                          ; Restore pointer.
  and BACKGROUND_TABLE_MASK       ; Apply mask to random number to get index.
  call get_word_from_table        ; Point HL to random background table at
  ;                               ; this difficulty.
  call load_vram_from_table       ; Load the tiles.
  call load_vram_from_table       ; Load the tilemap.
ret
;
;
;
setup_level_graphics:
  ; Load various assets into vram.
  ; Entry: None - uses consts and globals.
  ; Exit: None.
  ; Uses: ?
  ;
  call difficulty_to_random_background
  call load_sprites
  ; Set border color.
  ld a,DARK_BLUE
  ld b,REGISTER_7
  call set_register
  ; Print the dummy text under the playfield.
  ld b,DUMMY_TEXT_ROW
  ld c,DUMMY_TEXT_COLUMN
  ld hl,dummy_text
  call print
  ; Print the lives meter.
  ld hl,lives_string
  ld b,(lives_string_end-lives_string-2)/2
  call put_string
ret
;
;
;
print_gun_level:
  ; Print the ASCII version of the current gun level (POW:) to the name table.
  ; If we are at max gun level, print the word defined in gun_level_max_string.
  ; It is set up to be "MAX" but can be changed.
  ; Entry: None.
  ; Exit: None.
  ; Uses: ?
  ;
  ld a,(gun_level)
  cp GUN_LEVEL_MAX
  jp nz,+                     ; If gun max level is reached, print "MAX".
    ld hl,gun_level_max_string
    ld b,((gun_level_max_string_end-gun_level_max_string)-2)/2
    call put_string
    ret
  +:                          ; Else, just print the new gun level.
  ld hl,GUN_LEVEL_NAMETABLE_ADDRESS
  call set_vram_address
  ld a,(gun_level)
  add a,ASCII_ZERO
  out (DATA_PORT),a
  ;
  ld a,FALSE
  ld (gun_level_flag),a
ret
