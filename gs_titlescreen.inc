; gs_titlescreen.inc
;
prepare_titlescreen:
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
  ld b,1
  call set_register
  ei
  ;
  ; When all is set, change the game state.
  ;
  ld a,GS_RUN_TITLESCREEN
  ld (game_state),a
jp main_loop
;
run_titlescreen:
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
  ;
  call PSGSFXFrame
  call PSGFrame
  ;
  ld hl,frame_counter
  inc (hl)
  ;
jp main_loop
