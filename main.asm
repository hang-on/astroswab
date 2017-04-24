.include "bluelib.inc"
.include "astroswablib.inc"
.include "header.inc"

.include "psglib.inc"
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
    ;
    ld a,INITIAL_GAME_STATE
    ld (game_state),a
  jp main_loop
  ;
  ; ---------------------------------------------------------------------------
  main_loop:
    ; Note: This loop can begin on any line - wait for vblank in the states!
    ld a,(game_state)
    add a,a
    ld h,0
    ld l,a
    ld de,jump_table
    add hl,de
    ld a,(hl)
    inc hl
    ld h,(hl)
    ld l,a
    jp (hl)
    ;
  jump_table:
    ; Check the game state constants.
    .dw prepare_devmenu, run_devmenu, prepare_level, run_level
  ;
  ; ---------------------------------------------------------------------------
  ; L E V E L                                                        (gameplay)
  ; ---------------------------------------------------------------------------
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
    ;
    SELECT_BANK SPRITE_BANK
    ld bc,sprite_tiles_end-sprite_tiles
    ld de,SPRITE_BANK_START
    ld hl,sprite_tiles
    call load_vram
    ;
    call randomize  ; FIXME! Base on player input (titlescreen).
    ;
    ld b,22
    ld c,5
    ld hl,dummy_text
    call print
    ; Initialize variables
    ld ix,swabby
    ld hl,swabby_init_table
    call initialize_player_object
    ;
    ld a,GUN_DELAY_INIT
    ld (gun_delay),a
    xor a
    ld (gun_timer),a
    ld ix,bullet_table
    ld (ix+0),a     ; Sleeping bullets.
    ld (ix+3),a
    ld (ix+6),a
    ld (ix+9),a
    ld (ix+12),a
    ld a,TRUE
    ld (gun_released),a
    ;
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
    ;
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
  ;
  ; update()
  call get_input_ports
  call begin_sprites
  ; ---------------------------------------------------------------------------
  ; Handle Swabby sprite and movement:
  ld ix,swabby
  ld a,SWABBY_IDLE_SPRITE           ; Start by resetting sprite to idle.
  ld (ix+player_object.sprite),a
  ;
  call is_right_pressed             ; Check if player press right.
  ld a,0
  jp nc,+
    ld a,SWABBY_RIGHT_SPRITE        ; Set sprite.
    ld (ix+player_object.sprite),a
    ld a,(ix+player_object.xspeed)
    cp SWABBY_X_SPEED_MAX           ; Check current speed against max speed.
    jp z,++                         ; If we are already there, skip ahead...
      inc a                         ; If not, then accelerate a bit.
      jp ++                         ; Skip over the dpad-left check below.
  +:
  call is_left_pressed              ; Check if player press left.
  ld a,0
  jp nc,++
    ld a,SWABBY_LEFT_SPRITE
    ld (ix+player_object.sprite),a
    ld a,(ix+player_object.xspeed)
    cp -(SWABBY_X_SPEED_MAX)
    jp z,++
      dec a
  ++:
  ld (ix+player_object.xspeed),a    ; Load new value into Swabby's xspeed.
  ;
  call move_player_object
  call draw_player_object
  ;
  ; ---------------------------------------------------------------------------
  ; Handle gun and bullets:
  call is_button_1_pressed        ; AUTO FIRE PREVENTION AND RND_SEED
  jp c,+                          ; Is the player pressing the fire button?
    ld a,TRUE                     ; No - then set gun flag (to prevent
    ld (gun_released),a           ; auto fire).
    add a,(hl)
    ld (hl),a
  +:                              ; PROCESS GUN TIMER.
  ld a,(gun_timer)                ; If gun_timer is not already zero then
  or a                            ; decrement it.
  jp z,+                          ;
    dec a                         ;
    ld (gun_timer),a              ;
  +:                              ; ACTIVATE BULLET.
  call is_button_1_pressed        ; If the fire button is not pressed, skip...
  jp nc,activate_bullet_end       ; Else proceed to check gun timer.
    call randomize                ; Re-seed random number generator!
    ld a,(gun_timer)              ; Check gun timer (delay between shots).
    or a                          ;
    jp nz,activate_bullet_end     ; If timer not set, skip...
      ld a,(gun_released)         ; 3rd test: Is gun released? (no autofire!)
      cp TRUE                     ;
      jp nz,activate_bullet_end   ; If not, skip...
        ld b,MAX_BULLETS          ; OK, if we come here, we are clear to fire
        ld ix,bullet_table        ; a new bullet (if not all MAX_BULLETS are
        -:                        ; already active).
          ld a,(ix+0)             ; Search for sleeping bullet.
          cp BULLET_SLEEPING      ;
          jp nz,+                 ; If no luck, goto end of loop to try next.
            ld a,(swabby.y)       ; Else, activate this bullet!
            dec a                 ; Add a little y,x offset to bullet sprite in
            ld (ix+1),a           ; relation to Swabby's current location.
            ld a,(swabby.x)       ;
            add a,4               ;
            ld (ix+2),a           ;
            ld a,BULLET_ACTIVE    ; Activate this bullet!
            ld (ix+0),a           ;
            ld a,(gun_delay)      ; Make gun wait a little (load time)!
            ld (gun_timer),a      ;
            ld a,FALSE            ; Lock gun (released on fire button release).
            ld (gun_released),a   ;

            SELECT_BANK SOUND_BANK    ; Select the sound assets bank.
            ld c,SFX_CHANNEL2
            ld hl,shot_1
            call PSGSFXPlay           ; Play the swabby shot sound effect.

            jp activate_bullet_end
          +:                      ; Increment bullet table pointer.
          inc ix                  ;
          inc ix                  ;
          inc ix                  ;
        djnz -                    ; Process all bullets (MAX_BULLETS).
  activate_bullet_end:            ; End of bullet activation code.
  ;
  ld d,MAX_BULLETS                ; PROCESS ALL BULLETS IN TABLE.
  ld ix,bullet_table              ; Load loop counter and table pointer.
  -:                              ; For each bullet do...
    ld a,(ix+0)                   ; See if it is active.
    cp BULLET_ACTIVE              ;
    jp nz,+                       ; If not, skip to end of loop.
      ld a,(ix+1)                 ; Else, get this bullet's y-pos.
      sub BULLET_SPEED            ; Subtract bullet speed to move it up.
      ld (ix+1),a                 ;
      ld b,a                      ; Argument: Sprite y goes into B.
      ld a,(ix+2)                 ; Get sprite x-pos.
      ld c,a                      ; Argument: Sprite x goes into C.
      ld a,BULLET_SPRITE          ; Bullet sprite is a constant (argument).
      call add_sprite             ; Add this bullet sprite to SAT buffer.
      ld a,(ix+1)                 ; Get bullet y-pos.
      cp INVISIBLE_AREA_BOTTOM_BORDER-BULLET_SPEED
      jp c,+                      ; If this bullet has left through the roof,
        ld a,BULLET_SLEEPING      ; we can put it to sleep now...
        ld (ix+0),a               ;
    +:                            ; Forward the table pointer to next bullet.
    inc ix                        ;
    inc ix                        ;
    inc ix                        ;
    dec d                         ;
  jp nz,-                         ; Loop back and process next bullet.
  ; ---------------------------------------------------------------------------
  ld ix,asteroid
  ld b,ASTEROID_MAX
  process_asteroids:
    push bc
    call get_game_object_state
    cp ENEMY_OBJECT_INACTIVE
    jp nz,+
      ld hl,frame_counter
      bit 0,(hl)
      jp nz,+
        call get_random_number
        cp ASTEROID_REACTIVATE_VALUE
        jp nc,+
          ; Activate asteroid.
          call reset_game_object_position
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
  ;
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
        cp ENEMY_OBJECT_INACTIVE        ; Search for an inactive shard.
        jp nz,+
          call reset_game_object_position
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
    call get_game_object_state
    call move_game_object              ; Move shard.
    ; Deactivate shard if it is within the deactivate zone.
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
  ;
  ld ix,spinner
  call get_game_object_state           ; If spinner is already out, skip!
  cp ENEMY_OBJECT_ACTIVE
  jp z,+
    ld ix,spinner_trigger               ;
    call process_trigger
    jp nc,+
      ; If spinner_generator_timer is up, do...
      ; Activate a new spinner.
      ld ix,spinner
      call reset_game_object_position
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
  ;
  ; ---------------------------------------------------------------------------
  ld hl,frame_counter
  inc (hl)
  call is_reset_pressed
  jp nc,+
    call PSGSFXStop
    ld a,GS_PREPARE_DEVMENU
    ld (game_state),a
  +:
  call PSGSFXFrame
  call PSGFrame
  ;
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
.ends
;
.include "footer.inc"
