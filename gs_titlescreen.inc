; gs_titlescreen.inc
; -----------------------------------------------------------------------------
prepare_titlescreen:
; -----------------------------------------------------------------------------
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
  ld a,DARK_PURPLE
  ld b,REGISTER_7
  call set_register
  ; Wipe sprites.
  call begin_sprites
  call load_sat
  SELECT_BANK TITLESCREEN_BANK
  ; Background graphics
  ld hl,titlescreen_background_table
  call load_vram_from_table       ; Load the tiles.
  call load_vram_from_table       ; Load the tilemap.
  ;
  call randomize                  ; Seed the random number generator.
  ;
  ; Stop music and sound effects.
  call PSGSFXStop
  call PSGStop
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
  ld a,GS_RUN_TITLESCREEN
  ld (game_state),a
jp main_loop
;
; -----------------------------------------------------------------------------
run_titlescreen:
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
  call get_random_number        ; Randomness based on user input.
  ;
  call is_button_1_pressed
  jp c,+
  call is_button_2_pressed
  jp nc,++
    +:
    call initialize_variables_once_per_game
    ld a,GS_PREPARE_LEVEL
    call transition_to_gamestate
    ;jp setup_new_game           ; up a new game.
  ++:
  ;
  call PSGSFXFrame
  call PSGFrame
  ;
  ld hl,frame_counter
  inc (hl)
  ;
jp main_loop
; -----------------------------------------------------------------------------
