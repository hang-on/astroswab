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
        .asc "01900"
        .asc "???"
    +:
  .endif
  ;
  ld ix,cursor_effect_trigger
  ld hl,cursor_effect_trigger_init_table
  call initialize_trigger
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
  ld a,HISCORE_TABLE_ROW                ; Print table.
  ld b,HISCORE_TABLE_COLUMN
  call print_hiscore_table
  ;
  ; --------------------
  ld a,(new_hiscore)
  cp TRUE
  jp nz,end_new_hiscore_prep
    ; Place cursor at the first char in player initials.
    ld hl,peak_score
    call get_hiscore_table_postion_from_score
    add a,(TOP_POSITION_ROW-1)
    ld b,TOP_POSITION_COLUMN
    call set_cursor
    ;
    ; Initialize the cursor effect.
    ld ix,cursor_effect_trigger
    ld a,ENABLED
    ld (ix+trigger.state),a
    ;
    ld ix,cursor_effect
    ld hl,cursor_effect_init_table
    call initialize_game_object
    ;
    ld ix,release_keys_trigger
    ld hl,release_keys_trigger_init_table
    call initialize_trigger
    ;
    ; Position the cursor effect over the first char in initials.
    ld hl,peak_score
    call get_hiscore_table_postion_from_score
    ld b,a
    ld ix,cursor_effect
    ld a,(ix+game_object.y)
    -:
      add a,8
    djnz -
    ld (ix+game_object.y),a
  end_new_hiscore_prep:
  ; --------------------
  ;

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
  ;
  ld ix,cursor_effect
  call draw_game_object
  ;
  ; Switch case depending on whether we have a new hiscore.
  ld a,(new_hiscore)
  cp TRUE
  jp nz,hiscore_read_only
    ; New hiscore..
    ; Handle initials input...
    ld ix,cursor_effect_trigger
    call process_trigger
    ld ix,cursor_effect
    call c,toggle_game_object_state
    ;
    ; Forward to next char or save/exit when player presses button 1
    ld ix,release_keys_trigger
    call process_trigger
    jp nc,+
      ld a,DISABLED
      ld (ix+trigger.state),a
    +:
    ;
    ld a,(ix+trigger.state)
    cp DISABLED
    jp nz,+
      call is_button_1_pressed
      jp nc,+
        ld ix,cursor_effect
        ld a,(ix+game_object.x)
        cp CURSOR_EFFECT_MAX_X
        jp z,save_and_exit
          add a,ONE_CHAR_WIDTH
          ld (ix+game_object.x),a
          ld ix,release_keys_trigger
          ld a,ENABLED
          ld (ix+trigger.state),a
          jp +
          save_and_exit:
            ld a,GS_PREPARE_DEVMENU
            call transition_to_gamestate
            jp main_loop
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
  transition_to_gamestate:
    ; Transition to a gamestate passed in A.
    ; Entry: A = Game state.
    ; Exit: None
    ; Uses: ?
    push af
      call PSGSFXStop
      call PSGStop
      call FadeOutScreen
    pop af              ;ld a,GS_PREPARE_DEVMENU
    ld (game_state),a
  ret
