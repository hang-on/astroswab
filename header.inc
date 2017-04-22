; Font:
  .equ FONT_BANK 2
;
; Misc. definitions:
  .equ PICO8_PALETTE_SIZE 16
;
;
; Game states:
  .equ GS_PREPARE_DEVMENU 0
  .equ GS_RUN_DEVMENU 1
  .equ GS_PREPARE_LEVEL 2
  .equ GS_RUN_LEVEL 3

  .equ INITIAL_GAME_STATE GS_PREPARE_DEVMENU ; Where to go after boot?
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
; Background - nametable, tiles etc.:
  .equ BACKGROUND_BANK 4
  .equ NON_ASCII_AREA_START $0c00
;
; Levels:
  .equ LEVEL_1 0
  .equ LEVEL_2 1
  .equ LEVEL_3 2
  .equ LEVEL_4 3
  .equ LEVEL_5 4
; Swabby:
  .equ SWABBY_Y_INIT 155
  .equ SWABBY_X_INIT (248/2)+8
  .equ SWABBY_X_SPEED_MAX 5
  .equ SWABBY_Y_SPEED_INIT 0
  .equ SWABBY_X_SPEED_INIT 0
  .equ SWABBY_RIGHT_SPRITE SPRITE_1
  .equ SWABBY_LEFT_SPRITE SPRITE_2
  .equ SWABBY_IDLE_SPRITE SPRITE_3
  ;
  .equ MAX_BULLETS 5
  .equ BULLET_SLEEPING 0
  .equ BULLET_ACTIVE 1
  .equ BULLET_SPEED 5
  .equ BULLET_SPRITE 169
  .equ GUN_DELAY_INIT 12

  .equ ASTEROID_DEACTIVATE_ZONE_START 155
  .equ ASTEROID_DEACTIVATE_ZONE_END ASTEROID_DEACTIVATE_ZONE_START+8
  .equ ASTEROID_REACTIVATE_VALUE 1
  .equ ASTEROID_MAX 3
  .equ ASTEROID_SPEED_MODIFIER 1
  .equ ASTEROID_SPRITE_MASK %00000011

  .equ SHARD_MAX 2 + (ASTEROID_MAX*2)
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
  level db
  ;
  swabby instanceof player_object
  ;
  ;
  gun_timer db
  gun_released db
  gun_delay db
  bullet_table dsb 3*MAX_BULLETS   ; 5 bullets, state,y,x,
  ;
  rnd_seed dw
  ;
  asteroid instanceof enemy_object ASTEROID_MAX
  shard instanceof enemy_object SHARD_MAX
.ends
