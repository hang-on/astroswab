; gs_confirm_reset_data.inc
; -----------------------------------------------------------------------------
prepare_confirm_reset_data:
; -----------------------------------------------------------------------------
  di
  ; Turn off display and frame interrupts.
  call disable_display_and_sound
  ;
  ;
  ld hl,clean_blue_screen_arrange_vdp_parameters
  call arrange_vdp
  ;
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
  call await_frame_interrupt
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
