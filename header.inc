; Font:
  .equ FONT_BANK 2
;
; Misc. definitions:
  .equ PICO8_PALETTE_SIZE 16
;
; Game states:
; Note - these must correspond to the addresses in the game state jump table.
  .equ GS_PREPARE_DEVMENU 0
  .equ GS_RUN_DEVMENU 1
  .equ GS_PREPARE_LEVEL 2
  .equ GS_RUN_LEVEL 3
  .equ GS_PREPARE_SANDBOX 4
  .equ GS_RUN_SANDBOX 5
  ;
  .equ INITIAL_GAME_STATE GS_PREPARE_DEVMENU ; Where to go after boot?
  ;.equ INITIAL_GAME_STATE GS_PREPARE_SANDBOX ; Where to go after boot?
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
; Sandbox:
  .equ SANDBOX_LOGGER_START_ROW 12
  .equ NOT_MOVING 0
  .equ BULLET_HITBOX_WIDTH 8
  .equ ASTEROID_HITBOX_WIDTH 16

;
; Background - nametable, tiles etc.:
  .equ BACKGROUND_BANK 4
  .equ NON_ASCII_AREA_START $0c00
  .equ DUMMY_TEXT_ROW 22
  .equ DUMMY_TEXT_COLUMN 5
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
; Bullets and gun:
  .equ BULLET_MAX 7
  .equ BULLET_Y_OFFSET 1
  .equ BULLET_X_OFFSET 4
  .equ BULLET_Y_SPEED (-5)
  .equ BULLET_X_SPEED 0
  .equ BULLET_DEACTIVATE_ZONE_START INVISIBLE_AREA_TOP_BORDER
  .equ BULLET_DEACTIVATE_ZONE_END INVISIBLE_AREA_BOTTOM_BORDER
  .equ GUN_DELAY_INIT 11
; Asteroids:
  .equ ASTEROID_DEACTIVATE_ZONE_START 155
  .equ ASTEROID_DEACTIVATE_ZONE_END ASTEROID_DEACTIVATE_ZONE_START+8
  .equ ASTEROID_REACTIVATE_VALUE 10
  .equ ASTEROID_MAX 3
  .equ ASTEROID_SPEED_MODIFIER 1
  .equ ASTEROID_SPRITE_MASK %00000011
; Shards:
  .equ SHARD_GENERATOR_CHANCE_INIT 1  ; 130
  .equ SHARD_GENERATOR_INTERVAL 120
  .equ SHARD_MAX 2 + (ASTEROID_MAX*2)
  .equ SHARD_SPEED_MODIFIER 1
  .equ SHARD_FREEFALLING_XSPEED 0
  .equ SHARD_YELLOW_SPRITE SPRITE_45
  .equ SHARD_DEACTIVATE_ZONE_START 163
  .equ SHARD_DEACTIVATE_ZONE_END SHARD_DEACTIVATE_ZONE_START+8
; Spinner:
  .equ SPINNER_TRIGGER_CHANCE_INIT 50 ; 50
  .equ SPINNER_TRIGGER_INTERVAL_INIT 50 ;230
  .equ SPINNER_DEACTIVATE_ZONE_START ASTEROID_DEACTIVATE_ZONE_START-16
  .equ SPINNER_DEACTIVATE_ZONE_END ASTEROID_DEACTIVATE_ZONE_END-16
; Danish:
  .equ DANISH_TRIGGER_CHANCE_INIT 190 ; 190  1 to turn off
  .equ DANISH_TRIGGER_INTERVAL_INIT 230
  .equ DANISH_SPRITE_MASK %00000011
; Missile:
  .equ MISSILE_TRIGGER_CHANCE_INIT 1 ; 190
  .equ MISSILE_TRIGGER_INTERVAL_INIT 130
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
  vblank_update_finished_line db
  main_loop_finished_line db
  ;
  level db
  ;
  swabby instanceof game_object
  ;
  rnd_seed dw
  ;
  asteroid instanceof game_object ASTEROID_MAX
  ;
  shard_generator_timer db
  shard_generator_chance db
  shard instanceof game_object SHARD_MAX
  ;
  spinner instanceof game_object
  spinner_trigger instanceof trigger
  ;
  danish instanceof game_object
  danish_trigger instanceof trigger
  ;
  missile instanceof game_object
  missile_trigger instanceof trigger
  ;
  bullet instanceof game_object BULLET_MAX
  gun_timer db
  gun_released db
  gun_delay db
  ;
.ends
