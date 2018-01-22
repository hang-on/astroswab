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
  ;
  ; Stop music and sound effects.
  call PSGSFXStop
  SELECT_EXTRAM
    ld a,(EXTRAM_COUNTER)
  SELECT_ROM
    bit 0,a
    jp z,+
      ld hl,grimer
      jp ++
    +:
      ld hl,title_1
    ++:
  SELECT_BANK SOUND_BANK
  call PSGPlay
  ;
  ld ix,transition_trigger
  ld hl,transition_trigger_titlescreen_init_table
  call initialize_trigger
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

  ld ix,transition_trigger
  call process_trigger
  jp nc,+
    ld a,DISABLED
    ld (transition_trigger.state),a
  +:
  ;
  ld a,(full_reset_keys_end-full_reset_keys)/2
  ld hl,full_reset_keys
  call test_multiple_keys_AND
  jp nc,+
    xor a
    SELECT_EXTRAM
      ld (FIRST_GAME_BYTE),a
    SELECT_ROM
    jp init
  +:
  ;
  ld hl,both_player_1_buttons
  ld a,(both_player_1_buttons_end-both_player_1_buttons)/2
  call test_multiple_keys_OR
  jp nc,+
    ld a,(transition_trigger.state)
    cp DISABLED
    jp nz,+
    call initialize_variables_once_per_game
    ld a,GS_PREPARE_LEVEL
    call transition_to_gamestate
  +:
  ;
  ;
  call PSGSFXFrame
  call PSGFrame
  ;
  ld hl,frame_counter
  inc (hl)
  ; Re-seed the random number generator.
  ld a,(frame_counter)
  ld (rnd_seed+1),a
  ld b,a
  ld a,(rnd_seed)
  add a,b
  ld (rnd_seed),a
  ;
jp main_loop
; -----------------------------------------------------------------------------
