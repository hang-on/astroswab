; -----------------------------------------------------------------------------
; Enable debugging features and more:
 ;.equ DEVELOPER_MODE ; Comment out for consumer mode.
; -----------------------------------------------------------------------------
; External RAM map
  .equ EXTRAM_COUNTER EXTRAM_START
  .equ FIRST_GAME_BYTE EXTRAM_COUNTER+1
  .equ RESERVED_BYTE_1 FIRST_GAME_BYTE+1
  .equ RESERVED_BYTE_2 RESERVED_BYTE_1+1
  .equ RESERVED_WORD_1 RESERVED_BYTE_2+1
  .equ HISCORE_EXTRAM_ADDRESS RESERVED_WORD_1+2
;
; First game (first time the game is run - ever!):
  .equ FIRST_GAME_ID $aa  ; Check for this value in EXTRAM (FIRST_GAME_BYTE).

; Font:
  .equ FONT_BANK 2
;
; Hiscore:
  .equ HISCORE_BANK 9
  ;
; Title screen:
  .equ TITLESCREEN_BANK 10
  .equ TITLESCREEN_INTERVAL 70
  .equ TITLESCREEN_CYCLE_VALUE 14
; Game over:
  .equ GAME_OVER_BANK 10
  .equ GAME_OVER_TEXT_ROW 10
  .equ GAME_OVER_TEXT_COLUMN 12
  .equ GAME_OVER_INTERVAL 120
; Confirm reset:
  .equ CONFIRM_RESET_TEXT_ROW 5
  .equ CONFIRM_RESET_TEXT_COLUMN 2

; Warning (wrong tv-type)
  .equ WARNING_BANK 3
; Splash screen
  .equ SPLASH_SCREEN_INTERVAL 180
  ;
; Sandbox and console:
  .equ CONSOLE_LOGGER_START_ROW 4
  ;
; Hiscore:
  .equ HISCORE_INTERVAL 140
  .equ HISCORE_CYCLE_VALUE 3
;
; Pico-related definitions:
  .equ PICO8_PALETTE_SIZE 16
  .equ BLACK 0
  .equ DARK_BLUE 1
  .equ DARK_PURPLE 2
  .equ DARK_GREEN 3
  .equ BROWN 4
  .equ DARK_GREY 5
  .equ LIGHT_GREY 6
  .equ WHITE 7
  .equ RED 8
  .equ ORANGE 9
  .equ YELLOW 10
  .equ GREEN 11
  .equ BLUE 12
  .equ INDIGO 13
  .equ PINK 14
  .equ PEACH 15
  ;
; Misc. definitions:
  .equ PROFILER_MAX_LINE FIRST_LINE_OF_VBLANK
  .equ CHAR_FROM_BACKGROUND_BANK %00000000
  .equ CHAR_FROM_SPRITE_BANK %00000001
  .equ ASCII_LOWERCASE_X 88
  .equ ALL_SWITCHES_ON %00001111 ; For arrange_vdp parameter strings.
;
; Game states:
; Note - these must correspond to the addresses in the game state jump table.
  .equ GS_PREPARE_DEVMENU 0
  .equ GS_RUN_DEVMENU 1
  .equ GS_PREPARE_LEVEL 2
  .equ GS_RUN_LEVEL 3
  .equ GS_PREPARE_SANDBOX 4
  .equ GS_RUN_SANDBOX 5
  .equ GS_PREPARE_TITLESCREEN 6
  .equ GS_RUN_TITLESCREEN 7
  .equ GS_PREPARE_HISCORE 8
  .equ GS_RUN_HISCORE 9
  .equ GS_PREPARE_WARNING 10
  .equ GS_RUN_WARNING 11
  .equ GS_PREPARE_CONSOLE 12
  .equ GS_RUN_CONSOLE 13
  .equ GS_PREPARE_GAME_OVER 14
  .equ GS_RUN_GAME_OVER 15
  .equ GS_PREPARE_CONFIRM_RESET_DATA 16
  .equ GS_RUN_CONFIRM_RESET_DATA 17
  .equ GS_PREPARE_SPLASH_SCREEN 18
  .equ GS_RUN_SPLASH_SCREEN 19
  ;
  ; Initial game state: (Note: First-time warning will trump this).
  .ifdef DEVELOPER_MODE
    ;.equ INITIAL_GAME_STATE GS_PREPARE_TITLESCREEN
    .equ INITIAL_GAME_STATE GS_PREPARE_DEVMENU ; Where to go after boot?
    ;.equ INITIAL_GAME_STATE GS_PREPARE_SANDBOX ; Where to go after boot?
    ;.equ INITIAL_GAME_STATE GS_PREPARE_CONSOLE
  .else
    ;.equ INITIAL_GAME_STATE GS_PREPARE_SPLASH_SCREEN
    .equ INITIAL_GAME_STATE GS_PREPARE_TITLESCREEN
  .endif
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
; Game objects:
  .equ SWABBY 1
  .equ BULLET 2
  .equ ASTEROID 3
  .equ SHARD 4
  .equ SPINNER 5
  .equ MISSILE 6
  .equ BOOSTER 7
  .equ BEE 8
  .equ CURSOR_EFFECT 9
  .equ SWABBY_DEATH 10
  .equ UFO 11
  .equ UFO_BOMB 12
  .equ FESTPLOSION 13
  ;
  .equ NOT_MOVING 0

;
  .equ RUMBLE_END_TRIGGER_INTERVAL 45
;
; Background - nametable, tiles etc.:
  .equ BACKGROUND_BANK 4
  .equ BACKGROUND_BANK_2 6
  .equ BACKGROUND_BANK_3 7
  .equ BACKGROUND_BANK_4 8
  .equ NON_ASCII_AREA_START $0c00
  .equ DUMMY_TEXT_ROW 22
  .equ DUMMY_TEXT_COLUMN 1
  .equ BACKGROUND_TABLE_MASK %00000111

  ; -----------------------------------------------------------------------------
  ; Metasprites
  ; -----------------------------------------------------------------------------
  ; Definitions for referencing the sprites on the spritesheet.
  ; The astroswablib meta sprite code builds on the sprite routines in bluelib
  ; and assumes that metasprites are drawn using the specific astroswab approach
  ; with small, regular, tall and wide sizes.
  .equ SPRITE_1 0
  .equ SPRITE_2 2
  .equ SPRITE_3 8
  .equ SPRITE_4 10
  .equ SPRITE_5 16
  .equ SPRITE_6 18
  .equ SPRITE_7 24
  .equ SPRITE_8 26
  .equ SPRITE_9 32
  .equ SPRITE_10 34
  .equ SPRITE_11 40
  .equ SPRITE_12 42
  .equ SPRITE_13 48
  .equ SPRITE_14 50
  .equ SPRITE_15 56
  .equ SPRITE_16 58
  .equ SPRITE_17 64
  .equ SPRITE_18 66
  .equ SPRITE_19 72
  .equ SPRITE_20 74
  .equ SPRITE_21 80
  .equ SPRITE_22 82

  .equ SPRITE_23 88
  .equ SPRITE_24 90
  .equ SPRITE_25 96
  .equ SPRITE_26 98
  .equ SPRITE_27 104
  .equ SPRITE_28 106

  .equ SPRITE_29 112
  .equ SPRITE_30 114
  .equ SPRITE_31 120
  .equ SPRITE_32 122
  .equ SPRITE_33 128
  .equ SPRITE_34 130


  ; . . .
  .equ FIRST_8x8_SPRITE 168
  .equ SPRITE_43 FIRST_8x8_SPRITE
  .equ SPRITE_44 169
  .equ SPRITE_45 170
  .equ SPRITE_46 171
  .equ SPRITE_47 172
  .equ SPRITE_48 173
  .equ SPRITE_49 174

  .equ SPRITE_50 175
  .equ SPRITE_51 176
  .equ SPRITE_52 177
  .equ SPRITE_53 178
  .equ SPRITE_54 179
  .equ SPRITE_55 180
  .equ SPRITE_56 181
  .equ SPRITE_57 182
  .equ SPRITE_58 183
  .equ SPRITE_59 184
  .equ SPRITE_60 185
  .equ SPRITE_61 186
  .equ SPRITE_62 187
  .equ SPRITE_63 188
  .equ SPRITE_64 189
  .equ SPRITE_65 190
  .equ SPRITE_66 191 ; Last 8x8 sprite

;
; Swabby:
  .equ SWABBY_LEFT_BORDER_SPACER 9
  .equ SWABBY_RIGHT_BORDER_SPACER 15
  .equ SWABBY_WIDTH 16
  .equ SWABBY_Y_INIT 155
  .equ SWABBY_X_MAX CRT_RIGHT_BORDER-SWABBY_RIGHT_BORDER_SPACER
  .equ SWABBY_X_MIN CRT_LEFT_BORDER+SWABBY_LEFT_BORDER_SPACER
  .equ SWABBY_X_INIT (248/2)+8
  .equ SWABBY_X_SPEED_MAX 5
  .equ SWABBY_Y_SPEED_INIT 0
  .equ SWABBY_X_SPEED_INIT 0
  .equ SWABBY_RIGHT_SPRITE SPRITE_1
  .equ SWABBY_LEFT_SPRITE SPRITE_2
  .equ SWABBY_IDLE_SPRITE SPRITE_3
  .equ SWABBY_LIFE_CHAR 175
; Swabby death:
  .equ SWABBY_DEATH_SPRITE SPRITE_25
  .equ SWABBY_DEATH_TRIGGER_INTERVAL 23
  .equ SWABBY_RESPAWN_TRIGGER_INTERVAL 225
; Score:
  .equ SCORE_VRAM_ADDRESS $3d92-2
  .equ PEAK_SCORE_VRAM_ADDRESS SCORE_VRAM_ADDRESS+12
; Lives:
  .equ LIVES_INIT 2
  .equ LIVES_NAMETABLE_ADDRESS $3dac
;
; Bullets and gun:
  .equ BULLET_MAX 5
  .equ BULLET_Y_OFFSET 5
  .equ BULLET_X_OFFSET 4
  .equ BULLET_Y_SPEED (-5)
  .equ BULLET_X_SPEED 0
  .equ BULLET_DEACTIVATE_ZONE_START INVISIBLE_AREA_TOP_BORDER
  .equ BULLET_DEACTIVATE_ZONE_END INVISIBLE_AREA_BOTTOM_BORDER
  .equ GUN_DELAY_INIT 12
  .equ GUN_LEVEL_INIT 0
  .equ GUN_LEVEL_MAX 8
  .equ GUN_LEVEL_NAMETABLE_ADDRESS $3db8
  .equ GUN_LEVEL_NAMETABLE_FLAGS %00000000
  .equ BULLET_CHAR 169
;
; Asteroids:
  .equ ASTEROIDS_INTO_SHARDS_CHANCE 70
  .equ ASTEROID_DEACTIVATE_ZONE_START 155
  .equ ASTEROID_DEACTIVATE_ZONE_END ASTEROID_DEACTIVATE_ZONE_START+8 ; Deprecate this..
  .equ ASTEROID_GROUND_ZONE_START 155 ; Deprecate this...
  .equ ASTEROID_GROUND_ZONE_END ASTEROID_GROUND_ZONE_START+8
  .equ ASTEROID_REACTIVATE_VALUE 2 ;10
  .equ ASTEROID_MAX 5
  .equ ASTEROID_SPEED_MODIFIER %00000011
  .equ ASTEROID_SPRITE_MASK %00000111
  ;
  .equ YELLOW_ASTEROID_1 SPRITE_8
  .equ YELLOW_ASTEROID_2 SPRITE_9
  .equ PURPLE_ASTEROID SPRITE_7
  .equ GREY_ASTEROID SPRITE_11
  .equ BROWN_ASTEROID SPRITE_10
;
; Shards:
  .equ SHARD_MAX 2 + (ASTEROID_MAX*2)
  .equ SHARD_SPEED_MODIFIER 1
  .equ SHARD_FREEFALLING_XSPEED 0
  .equ SHARD_YELLOW_SPRITE SPRITE_45
  .equ YELLOW_SHARD_SPRITE SPRITE_45
  .equ BROWN_SHARD_SPRITE SPRITE_46
  .equ GREY_SHARD_SPRITE SPRITE_47
  .equ PURPLE_SHARD_SPRITE SPRITE_48
  .equ SHARD_DEACTIVATE_ZONE_START 163
  .equ SHARD_DEACTIVATE_ZONE_END SHARD_DEACTIVATE_ZONE_START+8
  .equ SHARD_GROUND_ZONE_START 163
  .equ SHARD_GROUND_ZONE_END SHARD_GROUND_ZONE_START+8
  ;
  ;
  .equ YELLOW_SHARD SPRITE_45
  .equ BROWN_SHARD SPRITE_46
  .equ GREY_SHARD SPRITE_47
  .equ PURPLE_SHARD SPRITE_48
;
; Spinner:
  .equ SPINNER_GROUND_ZONE_START ASTEROID_GROUND_ZONE_START-16
  .equ SPINNER_GROUND_ZONE_END ASTEROID_GROUND_ZONE_END-16
  .equ SPINNER_YSPEED_INIT 1
  .equ SPINNER_MAX 1
;
; Booster:
  .equ BOOSTER_SPRITE_MASK %00000011 ; deprec?
  .equ BOOSTER_GROUND_ZONE_START 155
  .equ BOOSTER_GROUND_ZONE_END BOOSTER_GROUND_ZONE_START+8
  .equ BOOSTER_MAX 1
;
; Missile:
  .equ MISSILE_TRACKING_LINE 90
  .equ MISSILE_YSPEED 4
  .equ MISSILE_XSPEED 4
  .equ MISSILE_GROUND_ZONE_START 155 ; Deprecate this...
  .equ MISSILE_GROUND_ZONE_END MISSILE_GROUND_ZONE_START+8
  .equ MISSILE_MAX 1
;
; UFO:
  .equ UFO_YSPEED_INIT 0
  .equ UFO_XSPEED_INIT 2
  .equ UFO_MAX 1
  .equ UFO_BOMB_MAX 5
  .equ UFO_BOMB_Y_OFFSET 14
  .equ UFO_BOMB_X_OFFSET 13
  .equ UFO_BOMB_Y_SPEED 3
  .equ UFO_BOMB_X_SPEED 0
  .equ UFO_BOMB_DEACTIVATE_ZONE_START SWABBY_Y_INIT+8
  .equ UFO_BOMB_DEACTIVATE_ZONE_END UFO_BOMB_DEACTIVATE_ZONE_START+UFO_BOMB_Y_SPEED+1
  .equ UFO_GUN_INTERVAL 15
  .equ UFO_VERTICAL_ZONE_START 240
  .equ UFO_VERTICAL_ZONE_END 255
  .equ UFO_EXPLODE_DURATION 15
;
.equ INITIAL_DIFFICULTY 0
.equ DIFFICULTY_MAX 4
;
.equ TRANSITION_TRIGGER_CYCLE_INIT 1
;
; Cheat mode:
.equ CHEAT_MODE_SHIELD_INIT 255
.equ CHEAT_MODE_SHIELD_MINIMUM 20
.equ CHEAT_MODE_SHIELD_STEP 20
;
; Difficulty 0
.equ SPINNER_TRIGGER_CHANCE_INIT_0 55
.equ SPINNER_TRIGGER_INTERVAL_INIT_0 220
.equ BOOSTER_TRIGGER_CHANCE_INIT_0 90 ; 190  1 to turn off
.equ BOOSTER_TRIGGER_INTERVAL_INIT_0 210
.equ MISSILE_TRIGGER_CHANCE_INIT_0 0 ; 190 - 1 to almost disable
.equ MISSILE_TRIGGER_INTERVAL_INIT_0 255
.equ SHARD_TRIGGER_CHANCE_INIT_0 90 ;50
.equ SHARD_TRIGGER_INTERVAL_INIT_0 120 ;230
.equ ASTEROID_TRIGGER_CHANCE_INIT_0 110 ; was: 140
.equ ASTEROID_TRIGGER_INTERVAL_INIT_0 60 ;10
.equ UFO_TRIGGER_CHANCE_INIT_0 0
.equ UFO_TRIGGER_INTERVAL_INIT_0 255
;
; Difficulty 1
.equ SPINNER_TRIGGER_CHANCE_INIT_1 60 ; 50
.equ SPINNER_TRIGGER_INTERVAL_INIT_1 210 ;230
.equ BOOSTER_TRIGGER_CHANCE_INIT_1 130 ; 190  1 to turn off
.equ BOOSTER_TRIGGER_INTERVAL_INIT_1 200
.equ MISSILE_TRIGGER_CHANCE_INIT_1 20 ; 190 - 1 to almost disable
.equ MISSILE_TRIGGER_INTERVAL_INIT_1 140 ;130
.equ SHARD_TRIGGER_CHANCE_INIT_1 60 ;50
.equ SHARD_TRIGGER_INTERVAL_INIT_1 40 ;230
.equ ASTEROID_TRIGGER_CHANCE_INIT_1 130 ; was: 150
.equ ASTEROID_TRIGGER_INTERVAL_INIT_1 50 ;10
.equ UFO_TRIGGER_CHANCE_INIT_1 6
.equ UFO_TRIGGER_INTERVAL_INIT_1 210
;
; Difficulty 2
.equ SPINNER_TRIGGER_CHANCE_INIT_2 65 ; 50
.equ SPINNER_TRIGGER_INTERVAL_INIT_2 200 ;230
.equ BOOSTER_TRIGGER_CHANCE_INIT_2 130 ; 190  1 to turn off
.equ BOOSTER_TRIGGER_INTERVAL_INIT_2 200
.equ MISSILE_TRIGGER_CHANCE_INIT_2 35 ; 190 - 1 to almost disable
.equ MISSILE_TRIGGER_INTERVAL_INIT_2 100
.equ SHARD_TRIGGER_CHANCE_INIT_2 60 ;50
.equ SHARD_TRIGGER_INTERVAL_INIT_2 50 ;230
.equ ASTEROID_TRIGGER_CHANCE_INIT_2 60 ; 50
.equ ASTEROID_TRIGGER_INTERVAL_INIT_2 10 ;10
.equ UFO_TRIGGER_CHANCE_INIT_2 30
.equ UFO_TRIGGER_INTERVAL_INIT_2 200
;
; Difficulty 3
.equ SPINNER_TRIGGER_CHANCE_INIT_3 70 ; 50
.equ SPINNER_TRIGGER_INTERVAL_INIT_3 200 ;230
.equ BOOSTER_TRIGGER_CHANCE_INIT_3 130 ; 190  1 to turn off
.equ BOOSTER_TRIGGER_INTERVAL_INIT_3 200
.equ MISSILE_TRIGGER_CHANCE_INIT_3 55 ; 190 - 1 to almost disable
.equ MISSILE_TRIGGER_INTERVAL_INIT_3 100
.equ SHARD_TRIGGER_CHANCE_INIT_3 90 ;50
.equ SHARD_TRIGGER_INTERVAL_INIT_3 50 ;230
.equ ASTEROID_TRIGGER_CHANCE_INIT_3 75 ; 50
.equ ASTEROID_TRIGGER_INTERVAL_INIT_3 10 ;10
.equ UFO_TRIGGER_CHANCE_INIT_3 40
.equ UFO_TRIGGER_INTERVAL_INIT_3 200
;
; Difficulty 4
.equ SPINNER_TRIGGER_CHANCE_INIT_4 75 ; 50
.equ SPINNER_TRIGGER_INTERVAL_INIT_4 200 ;230
.equ BOOSTER_TRIGGER_CHANCE_INIT_4 130 ; 190  1 to turn off
.equ BOOSTER_TRIGGER_INTERVAL_INIT_4 200
.equ MISSILE_TRIGGER_CHANCE_INIT_4 60 ; 190 - 1 to almost disable
.equ MISSILE_TRIGGER_INTERVAL_INIT_4 100
.equ SHARD_TRIGGER_CHANCE_INIT_4 100 ;50
.equ SHARD_TRIGGER_INTERVAL_INIT_4 50 ;230
.equ ASTEROID_TRIGGER_CHANCE_INIT_4 75 ; 50
.equ ASTEROID_TRIGGER_INTERVAL_INIT_4 10 ;10
.equ UFO_TRIGGER_CHANCE_INIT_4 50
.equ UFO_TRIGGER_INTERVAL_INIT_4 200
;
.equ VERTICAL_DEACTIVATE_ZONE_START 0
.equ VERTICAL_DEACTIVATE_ZONE_END 7
.equ BLANKED_COLUMN_START 0
.equ BLANKED_COLUMN_END 7
;
; Sound:
  .equ SOUND_BANK 5
;
; Collision:
  .equ COLLISION_TABLE_DIVIDER  (SWABBY_Y_INIT+8)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
.ramsection "Main variables" slot 3
  ;
  game_state db                   ; Contains game state.
  frame_counter db                ; Used in some loops.
  ;
  temp_byte db
  temp_word dw
  temp_buffer dsb 32*2
  ;
  ;
  menu_timer db
  menu_state db
  ;
  vblank_update_finished_line db
  ;
  level db
  difficulty db
  transition_trigger instanceof trigger
  blinking_text_trigger instanceof trigger
  ;
  ;dev_buffer_2 dsb 4
  swabby instanceof game_object
  swabby_responds_to_player_input db          ; TRUE/FALSE
  swabby_can_collide_with_objects db    ; TRUE/FALSE
  swabby_death instanceof game_object
  ;dev_buffer_2 dsb 4
  kill_swabby_flag db
  lives db
  lives_flag db
  ;dev_buffer_1 dsb 4
  swabby_death_trigger instanceof trigger
  swabby_respawn_trigger instanceof trigger
  player_score instanceof score_struct
  peak_score instanceof score_struct
  transition_score instanceof score_struct
  ;
  ;
  ;
  asteroid instanceof game_object ASTEROID_MAX
  asteroid_trigger instanceof trigger
  ;
  shard instanceof game_object SHARD_MAX
  shard_trigger instanceof trigger
  ;
  spinner instanceof game_object
  spinner_trigger instanceof trigger
  ;
  booster instanceof game_object
  booster_trigger instanceof trigger
  ;
  missile instanceof game_object
  missile_trigger instanceof trigger
  ;
  ufo instanceof game_object
  ufo_trigger instanceof trigger
  ufo_bomb instanceof game_object UFO_BOMB_MAX
  ufo_gun_trigger instanceof trigger
  ;
  bullet instanceof game_object BULLET_MAX
  gun_timer db
  gun_released db
  gun_delay db
  gun_level db
  gun_level_flag db
  gun_level_char_data dsb 4
  ;
  collision_table dsb COLLISION_TABLE_MAX_OBJECTS*COLLISION_TABLE_ELEMENT_SIZE
  objects_in_collision_table db
  ;bottom_collision_table dsb COLLISION_TABLE_MAX_OBJECTS*COLLISION_TABLE_ELEMENT_SIZE
  ;objects_in_bottom_collision_table db
  ;
  rumble_flag db
  vscroll_value db
  rumble_end_trigger instanceof trigger
  ;
  bee instanceof game_object
  ;
  sandbox_trigger instanceof trigger
  ;
  lock_input db
  ;
  cheat_mode_enabled db
  cheat_mode_shield db
  ;
.ends
