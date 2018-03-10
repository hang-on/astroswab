; gs_warning.inc
; This file includes code to prepare and run a warning screen that displays
; if the user boots Astroswab on a 60 Hz system (NTSC). Astroswab can indeed
; run on a 60 Hz system, but the gameplay and music is coded/made with 50 Hz
; in mind.
; -----------------------------------------------------------------------------
prepare_warning:
; -----------------------------------------------------------------------------
  di
  call disable_display_and_sound
  ;
  ld hl,warning_screen_arrange_vdp_parameters
  call arrange_vdp
  ;
  SELECT_BANK WARNING_BANK
  ld a,(warning_batch_print_table_end-warning_batch_print_table)/SIZE_OF_BATCH_PRINT_ITEM
  ld ix,warning_batch_print_table
  call batch_print
  ;
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,1
  call set_register
  ei
  ;
  ; When all is set, change the game state.
  ld a,GS_RUN_WARNING
  ld (game_state),a
jp main_loop
;
; -----------------------------------------------------------------------------
run_warning:
; -----------------------------------------------------------------------------
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
  call get_random_number
  ;
  ; Press buttons 1 and 2 to proceed to title screen.
  call is_button_1_pressed
  jp nc,+
    call is_button_2_pressed
    jp nc,+
      ld a,GS_PREPARE_TITLESCREEN
      call transition_to_gamestate
  +:
  ;
  call PSGSFXFrame
  call PSGFrame
  ;
  ld hl,frame_counter
  inc (hl)
  ;
jp main_loop
