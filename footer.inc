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
  asteroid_sprite_table:
    ; Note - only first four items are taken into account.
    .db SPRITE_4, SPRITE_5, SPRITE_6, SPRITE_4
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
  font_tiles:
    .include "bank_2\asciifont_atascii_tiles.inc"
  font_tiles_end:
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
.bank SCENE_1_BANK slot 2
; -----------------------------------------------------------------------------
.section "Scene 1 assets" free
; -----------------------------------------------------------------------------
  scene_1_tiles:
    .include "bank_4\scene_1_tiles.inc"
  scene_1_tiles_end:

  scene_1_tilemap:
    .include "bank_4\scene_1_tilemap.inc"
  scene_1_tilemap_end:
.ends
.bank SOUND_BANK slot 2
; -----------------------------------------------------------------------------
.section "Sound assets" free
; -----------------------------------------------------------------------------
  shot_1:
    .incbin "bank_5\shot_1.psg"
.ends