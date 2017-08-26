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
  asteroid_init_table:
    .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 16, 16
  asteroid_sprite_table:
    ; Note - only first four items are taken into account.
    .db SPRITE_8, SPRITE_9, SPRITE_10, SPRITE_11
  ;
  shard_init_table:
    .db SPRITE_45, 1, 0, GAME_OBJECT_SHAPE_SMALL
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00010001, 7, 7           ; TODO: Might need adjustment!
  shard_sprite_table:
    .db SPRITE_45
  ;
  spinner_init_table:
    .db SPRITE_12, 1, 0, GAME_OBJECT_SHAPE_TALL
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 16, 24
  spinner_trigger_init_table:
    .db SPINNER_TRIGGER_CHANCE_INIT, SPINNER_TRIGGER_INTERVAL_INIT
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
  danish_init_table:
    .db SPRITE_18, 1, 0, GAME_OBJECT_SHAPE_REGULAR
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 16, 16           ; TODO: Might need adjustment!
  danish_trigger_init_table:
    .db DANISH_TRIGGER_CHANCE_INIT, DANISH_TRIGGER_INTERVAL_INIT
  danish_sprite_table:
    ; Note - only first four items are taken into account.
    .db SPRITE_18, SPRITE_19, SPRITE_20, SPRITE_19
  ;
  missile_init_table:
    .db SPRITE_4, 2, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00010001, 14, 14     ; Might need adjustment!
  missile_trigger_init_table:
    .db MISSILE_TRIGGER_CHANCE_INIT, MISSILE_TRIGGER_INTERVAL_INIT
    .db ENABLED
  ;
  swabby_init_table:
    .db SWABBY_IDLE_SPRITE, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db SWABBY_Y_INIT, SWABBY_X_INIT, GAME_OBJECT_ACTIVE
    .db %00000001, 15, 14, 16
  ;
  bullet_init_table:
    .db SPRITE_44, BULLET_Y_SPEED, BULLET_X_SPEED, GAME_OBJECT_SHAPE_SMALL
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db %00100000, 4, 8
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
  ; ---------------------------------------------------------------------------
  sandbox_bullet_setup_table:
    .db SPRITE_44, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_SMALL
    .db %00100000, 4, 8
  sandbox_asteroid_setup_table:
    .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db %00000000, 16, 16
  ;
  sandbox_asteroid_init_table:
    .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 10, 10, GAME_OBJECT_ACTIVE
    .db %00000000, 16, 16
  sandbox_swabby_init_table:
    .db SWABBY_RIGHT_SPRITE, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 30, 10, GAME_OBJECT_ACTIVE
    .db %00000001, 15, 14, 16
  sandbox_batch_init_table:
    .dw asteroid, sandbox_asteroid_init_table, 3
    .dw swabby, sandbox_swabby_init_table, 1
  sandbox_batch_init_table_end:
  sandbox_batch_draw_table:
    .dw asteroid, 3, swabby, 1
  sandbox_batch_draw_table_end:

  batch_game_object_init_table:
    .dw swabby, swabby_init_table, 1
    .dw spinner, spinner_init_table, 1
    .dw danish, danish_init_table, 1
    .dw missile, missile_init_table, 1
    .dw bullet, bullet_init_table, BULLET_MAX
    .dw asteroid, asteroid_init_table, ASTEROID_MAX
    .dw shard, shard_init_table, SHARD_MAX
  batch_game_object_init_table_end:

  ; ---------------------------------------------------------------------------
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
