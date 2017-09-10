.bank 1 slot 1
;
; -----------------------------------------------------------------------------
.section "Misc data" free
; -----------------------------------------------------------------------------
  pico8_palette_sms:
    .db $00 $10 $12 $18 $06 $15 $3F $3F $13 $0B $0F $0C $38 $26 $27 $2F
  ;
  dummy_text:
    .asc "Score: 00000*00000#"
  ;
  game_state_jump_table:
    ; Check the game state constants in header.inc!
    .dw prepare_devmenu, run_devmenu, prepare_level, run_level
    .dw prepare_sandbox, run_sandbox
  ;
  background_table:
    ; level 1:
    .dw NON_ASCII_AREA_START,
    .dw background_1_tiles_end-background_1_tiles,
    .dw background_1_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_1_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
    ; level 2:
    .dw NON_ASCII_AREA_START,
    .dw background_2_tiles_end-background_2_tiles,
    .dw background_2_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_2_tilemap
    .dw $0000, $0000
    ; level 3:
    .dw NON_ASCII_AREA_START,
    .dw background_3_tiles_end-background_3_tiles,
    .dw background_3_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_3_tilemap
    .dw $0000, $0000
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
  batch_game_object_init_table_end:
  ;
  asteroid_init_table:
    .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 0, 0, 16, 16, ASTEROID
  asteroid_sprite_table:
    ; Note - only first eight items are taken into account.
    .db YELLOW_ASTEROID_1, YELLOW_ASTEROID_2, BROWN_ASTEROID, GREY_ASTEROID
    .db PURPLE_ASTEROID, BROWN_ASTEROID, GREY_ASTEROID, PURPLE_ASTEROID
  ;
  shard_init_table:
    .db SPRITE_45, 1, 0, GAME_OBJECT_SHAPE_SMALL
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00010001, 1, 1, 7, 7, SHARD           ; TODO: Might need adjustment!
  shard_trigger_init_table:
    .db SHARD_TRIGGER_CHANCE_INIT, SHARD_TRIGGER_INTERVAL_INIT
  ;
  trigger_batch_init_table:
    .dw spinner_trigger, spinner_trigger_init_table
    .dw booster_trigger, booster_trigger_init_table
    .dw missile_trigger, missile_trigger_init_table
    .dw shard_trigger, shard_trigger_init_table
  trigger_batch_init_table_end:
  ;
  spinner_init_table:
    .db SPRITE_12, 1, 0, GAME_OBJECT_SHAPE_TALL
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 0, 0, 16, 24, SPINNER
  spinner_trigger_init_table:
    .db SPINNER_TRIGGER_CHANCE_INIT, SPINNER_TRIGGER_INTERVAL_INIT
    .db ENABLED
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
  booster_trigger_init_table:
    .db BOOSTER_TRIGGER_CHANCE_INIT, BOOSTER_TRIGGER_INTERVAL_INIT
    .db ENABLED
  booster_sprite_table:
    ; Note - only first four items are taken into account.
    .db SPRITE_18, SPRITE_19, SPRITE_20, SPRITE_19
  ;
  missile_init_table:
    .db SPRITE_4, 2, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00010001, 1, 1, 14, 14, MISSILE     ; Might need adjustment!
  missile_trigger_init_table:
    .db MISSILE_TRIGGER_CHANCE_INIT, MISSILE_TRIGGER_INTERVAL_INIT
    .db ENABLED
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
  ; ---------------------------------------------------------------------------
  ; Menu item strings:
  menu_title:
    .asc "ASTROSWAB! debug menu#"
  item_1:
    .asc "Countryside#"
  item_2:
    .asc "Beach#"
  item_3:
    .asc "Shroomopia#"
  item_4:
    .asc "<unused>#"
  menu_footer:
    .asc "---------------------#"
  extram_counter_label:
    .asc "EXTRAM counter:#"
  gfx_label:
    .asc "GFX:#"
  loop_label:
    .asc "Loop:#"
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
  dd_revolution_tiles:
    .include "bank_2\dd_revolution_tiles.inc"
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
.bank SOUND_BANK slot 2
; -----------------------------------------------------------------------------
.section "Sound assets" free
; -----------------------------------------------------------------------------
  shot_1:
    .incbin "bank_5\shot_1.psg"
.ends
