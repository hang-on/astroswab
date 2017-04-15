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
  .equ NON_ASCII_AREA_START $0c00
  .equ SWABBY_Y_INIT 155
  .equ SWABBY_X_INIT 248/2
  .equ SWABBY_SPEED 5
  ;
  .equ MAX_BULLETS 5
  .equ BULLET_SLEEPING 0
  .equ BULLET_ACTIVE 1
  .equ BULLET_SPEED 5
  .equ BULLET_SPRITE 169
  .equ GUN_DELAY_INIT 12

  .equ GROUND_LEVEL SWABBY_Y_INIT
  .equ ASTEROID_REACTIVATE_VALUE 5
  .equ ASTEROID_MAX 5
  .equ ASTEROID_SPEED_MODIFIER 1
  .equ ASTEROID_SPRITE_MASK %00000011
;
; Sound:
  .equ SOUND_BANK 5
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
  ;
  swabby_y db
  swabby_x db
  swabby_sprite db
  ;
  gun_timer db
  gun_released db
  gun_delay db
  bullet_table dsb 3*MAX_BULLETS   ; 5 bullets, state,y,x,
  ;
  rnd_seed dw

  asteroid instanceof enemy_object ASTEROID_MAX
.ends
