.include "bluelib.inc"
.include "psglib.inc"
.include "testlib.inc"

.include "astroswablib.inc";
.include "header.inc"
;
.bank 0 slot 0
; -----------------------------------------------------------------------------
.section "main" free
; -----------------------------------------------------------------------------
  init:
    ; Run this function once (on game load).
    ; Load the pico-8 palette to colors 16-31.
    ld a,SPRITE_PALETTE_START
    ld b,PICO8_PALETTE_SIZE
    ld hl,pico8_palette_sms
    call load_cram
    ; Load the pico-8 palette to colors 0-15.
    ld a,BACKGROUND_PALETTE_START
    ld b,PICO8_PALETTE_SIZE
    ld hl,pico8_palette_sms
    call load_cram
    ; Load the font tiles.
    SELECT_BANK FONT_BANK
    ld hl,font_table
    call load_vram_from_table
    ;
    call PSGInit
    ; Go to the initial game state specified in the header.
    ld a,INITIAL_GAME_STATE
    ld (game_state),a
  jp main_loop
  ;
  ; ---------------------------------------------------------------------------
  main_loop:
    ; Note: This loop can begin on any line - wait for vblank in the states!
    ld a,(game_state)   ; Get current game state - it will serve as JT offset.
    add a,a             ; Double it up because jump table is word-sized.
    ld h,0              ; Set up HL as the jump table offset.
    ld l,a
    ld de,game_state_jump_table ; Point to JT base address (see footer.inc).
    add hl,de           ; Apply offset to base address.
    ld a,(hl)           ; Get LSB from table.
    inc hl              ; Increment pointer.
    ld h,(hl)           ; Get MSB from table.
    ld l,a              ; HL now contains the address of the state handler.
    jp (hl)             ; Jump to this handler - note, not call!
    ;
  ; ---------------------------------------------------------------------------
  ; L E V E L                                                        (gameplay)
  ; ---------------------------------------------------------------------------
  ; This section contains code for two different game states that handles
  ; preparing and running a level.
  prepare_level:
    di
    ; Turn off display and frame interrupts.
    ld a,DISPLAY_0_FRAME_0_SIZE_0
    ld b,1
    call set_register
    ;
    SELECT_BANK BACKGROUND_BANK
    ld a,(level)                          ; Multiply level with 16, because
    rla                                   ; the background table elements are
    rla                                   ; 3+3+2 words wide.
    rla
    rla
    ld d,0
    ld e,a
    ld hl,background_table
    add hl,de
    call load_vram_from_table             ; Load the tiles.
    call load_vram_from_table             ; Load the tilemap.
    ; Print the dummy text under the playfield.
    ld b,DUMMY_TEXT_ROW
    ld c,DUMMY_TEXT_COLUMN
    ld hl,dummy_text
    call print
    ;
    SELECT_BANK SPRITE_BANK
    ld bc,sprite_tiles_end-sprite_tiles
    ld de,SPRITE_BANK_START
    ld hl,sprite_tiles
    call load_vram
    ;
    call randomize  ; FIXME! Base on player input (titlescreen).
    ; Initialize Swabby
    ld ix,swabby
    ld hl,swabby_setup_table
    call set_game_object_from_table
    ld a,SWABBY_Y_INIT
    ld b,SWABBY_X_INIT
    call set_game_object_position
    call activate_game_object
    ; Initialize gun
    ld a,GUN_DELAY_INIT
    ld (gun_delay),a
    xor a
    ld (gun_timer),a
    ld a,TRUE
    ld (gun_released),a
    ; Init (deactivate) all bullets:
    ld b,BULLET_MAX
    ld ix,bullet
    ld hl,bullet_setup_table
    ld de,_sizeof_game_object
    -:
      call set_game_object_from_table
      call deactivate_game_object
      add ix,de
    djnz -
    ; Init (deactivate) all asteroids:
    ld b,ASTEROID_MAX
    ld ix,asteroid
    -:
      call deactivate_game_object
      ld de,_sizeof_game_object
      add ix,de
    djnz -
    ; Init (deactivate) all shards:
    ld b,SHARD_MAX
    ld ix,shard
    -:
      call deactivate_game_object
      ld de,_sizeof_game_object
      add ix,de
    djnz -
    ; Init shard generator:
    ld a,SHARD_GENERATOR_CHANCE_INIT
    ld (shard_generator_chance),a
    ;
    ; Init spinner and generator:
    ld ix,spinner
    call deactivate_game_object
    ld ix,spinner_trigger
    ld hl,spinner_trigger_init_table
    call initialize_trigger
    ; Init danish and generator:
    ld ix,danish
    call deactivate_game_object
    ld ix,danish_trigger
    ld hl,danish_trigger_init_table
    call initialize_trigger
    ; Init missile and generator:
    ld ix,missile
    call deactivate_game_object
    ld ix,missile_trigger
    ld hl,missile_trigger_init_table
    call initialize_trigger
    ; Reset debug meters:
    xor a
    ld (vblank_update_finished_line),a
    ld (main_loop_finished_line),a
    ; Wipe sprites.
    call begin_sprites
    call load_sat
    ;
    call PSGSFXStop
    call PSGStop
    ; Turn on screen, frame interrupts and blank left column.
    ld a,DISPLAY_1_FRAME_1_SIZE_0
    ld b,1
    call set_register
    ld a,SCROLL_0__LCB_1_LINE_0_SPRITES_0
    ld b,0
    call set_register
    ei
    ; When all is set, change the game state.
    ld a,GS_RUN_LEVEL
    ld (game_state),a
  jp main_loop
  ; ---------------------------------------------------------------------------
  ; ---------------------------------------------------------------------------
  run_level:
  call await_frame_interrupt
  call load_sat
  ; End of VDP-updating...
  ; Set debug meter for profiling the amount of lines consumed by functions
  ; operating on the graphics and expecting to work with the screen blanked.
  ; Make sure this meter shows a line number within the vblank period!
  in a,(V_COUNTER_PORT)                   ; Get current line number.
  ld b,a                                  ; Store it in B.
  ld a,(vblank_update_finished_line)      ; Get highest line number yet.
  cp b                                    ; Is the current line higher?
  jp nc,+                                 ; No, skip forward.
    ld a,b                                ; Yes, save current line number as
    ld (vblank_update_finished_line),a    ; the new 'high score'.
  +:                                      ;
  ;
  call get_input_ports
  call begin_sprites
  ; ---------------------------------------------------------------------------
  ; Handle Swabby sprite and movement:
  ld ix,swabby
  ld a,SWABBY_IDLE_SPRITE           ; Start by resetting sprite to idle.
  call set_game_object_sprite
  call is_right_pressed             ; Check if player press right.
  ld a,0
  jp nc,+
    ld a,SWABBY_RIGHT_SPRITE        ; Set sprite.
    call set_game_object_sprite
    call get_game_object_xspeed
    cp SWABBY_X_SPEED_MAX           ; Check current speed against max speed.
    jp z,++                         ; If we are already there, skip ahead...
      inc a                         ; If not, then accelerate a bit.
      jp ++                         ; Skip over the dpad-left check below.
  +:
  call is_left_pressed              ; Check if player press left.
  ld a,0
  jp nc,++
    ld a,SWABBY_LEFT_SPRITE
    call set_game_object_sprite
    call get_game_object_xspeed
    cp -(SWABBY_X_SPEED_MAX)
    jp z,++
      dec a
  ++:
  ld b,a
  xor a
  call set_game_object_speed
  call move_game_object
  call draw_game_object
  ; ---------------------------------------------------------------------------
  ; Gun and bullets.
  call is_button_1_pressed        ;
  jp c,+                          ; Is the player pressing the fire button?
    ld a,TRUE                     ; No - then set gun flag (to prevent
    ld (gun_released),a           ; auto fire).
    add a,(hl)
    ld (hl),a
  +:
  ; Process gun timer.
  ld a,(gun_timer)                ; If gun_timer is not already zero then
  or a                            ; decrement it.
  jp z,+                          ;
    dec a                         ;
    ld (gun_timer),a              ;
  +:                              ;
  call is_button_1_pressed        ; Test for fire button press...
  jp nc,activate_bullet_end       ; If the fire button is not pressed, skip...
    call get_random_number        ; Re-seed random number generator!
    ld a,(gun_timer)              ; Check gun timer (delay between shots).
    or a                          ;
    jp nz,activate_bullet_end     ; If timer not set, skip...
      ld a,(gun_released)         ; Is gun released? (no autofire!)
      cp TRUE                     ;
      jp nz,activate_bullet_end   ; If not, skip...
        ; If we get here, it is time to reset and activate a new bullet.
        ld a,(gun_delay)          ; Make gun wait a little (load time)!
        ld (gun_timer),a          ;
        ld a,FALSE                ; Lock gun (released on fire button release).
        ld (gun_released),a       ;
        ld ix,bullet
        ld a,BULLET_MAX
        call get_inactive_game_object ; Let IX point to first inactive bullet.
        jp c,activate_bullet_end      ; Skip on no inactive bullets (!).
          call activate_game_object
          push ix
          ld ix,swabby
          call get_game_object_position
          pop ix
          sub BULLET_Y_OFFSET
          ld c,a
          ld a,b
          add a,BULLET_X_OFFSET
          ld b,a
          ld a,c
          call set_game_object_position
          ;
          SELECT_BANK SOUND_BANK    ; Select the sound assets bank.
          ld c,SFX_CHANNEL2
          ld hl,shot_1
          call PSGSFXPlay           ; Play the swabby shot sound effect.
          ;
  activate_bullet_end:              ; End of bullet activation code.
  ; Process all bullets.
  ld ix,bullet
  ld b,BULLET_MAX
  -:
    push bc
    call move_game_object
    ld a,BULLET_DEACTIVATE_ZONE_START
    ld b,BULLET_DEACTIVATE_ZONE_END
    call horizontal_zone_deactivate_game_object
    ;
    call draw_game_object
    ;
    ld de,_sizeof_game_object
    add ix,de
    pop bc
  djnz -
  ; ---------------------------------------------------------------------------
  ld ix,asteroid
  ld b,ASTEROID_MAX
  process_asteroids:
    push bc
    call get_game_object_state
    cp GAME_OBJECT_INACTIVE
    jp nz,+
      ld hl,frame_counter                   ; Only consider reactivation on
      bit 0,(hl)                            ; even frames.
      jp nz,+
        call get_random_number
        cp ASTEROID_REACTIVATE_VALUE
        jp nc,+
          ; Activate asteroid.
          call spawn_game_object_in_invisible_area
          call get_random_number
          and ASTEROID_SPRITE_MASK
          ld hl,asteroid_sprite_table
          ld d,0
          ld e,a
          add hl,de
          ld a,(hl)
          call set_game_object_sprite
          call get_random_number
          and ASTEROID_SPEED_MODIFIER
          inc a
          ld b,0
          call set_game_object_speed
          call activate_game_object
    +:
    call move_game_object              ; Move asteroid downwards.
    ; Deactivate asteroid if it is within the deactivate zone.
    ld a,ASTEROID_DEACTIVATE_ZONE_START
    ld b,ASTEROID_DEACTIVATE_ZONE_END
    call horizontal_zone_deactivate_game_object
    ;
    call draw_game_object              ; Put it in the SAT.
    ;
    ld de,_sizeof_game_object
    add ix,de
    pop bc
  djnz process_asteroids
  ; ---------------------------------------------------------------------------
  ; Shard generator
  ld a,(shard_generator_timer)
  dec a
  ld (shard_generator_timer),a
  jp nz,+++
    ; If shard_generator_timer is up, do...
    ld a,(shard_generator_chance)
    ld b,a
    call get_random_number
    cp b
    jp nc,+++
      ld b,SHARD_MAX
      ld ix,shard
      -:
        push bc
        call get_game_object_state
        cp GAME_OBJECT_INACTIVE        ; Search for an inactive shard.
        jp nz,+
          call spawn_game_object_in_invisible_area
          ld a,SHARD_YELLOW_SPRITE
          ld b,a
          call get_random_number
          and %00000011
          add a,b
          call set_game_object_sprite
          call get_random_number
          and SHARD_SPEED_MODIFIER
          inc a
          ld b,SHARD_FREEFALLING_XSPEED
          call set_game_object_speed
          call activate_game_object
          call get_random_number        ; Get interval modifier.
          and %00111111                 ; ... and mask it to 0-63.
          ld b,a
          ld a,SHARD_GENERATOR_INTERVAL
          sub b
          ld (shard_generator_timer),a
          jp +++                        ; Jump out of loop.
        +:
        ld de,_sizeof_game_object
        add ix,de
        pop bc
      djnz -
  +++:
  ld ix,shard
  ld b,SHARD_MAX
  process_shards:
    push bc
    call move_game_object              ; Move shard.
    ld a,SHARD_DEACTIVATE_ZONE_START
    ld b,SHARD_DEACTIVATE_ZONE_END
    call horizontal_zone_deactivate_game_object
    ;
    call draw_game_object              ; Put it in the SAT.
    ;
    ld de,_sizeof_game_object
    add ix,de
    pop bc
  djnz process_shards
  ; ---------------------------------------------------------------------------
  ld ix,spinner
  call get_game_object_state           ; If spinner is already out, skip!
  cp GAME_OBJECT_ACTIVE
  jp z,+
    ld ix,spinner_trigger               ;
    call process_trigger
    jp nc,+
      ; If spinner_generator_timer is up, do...
      ; Activate a new spinner.
      ld ix,spinner
      call spawn_game_object_in_invisible_area
      ld hl,spinner_setup_table
      call set_game_object_from_table
      ld hl,spinner_anim_table
      call load_animation_game_object
      call activate_game_object
  +:
  ;
  ld ix,spinner
  call move_game_object              ; Move
  ld a,SPINNER_DEACTIVATE_ZONE_START
  ld b,SPINNER_DEACTIVATE_ZONE_END
  call horizontal_zone_deactivate_game_object
  call animate_game_object
  call draw_game_object              ; Put it in the SAT.
  ; ---------------------------------------------------------------------------
  ; Handle danish and trigger.
  ld ix,danish
  call get_game_object_state           ; If danish is already out, skip!
  cp GAME_OBJECT_ACTIVE
  jp z,+
    ld ix,danish_trigger               ;
    call process_trigger
    jp nc,+
      ; If danish_trigger generates a trigger event - activate a new danish.
      ld ix,danish
      call spawn_game_object_in_invisible_area
      ld hl,danish_setup_table
      call set_game_object_from_table
      call get_random_number
      and DANISH_SPRITE_MASK
      ld hl,danish_sprite_table
      ld d,0
      ld e,a
      add hl,de
      ld a,(hl)
      call set_game_object_sprite
      call activate_game_object
  +:
  ;
  ld ix,danish
  call move_game_object              ; Move
  ld a,ASTEROID_DEACTIVATE_ZONE_START
  ld b,ASTEROID_DEACTIVATE_ZONE_END
  call horizontal_zone_deactivate_game_object
  call draw_game_object              ; Put it in the SAT.
  ; Handle missile and trigger. -----------------------------------------------
  ld ix,missile
  call get_game_object_state           ; If missile is already out, skip!
  cp GAME_OBJECT_ACTIVE
  jp z,+
    ld ix,missile_trigger              ; Only process trigger if it is enabled.
    call get_trigger_state
    cp ENABLED
    jp nz,+
      call process_trigger
      jp nc,+
        ; If missile_generator_timer is up, activate a new missile.
        ld ix,missile
        call spawn_game_object_in_invisible_area
        ld hl,missile_setup_table
        call set_game_object_from_table
        call activate_game_object
  +:
  ;
  ld ix,missile
  ; TODO: Make missile track/follow Swabby.
  call get_game_object_x
  ; get player x, and then compare 1) somewhere above, 2) right 3) left
  call move_game_object              ; Move
  ld a,ASTEROID_DEACTIVATE_ZONE_START
  ld b,ASTEROID_DEACTIVATE_ZONE_END
  call horizontal_zone_deactivate_game_object
  call draw_game_object              ; Put it in the SAT.
  ; ---------------------------------------------------------------------------
  call PSGSFXFrame
  call PSGFrame
  ;
  ld hl,frame_counter
  inc (hl)
  call is_reset_pressed
  jp nc,+
    call PSGSFXStop
    ld a,GS_PREPARE_DEVMENU
    ld (game_state),a
  +:
  ; Set debug meter:
  in a,(V_COUNTER_PORT)
  ld b,a
  ld a,(main_loop_finished_line)
  cp b
  jp nc,+
    ld a,b
    ld (main_loop_finished_line),a
  +:
  jp main_loop
  ; ---------------------------------------------------------------------------
  ; D E V E L O P M E N T  M E N U
  ; ---------------------------------------------------------------------------
  prepare_devmenu:
    di
    ; Turn off display and frame interrupts.
    ld a,DISPLAY_0_FRAME_0_SIZE_0
    ld b,1
    call set_register
    ;
    ld a,ASCII_SPACE
    ld b,TILE_BANK_1
    call reset_name_table
    ld ix,batch_print_table
    ld a,(batch_print_table_end-batch_print_table)/4
    call batch_print
    ;
    SELECT_BANK SPRITE_BANK
    ld bc,sprite_tiles_end-sprite_tiles
    ld de,SPRITE_BANK_START
    ld hl,sprite_tiles
    call load_vram
    ; Set menu state
    xor a
    ld (menu_state),a
    ld (menu_timer),a
    ; Get the TV type (set during boot).
    ld a,(tv_type)
    ld b,20
    ld c,5
    or a
    jp z,+
      ld hl,pal_msg
      call print
      jp ++
    +:
      ld hl,ntsc_msg
      call print
    ++:
    ; Increment and print external ram counter.
    ld a,16
    ld b,21
    call set_cursor
    SELECT_EXTRAM
      ld hl,EXTRAM_START
      ld a,(hl)
      inc (hl)
    SELECT_ROM
    call print_register_a
    ; Print debug meters:
    ld a,17
    ld b,10
    call set_cursor
    ld a,(vblank_update_finished_line)
    call print_register_a
    ld a,17
    ld b,21
    call set_cursor
    ld a,(main_loop_finished_line)
    call print_register_a
    ; Wipe sprites.
    call begin_sprites
    call load_sat
    call PSGSFXStop
    call PSGStop
    ; Turn on screen and frame interrupts.
    ld a,DISPLAY_1_FRAME_1_SIZE_0
    ld b,1
    call set_register
    ei
    ; When all is set, change the game state.
    ld a,GS_RUN_DEVMENU
    ld (game_state),a
  jp main_loop
  ; ---------------------------------------------------------------------------
  ; ---------------------------------------------------------------------------
  run_devmenu:
    call await_frame_interrupt
    call load_sat
    ;
    ; update()
    call get_input_ports
    ;
    ld a,(menu_timer)                 ; If menu timer is up, then go on to
    cp MENU_DELAY                     ; check for keypresses. Otherwise, just
    jp z,+                            ; inc the timer (this timer goes from
      inc a                           ; 0 to MENU_DELAY) and stops there.
      ld (menu_timer),a               ; It is about anti-bouncing!
      jp menu_end
    +:
      call is_down_pressed       ; Move selector downwards if player
      jp nc,switch_menu_down_end      ; presses down. menu_state is the menu
        ld a,(menu_state)             ; item currently 'under' the selector.
        cp MENU_MAX
        jp z,switch_menu_down_end
          inc a
          ld (menu_state),a
          xor a
          ld (menu_timer),a
      switch_menu_down_end:
      call is_up_pressed         ; Move selector up, on dpad=up....
      jp nc,switch_menu_up_end
        ld a,(menu_state)
        cp MENU_MIN
        jp z,switch_menu_up_end
          dec a
          ld (menu_state),a
          xor a
          ld (menu_timer),a
      switch_menu_up_end:
      ; Check button 1 and 2 to see if user clicks menu item.
      call is_button_1_pressed
      jp c,handle_menu_click
      call is_button_2_pressed
      jp c,handle_menu_click
      jp menu_end
      ;
      ; -----------------------------
      handle_menu_click:
        ld a,(menu_state)
        dec a
        jp p,++
          ; Menu item 0:
          ld a,LEVEL_1
          ld (level),a
          jp +++
        ++:
        dec a
        jp p,++
          ; Menu item 1:
          ld a,LEVEL_2
          ld (level),a
          jp +++
        ++:
        dec a
        jp p,++
          ; Menu item 2:
          ld a,LEVEL_3
          ld (level),a
          jp +++
        ++:
        +++:
        ld a,GS_PREPARE_LEVEL
        ld (game_state),a                 ; Load game state for next loop,
      jp main_loop

    menu_end:
    ; Place menu sprite
    call begin_sprites
    ld hl,menu_table
    ld d,0
    ld a,(menu_state)
    ld e,a
    add hl,de
    ld b,(hl)
    ld a,MENU_ARROW
    ld c,70
    call add_sprite
    call PSGSFXFrame
    call PSGFrame
  jp main_loop
  ; ---------------------------------------------------------------------------
  ; S A N D B O X
  ; ---------------------------------------------------------------------------
  prepare_sandbox:
    di
    ; Turn off display and frame interrupts.
    ld a,DISPLAY_0_FRAME_0_SIZE_0
    ld b,1
    call set_register
    ;
    ld a,ASCII_SPACE
    ld b,TILE_BANK_1
    call reset_name_table
    ;ld ix,batch_print_table
    ;ld a,(batch_print_table_end-batch_print_table)/4
    ;call batch_print
    ;
    SELECT_BANK SPRITE_BANK
    ld bc,sprite_tiles_end-sprite_tiles
    ld de,SPRITE_BANK_START
    ld hl,sprite_tiles
    call load_vram
    ; Wipe sprites.
    call begin_sprites
    call load_sat
    ; Stop music and sound effects.
    call PSGSFXStop
    call PSGStop
    ; ----------------------------------------------------------
    ; ----------------------------------------------------------
    ; Turn on screen and frame interrupts.
    ld a,DISPLAY_1_FRAME_1_SIZE_0
    ld b,1
    call set_register
    ei
    ; When all is set, change the game state.
    ld a,GS_RUN_SANDBOX
    ld (game_state),a
  jp main_loop
  ; ---------------------------------------------------------------------------
  ; ---------------------------------------------------------------------------
  run_sandbox:
    ;
    call await_frame_interrupt
    call load_sat
    ; End of VDP-updating...
    call get_input_ports
    call begin_sprites
    ;
    ; Bullet vs. asteroid tests below:
    ld b,SANDBOX_LOGGER_START_ROW
    call reset_logger
    call test_same_coordinates
    call test_different_coordinates
    call test_horizontal_overlap
    call test_no_horizontal_overlap
    ;
  jp main_loop
  ; Tests for the sandbox:
  test_same_coordinates:
    ld a,10
    ld b,10
    call place_bullet
    ld a,10
    ld b,10
    call place_asteroid
    ld ix,bullet
    ld iy,asteroid
    call are_objects_on_the_same_coordinates
    assertCarrySet "Failed: test_same_coordinates#"
  ret
  test_different_coordinates:
    ld a,40
    ld b,40
    call place_bullet
    ;call draw_game_object
    ld a,10
    ld b,10
    call place_asteroid
    ;call draw_game_object
    ld ix,bullet
    ld iy,asteroid
    call are_objects_on_the_same_coordinates
    assertCarryReset "Failed: test_different_coordinates#"
  ret
  test_horizontal_overlap:
    ld a,10
    ld b,10
    call place_bullet
    call draw_game_object
    ld a,10
    ld b,12
    call place_asteroid
    call draw_game_object
    ld ix,bullet
    ld iy,asteroid
    call are_objects_overlapping_horizontally
    assertCarrySet "Failed: test_horizontal_overlap#"
  ret
  test_no_horizontal_overlap:
    ld a,10
    ld b,10
    call place_bullet
    ;call draw_game_object
    ld a,50
    ld b,50
    call place_asteroid
    ;call draw_game_object
    ld ix,bullet
    ld iy,asteroid
    call are_objects_overlapping_horizontally
    assertCarryReset "Fail: test_no_horizontal_overlap#"
  ret
.ends
;
.include "footer.inc"
