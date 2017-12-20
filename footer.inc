.bank 1 slot 1
;
; -----------------------------------------------------------------------------
.section "Misc data" free
; -----------------------------------------------------------------------------
  pico8_palette_sms:
    ; Make 32 colors for fading.
    .db $00 $10 $12 $18 $06 $15 $2A $3F $13 $0B $0F $0C $38 $26 $27 $2F
    .db $00 $10 $12 $18 $06 $15 $2A $3F $13 $0B $0F $0C $38 $26 $27 $2F
  ;
  dummy_text:
    .asc "  Score:-----*----- POW:#"
  ;
  ; Init and dummy data for testing hiscore table functionality.
  player_score_init:
    .asc "00000"
    .asc "   "
    ;
    ;
  transition_score_init_table:
    .dw difficulty_0_transition_score, difficulty_1_transition_score
    .dw difficulty_2_transition_score
  difficulty_0_transition_score:
    .asc "00500"
  difficulty_1_transition_score:
    .asc "01500"
  difficulty_2_transition_score:
    .asc "03000"
    ;
  gun_level_char_data_init:
    .dw GUN_LEVEL_NAMETABLE_ADDRESS
    .db ASCII_ZERO, GUN_LEVEL_NAMETABLE_FLAGS
  gun_level_string:
    .dw GUN_LEVEL_NAMETABLE_ADDRESS-4
    .db BULLET_CHAR, CHAR_FROM_SPRITE_BANK
    .db ASCII_LOWERCASE_X, CHAR_FROM_BACKGROUND_BANK
    .db ASCII_ZERO, CHAR_FROM_BACKGROUND_BANK
  gun_level_string_end:
  lives_string:
    .dw GUN_LEVEL_NAMETABLE_ADDRESS+4
    .db SWABBY_LIFE_CHAR, CHAR_FROM_SPRITE_BANK
    .db ASCII_LOWERCASE_X, CHAR_FROM_BACKGROUND_BANK
    .db ASCII_ZERO+3, CHAR_FROM_BACKGROUND_BANK
  lives_string_end:

  ;
  game_state_jump_table:
    ; Check the game state constants in header.inc!
    .dw prepare_devmenu, run_devmenu, prepare_level, run_level
    .dw prepare_sandbox, run_sandbox,
    .dw prepare_titlescreen, run_titlescreen, setup_new_game
    .dw prepare_hiscore, run_hiscore
    .dw prepare_warning, run_warning
  ;
  meta_background_table:
    ; Used when preparing a level.
    .dw difficulty_0_background_table, difficulty_1_background_table
    .dw difficulty_2_background_table, difficulty_3_background_table
  ;
  difficulty_0_background_table:
    ; Note: See BACKGROUND_TABLE_MASK!
    .dw background_0_1, background_0_1
    .dw background_0_2, background_0_2, background_0_2
    .dw background_0_3, background_0_3, background_0_3
  difficulty_1_background_table:
    .dw background_1_1, background_1_1
    .dw background_1_2, background_1_2, background_1_2
    .dw background_1_3, background_1_3, background_1_3
  difficulty_2_background_table:
    .dw background_2_1, background_2_1
    .dw background_2_2, background_2_2, background_2_2
    .dw background_2_3, background_2_3, background_2_3
  difficulty_3_background_table:
    .dw background_3_1, background_3_1
    .dw background_3_1, background_3_1, background_3_2
    .dw background_3_2, background_3_2, background_3_2
  ;

  background_0_1:
    .dw NON_ASCII_AREA_START,
    .dw background_1_tiles_end-background_1_tiles,
    .dw background_1_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_1_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
  background_0_2:
    .dw NON_ASCII_AREA_START,
    .dw background_2_tiles_end-background_2_tiles,
    .dw background_2_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_2_tilemap
    .dw $0000, $0000
  background_0_3:
    .dw NON_ASCII_AREA_START,
    .dw background_3_tiles_end-background_3_tiles,
    .dw background_3_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_3_tilemap
    .dw $0000, $0000
  ;
  background_1_1:
    .dw NON_ASCII_AREA_START,
    .dw background_4_tiles_end-background_4_tiles,
    .dw background_4_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_4_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
  background_1_2:
    .dw NON_ASCII_AREA_START,
    .dw background_5_tiles_end-background_5_tiles,
    .dw background_5_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_5_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
  background_1_3:
    .dw NON_ASCII_AREA_START,
    .dw background_6_tiles_end-background_6_tiles,
    .dw background_6_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_6_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
  ;
  background_2_1:
    .dw NON_ASCII_AREA_START,
    .dw background_7_tiles_end-background_7_tiles,
    .dw background_7_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_7_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
  background_2_2:
    .dw NON_ASCII_AREA_START,
    .dw background_8_tiles_end-background_8_tiles,
    .dw background_8_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_8_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
  background_2_3:
    .dw NON_ASCII_AREA_START,
    .dw background_9_tiles_end-background_9_tiles,
    .dw background_9_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_9_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
  ;
  background_3_1:
    .dw NON_ASCII_AREA_START,
    .dw background_10_tiles_end-background_10_tiles,
    .dw background_10_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_10_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
  background_3_2:
    .dw NON_ASCII_AREA_START,
    .dw background_11_tiles_end-background_11_tiles,
    .dw background_11_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_11_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
  ;
  difficulty_to_background_bank_table:
    .db 0, BACKGROUND_BANK, 1, BACKGROUND_BANK_2
    .db 2, BACKGROUND_BANK_3, 3, BACKGROUND_BANK_4
  difficulty_to_background_bank_table_end:
  ;
  batch_game_object_init_table:
    .dw swabby, swabby_init_table, 1
    .dw spinner, spinner_init_table, 1
    .dw booster, booster_init_table, 1
    .dw missile, missile_init_table, 1
    .dw bullet, bullet_init_table, BULLET_MAX
    .dw asteroid, asteroid_init_table, ASTEROID_MAX
    .dw shard, shard_init_table, SHARD_MAX
    .dw bee, bee_init_table, 1
    .dw swabby_death, swabby_death_init_table, 1
  batch_game_object_init_table_end:
  ;
  asteroid_init_table:
    .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 0, 0, 16, 16, ASTEROID
  ;asteroid_sprite_table:
    ; Note - only first eight items are taken into account.
  ;  .db YELLOW_ASTEROID_1, YELLOW_ASTEROID_2, BROWN_ASTEROID, GREY_ASTEROID
  ;  .db PURPLE_ASTEROID, BROWN_ASTEROID, YELLOW_ASTEROID_1
  new_asteroid_sprite_table:
    .db 0, BROWN_ASTEROID, 1, GREY_ASTEROID, 2, PURPLE_ASTEROID
    .db 3, YELLOW_ASTEROID_1, 4, YELLOW_ASTEROID_2, 5, BROWN_ASTEROID
    .db 6, GREY_ASTEROID, 7, YELLOW_ASTEROID_1
  new_asteroid_sprite_table_end:
  ;
  shard_init_table:
    .db SPRITE_45, 1, 0, GAME_OBJECT_SHAPE_SMALL
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00010001, 0, 0, 8, 8, SHARD
  ;
  shard_conversion_table:
    .db YELLOW_ASTEROID_1, YELLOW_SHARD
    .db YELLOW_ASTEROID_2, YELLOW_SHARD
    .db BROWN_ASTEROID, BROWN_SHARD
    .db GREY_ASTEROID, GREY_SHARD
    .db PURPLE_ASTEROID, PURPLE_SHARD
  shard_conversion_table_end:
  ;
  ;
  spinner_init_table:
    .db SPRITE_12, 1, 0, GAME_OBJECT_SHAPE_TALL
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 0, 0, 16, 24, SPINNER
  spinner_anim_table:
  .rept 8
    .rept 8
      .db SPRITE_12
    .endr
    .rept 8
      .db SPRITE_14
    .endr
    .rept 8
      .db SPRITE_16
    .endr
    .rept 8
      .db SPRITE_14
    .endr
  .endr
  ;
  booster_init_table:
    .db SPRITE_18, 1, 0, GAME_OBJECT_SHAPE_REGULAR
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 0, 0, 16, 16, BOOSTER           ; TODO: Might need adjustment!
  booster_sprite_table:
    ; Note - only first four items are taken into account.
    .db SPRITE_18, SPRITE_19, SPRITE_20, SPRITE_19
  ;
  missile_init_table:
    .db SPRITE_4, MISSILE_YSPEED, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00010001, 1, 1, 14, 14, MISSILE     ; Might need adjustment!
  ;
  swabby_init_table:
    .db SWABBY_IDLE_SPRITE, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db SWABBY_Y_INIT, SWABBY_X_INIT, GAME_OBJECT_ACTIVE
    .db %00000001, 1, 0, 15, 14, SWABBY
  swabby_collision_table:
    .dw missile, 1
    .dw asteroid, ASTEROID_MAX
    .dw shard, SHARD_MAX
    .dw spinner, 1
    .dw booster, 1
  swabby_collision_table_end:
  ;
  swabby_death_init_table:
    .db SWABBY_DEATH_SPRITE, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db SWABBY_Y_INIT-1, SWABBY_X_INIT+20, GAME_OBJECT_INACTIVE
    .db %00000001, 1, 0, 15, 14, SWABBY_DEATH
  ;
  swabby_death_anim_table:
  .rept 8
    .rept 8
      .db SPRITE_25
    .endr
    .rept 8
      .db SPRITE_26
    .endr
    .rept 8
      .db SPRITE_27
    .endr
    .rept 8
      .db SPRITE_25
    .endr
  .endr
  ;
  swabby_death_trigger_init_table:
    .db 255, SWABBY_DEATH_TRIGGER_INTERVAL
    .db DISABLED, DEFAULT_CYCLE_VALUE
  ;
  bullet_init_table:
    .db SPRITE_44, BULLET_Y_SPEED, BULLET_X_SPEED, GAME_OBJECT_SHAPE_SMALL
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00100000, 2, 0, 4, 8, BULLET
  bullet_collision_table:
    .dw missile, 1
    .dw asteroid, ASTEROID_MAX
    .dw shard, SHARD_MAX
    .dw spinner, 1
    .dw booster, 1
  bullet_collision_table_end:
  ;
  bee_init_table:
    .db SPRITE_65, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_SMALL
    .db 0, 8, GAME_OBJECT_ACTIVE
    .db %00000000, 0, 0, 1, 1, BEE
  ;
  transition_trigger_init_table:
    .db 255, 255
    .db DISABLED, TRANSITION_TRIGGER_CYCLE_INIT
  ;
  difficulty_to_trigger_init_tables:
    .dw difficulty_0_batch_trigger_init_table
    .dw difficulty_1_batch_trigger_init_table
    .dw difficulty_2_batch_trigger_init_table
    .dw difficulty_3_batch_trigger_init_table
  ;
  ; Triggers are batch-disabled to stop enemy spawning.
  batch_wipe_and_disable_all_triggers:
    .dw spinner_trigger, trigger_wipe_table
    .dw booster_trigger, trigger_wipe_table
    .dw missile_trigger, trigger_wipe_table
    .dw shard_trigger, trigger_wipe_table
    .dw asteroid_trigger, trigger_wipe_table
  batch_wipe_and_disable_all_triggers_end:
  trigger_wipe_table:
    .db 0, 0, DISABLED
  ;
  ; Note! First table is used for dimensioning batch inits (lev. startup).
  difficulty_0_batch_trigger_init_table:
    .dw spinner_trigger, spinner_trigger_init_table_0
    .dw booster_trigger, booster_trigger_init_table_0
    .dw missile_trigger, missile_trigger_init_table_0
    .dw shard_trigger, shard_trigger_init_table_0
    .dw asteroid_trigger, asteroid_trigger_init_table_0
  difficulty_0_batch_trigger_init_table_end:
  difficulty_1_batch_trigger_init_table:
    .dw spinner_trigger, spinner_trigger_init_table_1
    .dw booster_trigger, booster_trigger_init_table_1
    .dw missile_trigger, missile_trigger_init_table_1
    .dw shard_trigger, shard_trigger_init_table_1
    .dw asteroid_trigger, asteroid_trigger_init_table_1
  difficulty_2_batch_trigger_init_table:
    .dw spinner_trigger, spinner_trigger_init_table_2
    .dw booster_trigger, booster_trigger_init_table_2
    .dw missile_trigger, missile_trigger_init_table_2
    .dw shard_trigger, shard_trigger_init_table_2
    .dw asteroid_trigger, asteroid_trigger_init_table_2
  difficulty_3_batch_trigger_init_table:
    .dw spinner_trigger, spinner_trigger_init_table_3
    .dw booster_trigger, booster_trigger_init_table_3
    .dw missile_trigger, missile_trigger_init_table_3
    .dw shard_trigger, shard_trigger_init_table_3
    .dw asteroid_trigger, asteroid_trigger_init_table_3
  ;
  spinner_trigger_init_table_0:
    .db SPINNER_TRIGGER_CHANCE_INIT_0, SPINNER_TRIGGER_INTERVAL_INIT_0
    .db ENABLED, DEFAULT_CYCLE_VALUE
  booster_trigger_init_table_0:
    .db BOOSTER_TRIGGER_CHANCE_INIT_0, BOOSTER_TRIGGER_INTERVAL_INIT_0
    .db ENABLED, DEFAULT_CYCLE_VALUE
  missile_trigger_init_table_0:
    .db MISSILE_TRIGGER_CHANCE_INIT_0, MISSILE_TRIGGER_INTERVAL_INIT_0
    .db ENABLED, DEFAULT_CYCLE_VALUE
  shard_trigger_init_table_0:
    .db SHARD_TRIGGER_CHANCE_INIT_0, SHARD_TRIGGER_INTERVAL_INIT_0
    .db ENABLED, DEFAULT_CYCLE_VALUE
  asteroid_trigger_init_table_0:
    .db ASTEROID_TRIGGER_CHANCE_INIT_0, ASTEROID_TRIGGER_INTERVAL_INIT_0
    .db ENABLED, DEFAULT_CYCLE_VALUE
  ;
  spinner_trigger_init_table_1:
    .db SPINNER_TRIGGER_CHANCE_INIT_1, SPINNER_TRIGGER_INTERVAL_INIT_1
    .db ENABLED, DEFAULT_CYCLE_VALUE
  booster_trigger_init_table_1:
    .db BOOSTER_TRIGGER_CHANCE_INIT_1, BOOSTER_TRIGGER_INTERVAL_INIT_1
    .db ENABLED, DEFAULT_CYCLE_VALUE
  missile_trigger_init_table_1:
    .db MISSILE_TRIGGER_CHANCE_INIT_1, MISSILE_TRIGGER_INTERVAL_INIT_1
    .db ENABLED, DEFAULT_CYCLE_VALUE
  shard_trigger_init_table_1:
    .db SHARD_TRIGGER_CHANCE_INIT_1, SHARD_TRIGGER_INTERVAL_INIT_1
    .db ENABLED, DEFAULT_CYCLE_VALUE
  asteroid_trigger_init_table_1:
    .db ASTEROID_TRIGGER_CHANCE_INIT_1, ASTEROID_TRIGGER_INTERVAL_INIT_1
    .db ENABLED, DEFAULT_CYCLE_VALUE
  ;
  spinner_trigger_init_table_2:
    .db SPINNER_TRIGGER_CHANCE_INIT_2, SPINNER_TRIGGER_INTERVAL_INIT_2
    .db ENABLED, DEFAULT_CYCLE_VALUE
  booster_trigger_init_table_2:
    .db BOOSTER_TRIGGER_CHANCE_INIT_2, BOOSTER_TRIGGER_INTERVAL_INIT_2
    .db ENABLED, DEFAULT_CYCLE_VALUE
  missile_trigger_init_table_2:
    .db MISSILE_TRIGGER_CHANCE_INIT_2, MISSILE_TRIGGER_INTERVAL_INIT_2
    .db ENABLED, DEFAULT_CYCLE_VALUE
  shard_trigger_init_table_2:
    .db SHARD_TRIGGER_CHANCE_INIT_2, SHARD_TRIGGER_INTERVAL_INIT_2
    .db ENABLED, DEFAULT_CYCLE_VALUE
  asteroid_trigger_init_table_2:
    .db ASTEROID_TRIGGER_CHANCE_INIT_2, ASTEROID_TRIGGER_INTERVAL_INIT_2
    .db ENABLED, DEFAULT_CYCLE_VALUE
  ;
  spinner_trigger_init_table_3:
    .db SPINNER_TRIGGER_CHANCE_INIT_3, SPINNER_TRIGGER_INTERVAL_INIT_3
    .db ENABLED, DEFAULT_CYCLE_VALUE
  booster_trigger_init_table_3:
    .db BOOSTER_TRIGGER_CHANCE_INIT_3, BOOSTER_TRIGGER_INTERVAL_INIT_3
    .db ENABLED, DEFAULT_CYCLE_VALUE
  missile_trigger_init_table_3:
    .db MISSILE_TRIGGER_CHANCE_INIT_3, MISSILE_TRIGGER_INTERVAL_INIT_3
    .db ENABLED, DEFAULT_CYCLE_VALUE
  shard_trigger_init_table_3:
    .db SHARD_TRIGGER_CHANCE_INIT_3, SHARD_TRIGGER_INTERVAL_INIT_3
    .db ENABLED, DEFAULT_CYCLE_VALUE
  asteroid_trigger_init_table_3:
    .db ASTEROID_TRIGGER_CHANCE_INIT_3, ASTEROID_TRIGGER_INTERVAL_INIT_3
    .db ENABLED, DEFAULT_CYCLE_VALUE
  ;
  ;
  ; ---------------------------------------------------------------------------
  ; Menu item strings:
  menu_title:
    .asc "ASTROSWAB! debug menu#"
  item_1:
    .asc "Start new game#"
  item_2:
    .asc "Hiscore table#"
  item_3:
    .asc "- unused - #"
  item_4:
    .asc "- unused -#"
  menu_footer:
    .asc "---------------------#"
  extram_counter_label:
    .asc "EXTRAM counter:#"
  gfx_label:
    .asc "GFX:#"
  loop_label:
    .asc "Loop: n.a.#"
  ;
  menu_table:
    .db 47, 63, 79, 95                       ; Contains y-pos for menu selector.
  ;
  batch_print_table:
    .dw menu_title
    .db 4, 5
    .dw item_1
    .db 6, 10
    .dw item_2
    .db 8, 10
    .dw item_3
    .db 10, 10
    .dw item_4
    .db 12, 10
    .dw menu_footer
    .db 14, 5
    .dw extram_counter_label
    .db 16, 5
    .dw gfx_label
    .db 17, 5
    .dw loop_label
    .db 17, 15
  batch_print_table_end:
  pal_msg:
    .asc "TV type: PAL#"
  ntsc_msg:
    .asc "TV type: NTSC#"
.ends
;
.bank FONT_BANK slot 2
; -----------------------------------------------------------------------------
.section "Font assets" free
; -----------------------------------------------------------------------------
  ; Put this ascii map in header:
  ;   .asciitable
  ;      map " " to "z" = 0
  ;    .enda
  font_table:
    .dw BACKGROUND_BANK_START
    .dw CHARS_IN_ASCII_SET*BYTES_PER_TILE
    .dw dd_revolution_tiles
  ;
  dd_revolution_tiles: ; N.B.: Both ressources point to alternative tiles!
    .include "bank_2\dd_revolution_tiles.inc"
    ;
  alternative_font_table:
    .dw BACKGROUND_BANK_START
    .dw CHARS_IN_ASCII_SET*BYTES_PER_TILE
    .dw dd_revolution_alternative_tiles
  dd_revolution_alternative_tiles:
    .include "bank_2\dd_revolution_alternative_tiles.inc"
.ends
;
.bank SPRITE_BANK slot 2
; -----------------------------------------------------------------------------
.section "Sprite assets" free
; -----------------------------------------------------------------------------
  sprite_tiles:
    .include "bank_3\spritesheet.png_tiles.inc"
  sprite_tiles_end:
.ends
.bank BACKGROUND_BANK slot 2
; -----------------------------------------------------------------------------
.section "Background assets" free
; -----------------------------------------------------------------------------
  ;
  background_1_tiles:
    .include "bank_4\background_1_tiles.inc"
  background_1_tiles_end:
  background_1_tilemap:
    .include "bank_4\background_1_tilemap.inc"
  ;
  background_2_tiles:
    .include "bank_4\background_2_tiles.inc"
  background_2_tiles_end:
  background_2_tilemap:
    .include "bank_4\background_2_tilemap.inc"
  ;
  background_3_tiles:
    .include "bank_4\background_3_tiles.inc"
  background_3_tiles_end:
  background_3_tilemap:
    .include "bank_4\background_3_tilemap.inc"
.ends
;
.bank BACKGROUND_BANK_2 slot 2
; -----------------------------------------------------------------------------
.section "Background assets #2" free
; -----------------------------------------------------------------------------
  ;
  background_4_tiles:
    .include "bank_6\background_4_tiles.inc"
  background_4_tiles_end:
  background_4_tilemap:
    .include "bank_6\background_4_tilemap.inc"
  ;
  background_5_tiles:
    .include "bank_6\background_5_tiles.inc"
  background_5_tiles_end:
  background_5_tilemap:
    .include "bank_6\background_5_tilemap.inc"
  ;
  background_6_tiles:
    .include "bank_6\background_6_tiles.inc"
  background_6_tiles_end:
  background_6_tilemap:
    .include "bank_6\background_6_tilemap.inc"

.ends
.bank BACKGROUND_BANK_3 slot 2
; -----------------------------------------------------------------------------
.section "Background assets #3" free
; -----------------------------------------------------------------------------
  ;
  background_7_tiles:
    .include "bank_7\background_7_tiles.inc"
  background_7_tiles_end:
  background_7_tilemap:
    .include "bank_7\background_7_tilemap.inc"
    ;
  background_8_tiles:
    .include "bank_7\background_8_tiles.inc"
  background_8_tiles_end:
  background_8_tilemap:
    .include "bank_7\background_8_tilemap.inc"
    ;
  background_9_tiles:
    .include "bank_7\background_9_tiles.inc"
  background_9_tiles_end:
  background_9_tilemap:
    .include "bank_7\background_9_tilemap.inc"
.ends
;
.bank BACKGROUND_BANK_4 slot 2
; -----------------------------------------------------------------------------
.section "Background assets #4" free
; -----------------------------------------------------------------------------
  ;
  background_10_tiles:
    .include "bank_8\background_10_tiles.inc"
  background_10_tiles_end:
  background_10_tilemap:
    .include "bank_8\background_10_tilemap.inc"
    ;
  background_11_tiles:
    .include "bank_8\background_11_tiles.inc"
  background_11_tiles_end:
  background_11_tilemap:
    .include "bank_8\background_11_tilemap.inc"
    ;
.ends
;
.bank SOUND_BANK slot 2
; -----------------------------------------------------------------------------
.section "Sound assets" free
; -----------------------------------------------------------------------------
  shot_1:
    .incbin "bank_5\shot_3.psg"
  spinner_coming_down:
    .incbin "bank_5\spinner_coming_down.psg"
  explosion:
    .incbin "bank_5\explosion.psg"
  got_booster:
    .incbin "bank_5\got_booster.psg"
  shot_booster:
    .incbin "bank_5\shot_booster.psg"
  missile_warning:
    .incbin "bank_5\missile_warning.psg"
.ends
;
.bank HISCORE_BANK slot 2
; -----------------------------------------------------------------------------
.section "Hiscore assets" free
; -----------------------------------------------------------------------------
  ;
hiscore_init:
  .asc "01000"
  .asc "   "
  .asc "00900"
  .asc "   "
  .asc "00800"
  .asc "   "
  .asc "00700"
  .asc "   "
  .asc "00600"
  .asc "   "
  .asc "00500"
  .asc "   "
  .asc "00400"
  .asc "   "
  .asc "00300"
  .asc "   "
  .asc "00200"
  .asc "   "
  .asc "00100"
  .asc "   "
;
hiscore_background_tiles:
  .include "bank_9\hiscore_background_tiles.inc"
hiscore_background_tiles_end:
hiscore_background_tilemap:
  .include "bank_9\hiscore_background_tilemap.inc"
hiscore_background_table:
  .dw NON_ASCII_AREA_START,
  .dw hiscore_background_tiles_end-hiscore_background_tiles,
  .dw hiscore_background_tiles
  .dw NAME_TABLE_START,
  .dw VISIBLE_NAME_TABLE_SIZE,
  .dw hiscore_background_tilemap
  .dw $0000, $0000                                  ; Filler: 2 words.

.ends
;
.bank TITLESCREEN_BANK slot 2
; -----------------------------------------------------------------------------
.section "Titlescreen assets" free
; -----------------------------------------------------------------------------
;
titlescreen_background_tiles:
  .include "bank_10\titlescreen_background_tiles.inc"
titlescreen_background_tiles_end:
titlescreen_background_tilemap:
  .include "bank_10\titlescreen_background_tilemap.inc"
titlescreen_background_table:
  .dw NON_ASCII_AREA_START,
  .dw titlescreen_background_tiles_end-titlescreen_background_tiles,
  .dw titlescreen_background_tiles
  .dw NAME_TABLE_START,
  .dw VISIBLE_NAME_TABLE_SIZE,
  .dw titlescreen_background_tilemap
  .dw $0000, $0000                                  ; Filler: 2 words.

.ends
;
.bank WARNING_BANK slot 2
; -----------------------------------------------------------------------------
.section "Warning assets" free
; -----------------------------------------------------------------------------
;
warning_background_tiles:
  .include "bank_11\warning_background_tiles.inc"
warning_background_tiles_end:
warning_background_tilemap:
  .include "bank_11\warning_background_tilemap.inc"
warning_background_table:
  .dw NON_ASCII_AREA_START,
  .dw warning_background_tiles_end-warning_background_tiles,
  .dw warning_background_tiles
  .dw NAME_TABLE_START,
  .dw VISIBLE_NAME_TABLE_SIZE,
  .dw warning_background_tilemap
  .dw $0000, $0000                                  ; Filler: 2 words.
;
; Menu item strings:
warning_string_1:
  .asc "W A R N I N G#"
warning_string_2:
  .asc "Your setup is NTSC!#"
warning_string_3:
  .asc "Please switch to PAL for#"
warning_string_4:
  .asc "the best experience.#"
warning_string_5:
  .asc "Press both buttons to#"
warning_string_6:
  .asc "continue anyway...#"
;
warning_batch_print_table:
  ;.dw warning_string_1
  ;.db 2, 7
  .dw warning_string_2
  .db 2, 7
  .dw warning_string_3
  .db 3, 7
  .dw warning_string_4
  .db 4, 7
  .dw warning_string_5
  .db 6, 7
  .dw warning_string_6
  .db 7, 7
warning_batch_print_table_end:

.ends
