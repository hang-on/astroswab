; gs_warning.inc
;
prepare_warning:
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
  SELECT_BANK WARNING_BANK
  ; Background graphics
  ld hl,warning_background_table
  call load_vram_from_table       ; Load the tiles.
  call load_vram_from_table       ; Load the tilemap.
  ;
  ld a,6
  ld ix,warning_batch_print_table
  call batch_print
  ;
  ; Stop music and sound effects.
  call PSGSFXStop
  call PSGStop
  ;
  ; Turn on screen and frame interrupts.
  ld a,DISPLAY_1_FRAME_1_SIZE_0
  ld b,1
  call set_register
  ei
  ;
  ; When all is set, change the game state.
  ;
  ld a,GS_RUN_WARNING
  ld (game_state),a
jp main_loop
;
run_warning:
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
  ;
  call PSGSFXFrame
  call PSGFrame
  ;
  ld hl,frame_counter
  inc (hl)
  ;
jp main_loop