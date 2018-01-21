; gs_devmenu.inc
; Code to prepare and run the development menu game state.
;
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
  ;
  ; Load the font tiles.
  SELECT_BANK FONT_BANK
  ld hl,font_table
  call load_vram_from_table
  ;
  ld ix,batch_print_table
  ld a,(batch_print_table_end-batch_print_table)/4
  call batch_print
  ;
  SELECT_BANK SPRITE_BANK
  ld bc,sprite_tiles_end-sprite_tiles
  ld de,SPRITE_BANK_START
  ld hl,sprite_tiles
  call load_vram
  ;
  ; Set border color.
  ld a,DARK_BLUE
  ld b,REGISTER_7
  call set_register
  ; Set menu state
  xor a
  ld (menu_state),a
  ld (menu_timer),a
  ; Get the TV type (set during boot).
  ld a,(tv_type)
  ld b,20
  ld c,5
  or a
  jp z,+            ; NTSC = 0, PAL = 1
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
    ld hl,EXTRAM_COUNTER
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
  ;
  ;call reset_scores                   ; Reset scores when preparing devmenu.
  call randomize
  ;
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
  call await_frame_interrupt
  call FadeInScreen
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
  ;
  /*
  call is_button_1_pressed
  jp nz,+
    call is_button_2_pressed
    jp nz,+
      ; Both buttons are pressed. Reset hiscore table.
      ;call reset_scores
      ;SELECT_BANK HISCORE_BANK
      ;ld hl,hiscore_init        ; Hiscore initialization data.
      ;ld de,hiscore_item.1      ; Start of hiscore table.
      ;call copy_hiscore_table   ; Initialize hiscore table.
      xor a
      SELECT_EXTRAM
        ld (FIRST_GAME_BYTE),a
        ;ld hl,HISCORE_EXTRAM_ADDRESS
        ;call save_hiscore_table_to_extram
      SELECT_ROM
      ;ld a,GS_PREPARE_HISCORE
      ;call transition_to_gamestate
      jp init
      ;jp main_loop
  +:
  */
  ;
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
    handle_menu_click:  ; N.B. We jump here - not call.
      ld a,(menu_state)
      dec a
      jp p,++
        ; Menu item 0:
        ld a,GS_PREPARE_TITLESCREEN
        call transition_to_gamestate
        jp main_loop                      ; OK, because we jumped here.
      ++:
      dec a
      jp p,++
        ; Menu item 1:
        call FadeOutScreen
        ld a,GS_PREPARE_HISCORE
        ld (game_state),a                 ; Load game state for next loop,
        jp main_loop
      ++:
      dec a
      jp p,++
        ; Menu item 2:
        ld a,GS_PREPARE_SANDBOX
        call transition_to_gamestate
        jp main_loop
      ++:
      ; Menu item 3 (fall through):
      ld a,GS_PREPARE_CONSOLE
      call transition_to_gamestate
    jp main_loop

    ;
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
