; gs_hiscore.inc
; ---------------------------------------------------------------------------
; H I S C O R E
; ---------------------------------------------------------------------------
prepare_hiscore:
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
  SELECT_BANK HISCORE_BANK
  ; Background graphics
  ld hl,hiscore_background_table
  call load_vram_from_table       ; Load the tiles.
  call load_vram_from_table       ; Load the tilemap.

  ; Stop music and sound effects.
  call PSGSFXStop
  call PSGStop
  ;
  ;
  ld hl,HISCORE_EXTRAM_ADDRESS
  call load_hiscore_table_from_extram
  ;
  ; Preset peak score for testing.
  .ifdef DEVELOPER_MODE
    ld hl,peak_score_test_data
    ld de,peak_score
    call copy_score_and_increment_pointers
    jp +
      peak_score_test_data:
        .asc "???"
    +:
  .endif
  ;
  ; Point to the last hiscore item...
  ld de,_sizeof_score_struct
  ld b,NUMBER_OF_HISCORE_ITEMS-1  ; Check if this is correct.
  ld hl,hiscore_item.1
  -:
    add hl,de
  djnz -
  ; Now hl points to the last hiscore item.
  ; Compare against peak score.
  ld iy,peak_score
  push hl
  pop ix
  call compare_scores
  jp nc,+
    ; Player broke on one the highscores -> enter new hiscore mode
    ld a,TRUE
    ld (new_hiscore),a
    jp ++
  +:
    ; Player did not break hiscore -> read-only mode
    ld a,FALSE
    ld (new_hiscore),a
    ld hl,transition_trigger_init_table
    ld ix,transition_trigger
    call initialize_trigger
    call enable_trigger
  ++:
  ;
  ld hl,peak_score                      ; Rebuild the hiscore table againt the
  call rebuild_hiscore_table            ; player's peak score.
  ;
  ; --------------------
  ld hl,peak_score
  call get_hiscore_table_postion_from_score
  jp c,+
    ; Carry not set - peak score is in the hiscore table.
    SELECT_EXTRAM
    ;ld a,1
    ld (RESERVED_BYTE_1),a
    SELECT_ROM
    jp ++
  +:
    ; Carry set: Peak score is NOT in the hiscore table.
    SELECT_EXTRAM
    ld a,0
    ld (RESERVED_BYTE_1),a
    SELECT_ROM
  ++:
  ; --------------------

  ld a,HISCORE_TABLE_ROW                ; Print table.
  ld b,HISCORE_TABLE_COLUMN
  call print_hiscore_table
  ;
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,1
  call set_register
  ei
  call FadeInScreen
  ; When all is set, change the game state.
  ld a,GS_RUN_HISCORE
  ld (game_state),a
jp main_loop
;
; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
run_hiscore:
  ;
  -:
    call await_frame_interrupt
    in a,(V_COUNTER_PORT)
    cp FIRST_LINE_OF_VBLANK+1
  jp nz,-
  ;
  call load_sat
  ;
  ; End of (unsafe) VDP-updating...
  ;
  call get_input_ports
  call begin_sprites
  ; Switch case depending on whether we have a new hiscore.
  ld a,(new_hiscore)
  cp TRUE
  jp nz,hiscore_read_only
    ; New hiscore..
    ; Handle initials input...
    ;
    ; Save hiscore table and exit when player press button.
    call is_button_1_pressed
    jp nc,+
      ld hl,HISCORE_EXTRAM_ADDRESS
      call save_hiscore_table_to_extram
      ;
      call transition_to_devmenu
    +:
  jp ++
  hiscore_read_only:
    ; No new hiscore.
    ; Display hiscore table until timer depletes, then transition...
    .ifndef DEVELOPER_MODE             ; Disable timer while in developer mode.
      ld ix,transition_trigger
      call process_trigger
      call c,transition_to_devmenu
    .endif
  ;
  ++:

  call PSGSFXFrame
  call PSGFrame
  ;
  ld hl,frame_counter
  inc (hl)
  .ifdef DEVELOPER_MODE
    call is_reset_pressed
    call c,transition_to_devmenu
  .endif

jp main_loop
  ;
  transition_to_devmenu: ; TODO: Make game state transition func.
    call PSGSFXStop
    call PSGStop
    call FadeOutScreen
    ld a,GS_PREPARE_DEVMENU
    ld (game_state),a
  ret
