; -----------------------------------------------------------------------------
; Enable debugging features and more:
  .equ DEVELOPER_MODE ; Comment out for consumer mode.
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
; Warning (wrong tv-type)
  .equ WARNING_BANK 11
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
  .equ FIRST_LINE_OF_VBLANK 192
  .equ PROFILER_MAX_LINE FIRST_LINE_OF_VBLANK
  .equ CHAR_FROM_BACKGROUND_BANK %00000000
  .equ CHAR_FROM_SPRITE_BANK %00000001
  .equ ASCII_LOWERCASE_X 88
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
  .equ GS_SETUP_NEW_GAME 8
  .equ GS_PREPARE_HISCORE 9
  .equ GS_RUN_HISCORE 10
  .equ GS_PREPARE_WARNING 11
  .equ GS_RUN_WARNING 12
  ;
  .ifdef DEVELOPER_MODE
    .equ INITIAL_GAME_STATE GS_PREPARE_TITLESCREEN
    ;.equ INITIAL_GAME_STATE GS_PREPARE_DEVMENU ;GS_SETUP_NEW_GAME ; Where to go after boot?
    ;.equ INITIAL_GAME_STATE GS_PREPARE_DEVMENU ; Where to go after boot?
    ;.equ INITIAL_GAME_STATE GS_PREPARE_SANDBOX ; Where to go after boot?
  .else
    .equ INITIAL_GAME_STATE GS_PREPARE_TITLESCREEN
    ;.equ INITIAL_GAME_STATE GS_PREPARE_TITLE_SCREEN ; Where to go after boot?
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
  .equ NOT_MOVING 0
  .equ SWABBY_DEATH 10
;
; How many different kinds of triggers to initialize on level init?
; shard, missile, booster, spinner = 4
  .equ NUMBER_OF_TRIGGERS 4 ; FIXME? Could be calc. from one of the diffi.tables??
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
; Score:
  .equ SCORE_VRAM_ADDRESS $3d92
  .equ PEAK_SCORE_VRAM_ADDRESS $3d92+12
;
; Bullets and gun:
  .equ BULLET_MAX 5
  .equ BULLET_Y_OFFSET 1
  .equ BULLET_X_OFFSET 4
  .equ BULLET_Y_SPEED (-5)
  .equ BULLET_X_SPEED 0
  .equ BULLET_DEACTIVATE_ZONE_START INVISIBLE_AREA_TOP_BORDER
  .equ BULLET_DEACTIVATE_ZONE_END INVISIBLE_AREA_BOTTOM_BORDER
  .equ GUN_DELAY_INIT 12
  .equ GUN_LEVEL_INIT 0
  .equ GUN_LEVEL_MAX 8
  .equ GUN_LEVEL_NAMETABLE_ADDRESS $3dae+4
  .equ GUN_LEVEL_NAMETABLE_FLAGS %00000000
  .equ BULLET_CHAR 169
;
; Asteroids:
  .equ ASTEROIDS_INTO_SHARDS_CHANCE 70
  .equ ASTEROID_DEACTIVATE_ZONE_START 155
  .equ ASTEROID_DEACTIVATE_ZONE_END ASTEROID_DEACTIVATE_ZONE_START+8
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
  .equ SHARD_DEACTIVATE_ZONE_START 163
  .equ SHARD_DEACTIVATE_ZONE_END SHARD_DEACTIVATE_ZONE_START+8
  ;
  .equ YELLOW_SHARD SPRITE_45
  .equ BROWN_SHARD SPRITE_46
  .equ GREY_SHARD SPRITE_47
  .equ PURPLE_SHARD SPRITE_48
;
; Spinner:
  .equ SPINNER_DEACTIVATE_ZONE_START ASTEROID_DEACTIVATE_ZONE_START-16
  .equ SPINNER_DEACTIVATE_ZONE_END ASTEROID_DEACTIVATE_ZONE_END-16
;
; Booster:
  .equ BOOSTER_SPRITE_MASK %00000011
;
; Missile:
  .equ MISSILE_TRACKING_LINE 125
  .equ MISSILE_YSPEED 4
  .equ MISSILE_XSPEED 1
;
.equ INITIAL_DIFFICULTY 0
.equ DIFFICULTY_MAX 3
;
.equ TRANSITION_TRIGGER_CYCLE_INIT 1
;
; Difficulty 0
.equ SPINNER_TRIGGER_CHANCE_INIT_0 110 ; 50
.equ SPINNER_TRIGGER_INTERVAL_INIT_0 180 ;230
.equ BOOSTER_TRIGGER_CHANCE_INIT_0 90 ; 190  1 to turn off
.equ BOOSTER_TRIGGER_INTERVAL_INIT_0 210
.equ MISSILE_TRIGGER_CHANCE_INIT_0 1 ; 190 - 1 to almost disable
.equ MISSILE_TRIGGER_INTERVAL_INIT_0 255
.equ SHARD_TRIGGER_CHANCE_INIT_0 90 ;50
.equ SHARD_TRIGGER_INTERVAL_INIT_0 120 ;230
.equ ASTEROID_TRIGGER_CHANCE_INIT_0 145 ; 50
.equ ASTEROID_TRIGGER_INTERVAL_INIT_0 40 ;10
;
; Difficulty 1
.equ SPINNER_TRIGGER_CHANCE_INIT_1 150 ; 50
.equ SPINNER_TRIGGER_INTERVAL_INIT_1 170 ;230
.equ BOOSTER_TRIGGER_CHANCE_INIT_1 130 ; 190  1 to turn off
.equ BOOSTER_TRIGGER_INTERVAL_INIT_1 200
.equ MISSILE_TRIGGER_CHANCE_INIT_1 1 ; 190 - 1 to almost disable
.equ MISSILE_TRIGGER_INTERVAL_INIT_1 255
.equ SHARD_TRIGGER_CHANCE_INIT_1 60 ;50
.equ SHARD_TRIGGER_INTERVAL_INIT_1 40 ;230
.equ ASTEROID_TRIGGER_CHANCE_INIT_1 55 ; 50
.equ ASTEROID_TRIGGER_INTERVAL_INIT_1 10 ;10
;
; Difficulty 2
.equ SPINNER_TRIGGER_CHANCE_INIT_2 150 ; 50
.equ SPINNER_TRIGGER_INTERVAL_INIT_2 170 ;230
.equ BOOSTER_TRIGGER_CHANCE_INIT_2 130 ; 190  1 to turn off
.equ BOOSTER_TRIGGER_INTERVAL_INIT_2 200
.equ MISSILE_TRIGGER_CHANCE_INIT_2 1 ; 190 - 1 to almost disable
.equ MISSILE_TRIGGER_INTERVAL_INIT_2 255
.equ SHARD_TRIGGER_CHANCE_INIT_2 60 ;50
.equ SHARD_TRIGGER_INTERVAL_INIT_2 50 ;230
.equ ASTEROID_TRIGGER_CHANCE_INIT_2 65 ; 50
.equ ASTEROID_TRIGGER_INTERVAL_INIT_2 10 ;10
;
; Difficulty 3
.equ SPINNER_TRIGGER_CHANCE_INIT_3 150 ; 50
.equ SPINNER_TRIGGER_INTERVAL_INIT_3 140 ;230
.equ BOOSTER_TRIGGER_CHANCE_INIT_3 130 ; 190  1 to turn off
.equ BOOSTER_TRIGGER_INTERVAL_INIT_3 200
.equ MISSILE_TRIGGER_CHANCE_INIT_3 200 ; 190 - 1 to almost disable
.equ MISSILE_TRIGGER_INTERVAL_INIT_3 150
.equ SHARD_TRIGGER_CHANCE_INIT_3 90 ;50
.equ SHARD_TRIGGER_INTERVAL_INIT_3 50 ;230
.equ ASTEROID_TRIGGER_CHANCE_INIT_3 75 ; 50
.equ ASTEROID_TRIGGER_INTERVAL_INIT_3 10 ;10

;
.equ VERTICAL_DEACTIVATE_ZONE_START 0
.equ VERTICAL_DEACTIVATE_ZONE_END 7
;
; Sound:
  .equ SOUND_BANK 5
;
; Collision:
  .equ COLLISION_TABLE_DIVIDER  (SWABBY_Y_INIT/2)
; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
.ramsection "Main variables" slot 3
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
  ;
  swabby instanceof game_object
  swabby_death instanceof game_object
  swabby_death_trigger instanceof trigger
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
  bullet instanceof game_object BULLET_MAX
  gun_timer db
  gun_released db
  gun_delay db
  gun_level db
  gun_level_flag db
  gun_level_char_data dsb 4
  ;
  top_collision_table dsb COLLISION_TABLE_MAX_OBJECTS*COLLISION_TABLE_ELEMENT_SIZE
  objects_in_top_collision_table db
  bottom_collision_table dsb COLLISION_TABLE_MAX_OBJECTS*COLLISION_TABLE_ELEMENT_SIZE
  objects_in_bottom_collision_table db
  ;
  bee instanceof game_object
  ;
  sandbox_trigger instanceof trigger
  ;
.ends
