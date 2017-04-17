.bank 1 slot 1
;
; -----------------------------------------------------------------------------
.section "Misc data" free
; -----------------------------------------------------------------------------
  pico8_palette_sms:
    .db $00 $10 $12 $18 $06 $15 $3F $3F $13 $0B $0F $0C $38 $26 $27 $2F
  ;
  dummy_text:
    .asc "Score: 00000   Lives: 8#"
  dummy_text2:
    .asc "  Max: 00000    Rank: 0#"
  ;
  ;
  background_table_1:
    .dw NON_ASCII_AREA_START,
    .dw background_1_tiles_end-background_1_tiles,
    .dw background_1_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_1_tilemap

  asteroid_sprite_table:
    ; Note - only first four items are taken into account.
    .db SPRITE_8, SPRITE_9, SPRITE_10, SPRITE_11
  swabby_init_table:
    .db SWABBY_Y_INIT, SWABBY_X_INIT, SPRITE_3
    .db SWABBY_Y_SPEED_INIT, SWABBY_X_SPEED_INIT, PLAYER_OBJECT_ACTIVE
  ;
  ; Menu item strings:
  menu_title:
    .asc "ASTROSWAB! debug menu#"
  item_1:
    .asc "Scene 1#"
  item_2:
    .asc "<unused>#"
  item_3:
    .asc "<unused>#"
  item_4:
    .asc "<unused>#"
  menu_footer:
    .asc "---------------------#"
  menu_state_to_game_state:           ; menu_item(0) == game_state(1), etc.
    .db GS_PREPARE_SCENE_1
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
    .db 18, 5
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
  atascii_tiles:
    .include "bank_2\asciifont_atascii_tiles.inc"
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
