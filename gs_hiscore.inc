; gs_hiscore.inc
; ---------------------------------------------------------------------------
prepare_hiscore:
; ---------------------------------------------------------------------------
  di
  ;
  call disable_display_and_sound
  ;
  call ClearVRam
  ld hl,hiscore_arrange_vdp_parameters
  call arrange_vdp

  xor a
  ld b,VERTICAL_SCROLL_REGISTER
  call set_register
  ;
  ;
  ld hl,HISCORE_EXTRAM_ADDRESS
  call load_hiscore_table_from_extram
  ;
  ld ix,cursor_effect_trigger
  ld hl,cursor_effect_trigger_init_table
  call initialize_trigger
  ;
  ; --------------------------------------------------------------------------
  ; Test: Is the player's peak score at least equal to the bottommost entry in
  ; the hiscore table?
  ld de,_sizeof_score_struct        ; Point HL to the last hiscore item...
  ld b,NUMBER_OF_HISCORE_ITEMS-1
  ld hl,hiscore_item.1
  -:
    add hl,de
  djnz -
  ;                                 ;
  ld iy,peak_score                  ; Put ptr to peak_score in IY.
  push hl                           ; Put ptr to last hiscore table item in IX.
  pop ix
  call compare_scores               ; And compare them...
  jp nc,+
    ; Player broke on one the highscores -> enter new hiscore mode.
    ld a,TRUE
    ld (new_hiscore),a
    jp ++
  +:
    ; Player did not break hiscore -> read-only mode.
    ld a,FALSE
    ld (new_hiscore),a
  ++:
  ;
  ld hl,transition_trigger_hiscore_init_table ; In read-only mode, the hiscore table
  ld ix,transition_trigger            ; displays for a short period of time,
  call initialize_trigger             ; and then we make a transition back
  call enable_trigger                 ; to te title screen.
  ;
  ld hl,peak_score                      ; Rebuild the hiscore table againt the
  call rebuild_hiscore_table            ; player's peak score.
  ;
  ld a,HISCORE_TABLE_ROW                ; Print table.
  ld b,HISCORE_TABLE_COLUMN
  call print_hiscore_table
  ;
  ; ---------------------------------------------------------------------------
  ; Only when we have a new hiscore - and the player must enter initials...
  ld a,(new_hiscore)
  cp TRUE
  jp nz,end_new_hiscore_prep
    ; Place cursor at the first char in player initials.
    ld hl,peak_score
    call get_hiscore_table_postion_from_score
    add a,(TOP_POSITION_ROW-1)
    ld b,TOP_POSITION_COLUMN
    call set_cursor
    ld a,1
    ld (active_initial),a
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
    ; Position the cursor effect over the first char in initials.
    ld hl,peak_score
    call get_hiscore_table_postion_from_score
    ld b,a
    ld ix,cursor_effect
    ld a,(ix+game_object.y)
    -:
      add a,ONE_CHAR_WIDTH
    djnz -
    ld (ix+game_object.y),a
    ; Select the default char.
    ld a,DEFAULT_CHAR
    ld (selected_char),a
    ; Put the char to the screen.
    call convert_and_put_selected_char
    ld hl,cursor_x
    dec (hl)
    ;
    ; Debouncing keys.
    ld ix,release_keys_trigger
    ld hl,release_keys_trigger_init_table
    call initialize_trigger
    ;

  end_new_hiscore_prep:
  ;
  ; Un-blank left column (mostly for emulators)
  ld a,SCROLL_0__LCB_0_LINE_0_SPRITES_0
  ld b,REGISTER_0
  call set_register
  ;
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,REGISTER_1
  call set_register
  ;
  ei
  call FadeInScreen
  ;
  ld a,(new_hiscore)
  cp TRUE
  jp nz,+
    ld hl,hiscore_track
    call PSGPlay
  +:
  ;
  ; When all is set, change the game state.
  ld a,GS_RUN_HISCORE
  ld (game_state),a
jp main_loop
;
;
;
; ---------------------------------------------------------------------------
run_hiscore:
; ---------------------------------------------------------------------------
  ;
  call await_frame_interrupt
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
  ;
  ; Switch case depending on whether we have a new hiscore.
  ld a,(new_hiscore)
  cp TRUE
  jp nz,hiscore_read_only
; *****************************************************************************
;   New hiscore (edit mode).
; *****************************************************************************
    ; New hiscore input mode. Flash the cursor effect game object, and process
    ; input from buttons and the d-pad.
    ;
    ld ix,cursor_effect_trigger         ; Activate/deactivate cursor effect
    call process_trigger                ; game object in order to create the
    ld ix,cursor_effect                 ; blinking effect.
    call c,toggle_game_object_state
    ;
    ; Process keylock trigger.
    ld ix,release_keys_trigger
    call process_trigger
    jp nc,+
      ld a,DISABLED                     ; If time is up, release keylock.
      ld (ix+trigger.state),a           ; (Anti bouncing measure).
    +:
    ;
    ld a,(ix+trigger.state)             ; Is keylock disabled?
    cp DISABLED
    jp nz,end_of_input_tests            ; If not, then skip the input tests.
      ; -----------------------------------------------------------------------
      call is_button_1_pressed
      ; -----------------------------------------------------------------------
      ; Button 1 means: Stamp out currently selected char. Save and switch to
      ; read-only mode if we are at the last initial.
      jp nc,+
        ld a,(active_initial)
        cp LETTERS_IN_INITIALS
        jp z,save_and_switch_to_read_only_mode
          ld ix,cursor_effect
          ld a,(ix+game_object.x)
          add a,ONE_CHAR_WIDTH
          ld (ix+game_object.x),a
          ;
          ld ix,release_keys_trigger
          ld a,ENABLED
          ld (ix+trigger.state),a
          ;
          call convert_and_put_selected_char_and_update_peak_score_struct
          ; Forward to next active initial
          ld hl,active_initial
          inc (hl)
          ; Put char under cursor effect.
          call convert_and_put_selected_char
          ld hl,cursor_x
          dec (hl)
          ;
          jp end_of_input_tests
          save_and_switch_to_read_only_mode:
            ;
            call convert_and_put_selected_char_and_update_peak_score_struct
            ;
            ld hl,HISCORE_EXTRAM_ADDRESS
            call load_hiscore_table_from_extram
            ld hl,peak_score
            call rebuild_hiscore_table
            ld hl,HISCORE_EXTRAM_ADDRESS
            call save_hiscore_table_to_extram
            ;
            ld a,FALSE
            ld (new_hiscore),a
            ;
            ld ix,cursor_effect
            ld a,GAME_OBJECT_INACTIVE
            ld (ix+game_object.state),a
            ;
            ld hl,tadaa
            ld c,SFX_CHANNELS2AND3
            call PSGSFXPlay
            call PSGStop
            ld ix,transition_trigger
            ld hl,short_interval_transition
            call initialize_trigger
            call reset_scores
            jp end_of_input_tests
    +:
    ; -----------------------------------------------------------------------
    call is_right_pressed
    ; -----------------------------------------------------------------------
    ; Right means: Select next char.
    jp nc,+
      ld ix,release_keys_trigger
      ld a,ENABLED
      ld (ix+trigger.state),a
      ;
      ld hl,selected_char
      inc (hl)
      ld a,(hl)
      cp LAST_SELECTED_CHAR+1 ; last value in table +1 .
      jp nz,skip_wrap_around
        ld a,FIRST_SELECTED_CHAR
        ld (hl),a
      skip_wrap_around:
      ; Put char under cursor effect.
      call convert_and_put_selected_char
      ld hl,cursor_x
      dec (hl)
      jp end_of_input_tests
    +:
    ; -----------------------------------------------------------------------
    call is_left_pressed
    ; -----------------------------------------------------------------------
    ; Left means: Select previous char.
    jp nc,+
      ld ix,release_keys_trigger
      ld a,ENABLED
      ld (ix+trigger.state),a
      ;
      ld hl,selected_char
      dec (hl)
      ld a,(hl)
      cp FIRST_SELECTED_CHAR-1 ; last value in table +1 .
      jp nz,skip_wrap_around_2
        ld a,LAST_SELECTED_CHAR
        ld (hl),a
      skip_wrap_around_2:
      ; Put char under cursor effect.
      call convert_and_put_selected_char
      ld hl,cursor_x
      dec (hl)
      jp end_of_input_tests
    +:
    ; -----------------------------------------------------------------------
    call is_button_2_pressed
    ; -----------------------------------------------------------------------
    ; Button 2: Backspace/delete currently selected char.
    jp nc,+
      ld a,(active_initial)
      cp 1
      jp z,+
        ; Put a space in.
        ld a,LAST_SELECTED_CHAR
        ld (selected_char),a
        call convert_and_put_selected_char
        ld hl,cursor_x
        dec (hl)
        ;
        ld ix,cursor_effect
        ld a,(ix+game_object.x)
        sub ONE_CHAR_WIDTH
        ld (ix+game_object.x),a
        ;
        ld hl,cursor_x
        dec (hl)
        ;
        ld ix,release_keys_trigger
        ld a,ENABLED
        ld (ix+trigger.state),a
        ; Get ascii from initials string at position. Then convert back to
        ; selected char format.
        ld hl,active_initial
        dec (hl)
        ;
        call get_active_initial_ascii
        ; Convert ascii to selected char
        ld b,(ascii_to_selected_char_table_end-ascii_to_selected_char_table)/2
        ld hl, ascii_to_selected_char_table
        call convert_byte                     ; Now A holds the selected char.
        ld (selected_char),a
        ;
        ; Write the selected char to
        call convert_and_put_selected_char
        ld hl,cursor_x
        dec (hl)
        ;
        ; Put char under cursor effect.
        call convert_and_put_selected_char
        ld hl,cursor_x
        dec (hl)
        ;
        jp end_of_input_tests
    +:
  ; ---------------------------------------------------------------------------
  end_of_input_tests:
  ; The various input processing functions above all end here.
  jp finish_hiscore_loop
  ;
  ;
; *****************************************************************************
  hiscore_read_only: ; (read-only mode).
; *****************************************************************************
    ; No new hiscore.
    ; Display hiscore table until timer depletes, then transition...
    ld ix,transition_trigger
    call process_trigger
    jp nc,+
      ld a,GS_PREPARE_TITLESCREEN
      call transition_to_gamestate
    +:
    call is_button_1_pressed
    jp c,+
    call is_button_2_pressed
    jp nc,++
      +:
      ld a,(transition_trigger.cycle)
      cp HISCORE_CYCLE_VALUE
      jp z,++
        call PSGSFXGetStatus
        cp PSG_PLAYING
        jp z,++
          ld a,GS_PREPARE_TITLESCREEN
          call transition_to_gamestate
    ++:

    ;
; *****************************************************************************
  finish_hiscore_loop:
  ; Both edit and read-only modes end here...
  ;
  ;SELECT_BANK SOUND_BANK
  call PSGSFXFrame
  call PSGFrame
  ;SELECT_BANK HISCORE_BANK
  ;
  ld hl,frame_counter
  inc (hl)
  ;
jp main_loop
  ; ---------------------------------------------------------------------------
  ; Helper functions only for this game state.
  convert_and_put_selected_char_and_update_peak_score_struct:
    ld a,(selected_char)
    ld b,(selected_char_to_ascii_table_end-selected_char_to_ascii_table)/2
    ld hl,selected_char_to_ascii_table
    call convert_byte
    push af
      call put_char
      ; Write it to struct
      ld a,(active_initial)
      ld hl,peak_score.initials
      dec hl
      ld d,0
      ld e,a
      add hl,de
    pop af
    ld (hl),a
  ret
  ;
  convert_and_put_selected_char:
    ld a,(selected_char)
    ld b,(selected_char_to_ascii_table_end-selected_char_to_ascii_table)/2
    ld hl,selected_char_to_ascii_table
    call convert_byte
    call put_char
  ret
  ;
  get_active_initial_ascii:
    ; Return the ascii value for the active initial
    ; Exit: A = ascii value.
    ld a,(active_initial)
    ld hl,peak_score.initials
    dec hl
    ld d,0
    ld e,a
    add hl,de
    ld a,(hl)
  ret
  ;
  ;
  .db "No-Gnah Tnemniatretne"
