; Font:
  .equ FONT_BANK 2
  .equ STRING_TERMINATOR 3        ; Terminate strings with #
  .asciitable
    map " " to "z" = 0
  .enda
;
; Misc. definitions:
  .equ PICO8_PALETTE_SIZE 16
;
; Game states:
  .equ GS_PREPARE_DEVMENU 0
  .equ GS_RUN_DEVMENU 1
  .equ GS_PREPARE_SCENE_1 2
  .equ GS_RUN_SCENE_1 3
  .equ INITIAL_GAME_STATE GS_PREPARE_SCENE_1 ; Where to go after boot?
;
; Sprites:
  .equ SPRITE_BANK 3
;
; Development menu:
  .equ MENU_MIN 0
  .equ MENU_MAX 3
  .equ MENU_DELAY 10
  .equ MENU_ARROW 168
  .equ TILE_BANK_1 1
  .equ TILE_BANK_2 2
;
; Scene 1:
  .equ SCENE_1_BANK 4

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
.ramsection "Main variables" slot 3
  game_state db                   ; Contains game state.
  frame_counter db                ; Used in some loops.
  ;
  temp_byte db
  temp_word dw
  temp_buffer dsb 32*2
  ;
  menu_timer db
  menu_state db
.ends
