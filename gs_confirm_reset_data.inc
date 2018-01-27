; gs_confirm_reset_data.inc
; -----------------------------------------------------------------------------
prepare_confirm_reset_data:
; -----------------------------------------------------------------------------
  di
  ; Turn off display and frame interrupts.
  call disable_display_and_sound
  call PSGSilenceChannels
  ;
  ;
  ld a,ASCII_SPACE
  ld b,TILE_BANK_1
  call reset_name_table
  ;
  ; Load the font tiles.
  SELECT_BANK FONT_BANK
  ld hl,font_table
  call load_vram_from_table
  ;
  SELECT_BANK SPRITE_BANK
  ld bc,sprite_tiles_end-sprite_tiles
  ld de,SPRITE_BANK_START
  ld hl,sprite_tiles
  call load_vram
  ; Set border color.
  ld a,DARK_BLUE
  ld b,REGISTER_7
  call set_register
  ; Wipe sprites.
  call begin_sprites
  call load_sat
  SELECT_BANK GAME_OVER_BANK
  ; Background graphics
  ;ld hl,game_over_background_table
  ;call load_vram_from_table       ; Load the tiles.
  ;call load_vram_from_table       ; Load the tilemap.
  ;
  ; Print "GAME OVER"
  ;ld b,CONFIRM_RESET_TEXT_ROW
  ;ld c,CONFIRM_RESET_TEXT_COLUMN
  ;ld hl,confirm_reset_text_1
  ;call print
  ld ix,confirm_reset_batch_print_table
  ld a,(confirm_reset_batch_print_table_end-confirm_reset_batch_print_table)/4
  call batch_print
  ;
  ld a,TRUE
  ld (lock_input),a
  ;
  ;
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,REGISTER_1
  call set_register
  ei
  call FadeInScreen
  ;
  ;
  ; When all is set, change the game state.
  ;
  ld a,GS_RUN_CONFIRM_RESET_DATA
  ld (game_state),a
jp main_loop
;
; -----------------------------------------------------------------------------
run_confirm_reset_data:
; -----------------------------------------------------------------------------
  ; Make sure we start at the first line of vblank.
  -:
    call await_frame_interrupt
    in a,(V_COUNTER_PORT)
    cp FIRST_LINE_OF_VBLANK+1
  jp nz,-
  ;
  call load_sat
  ;
  ; End of (unsafe) VDP-updating...
  ; ---------------------------------------------------------------------------
  call get_input_ports
  call begin_sprites
  ;
  ;
  ld a,(lock_input)
  cp TRUE
  jp z,skip_locked_keys
    ; Wipe extram on down + button 1 + button 2
    ld a,(confirm_full_reset_keys_end-confirm_full_reset_keys)/2
    ld hl,confirm_full_reset_keys
    call test_multiple_keys_AND
    jp nc,+
      xor a
      SELECT_EXTRAM
        ld (FIRST_GAME_BYTE),a
      SELECT_ROM
      call sound_explosion      ; Dirty little hack coming up...
      ld b,100
      -:
        push bc
          halt
          call PSGSFXFrame
        pop bc
      djnz -
      jp init
    +:
    ; Go back to titlescreen on button 1.
    call is_button_1_pressed
    jp nc,+
      ld a,GS_PREPARE_TITLESCREEN
      call transition_to_gamestate
    +:
    ;
  skip_locked_keys:
  call is_button_1_pressed  ; Prevent player holding down key and accidentally
  jp c,+                    ; transition back to title screen.
    ld a,FALSE
    ld (lock_input),a
  +:
  ;
  call PSGFrame
  call PSGSFXFrame
  ;
  ld hl,frame_counter
  inc (hl)
  ;
jp main_loop
; -----------------------------------------------------------------------------
