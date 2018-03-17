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
    .asc " Score:-----*-----     POW:#"
  ;
  ; Init and dummy data for testing hiscore table functionality.
  player_score_init:
    .asc "00000"
    .asc "   "
    ;
    ;
  transition_score_init_table:
    .dw difficulty_0_transition_score, difficulty_1_transition_score
    .dw difficulty_2_transition_score, difficulty_3_transition_score
  difficulty_0_transition_score:
    .asc "05000"
  difficulty_1_transition_score:
    .asc "15000"
  difficulty_2_transition_score:
    .asc "35000"
  difficulty_3_transition_score:
    .asc "60000"
  max_score:
    .asc "90000" ;99000
  ;test_start_score:
  ;  .asc "98700"
  ;  .asc "   "
  ;
  gun_level_char_data_init:
    .dw GUN_LEVEL_NAMETABLE_ADDRESS
    .db ASCII_ZERO+GUN_LEVEL_INIT, GUN_LEVEL_NAMETABLE_FLAGS
  ;
  lives_string:
    .dw GUN_LEVEL_NAMETABLE_ADDRESS-16
    .db SWABBY_LIFE_CHAR, CHAR_FROM_SPRITE_BANK
    .db ASCII_LOWERCASE_X, CHAR_FROM_BACKGROUND_BANK
    .db ASCII_ZERO+3, CHAR_FROM_BACKGROUND_BANK
  lives_string_end:
  ;
  game_over_text:
    .asc "GAME OVER#"
  ;
  confirm_reset_text_0:
    .asc "      RESET GAME DATA ?#"
  confirm_reset_text_1:
    .asc "Press [down] AND both buttons#"
  confirm_reset_text_2:
    .asc "to reset all game data!#"
  confirm_reset_text_3:
    .asc "Press button 1 to go safely#"
  confirm_reset_text_4:
    .asc "back to title screen...#"

  confirm_reset_batch_print_table:
    .dw confirm_reset_text_0
    .db 2, 2
    .dw confirm_reset_text_1
    .db 6, 2
    .dw confirm_reset_text_2
    .db 7, 2
    .dw confirm_reset_text_3
    .db 10, 2
    .dw confirm_reset_text_4
    .db 11, 2
  confirm_reset_batch_print_table_end:
  ;
  confirm_full_reset_keys:
    .dw is_down_pressed, is_button_1_pressed, is_button_2_pressed
  confirm_full_reset_keys_end:
  ;
  clean_blue_screen_arrange_vdp_parameters:
    .db ALL_SWITCHES_ON               ; Switches.
    .db ASCII_SPACE, TILE_BANK_1      ; Wipe tilebank with character.
    .db DARK_BLUE                     ; Border color
    ;
    .db 1                             ; Number of block to load into vram
    .db FONT_BANK                     ; Bank
    .dw font_table                    ; Table in the format used by load_vram_from_table.
  ;
  ; Warning screen setup:
  warning_screen_arrange_vdp_parameters:
    .db ALL_SWITCHES_ON               ; Switches.
    .db ASCII_SPACE, TILE_BANK_1      ; Wipe tilebank with character.
    .db DARK_PURPLE                   ; Border color
    ;
    .db 3                             ; Number of block to load into vram
    .db FONT_BANK                     ; Bank
    .dw alternative_font_table        ; Table in the format used by load_vram_from_table.
    .db WARNING_BANK
    .dw warning_tiles_table
    .db WARNING_BANK
    .dw warning_tilemap_table
  ;
  rumble_end_trigger_init_table:
    .db 255, RUMBLE_END_TRIGGER_INTERVAL
    .db DISABLED, DEFAULT_CYCLE_VALUE
  ;
  game_state_jump_table:
    ; Check the game state constants in header.inc!
    .dw prepare_devmenu, run_devmenu, prepare_level, run_level
    .dw prepare_sandbox, run_sandbox,
    .dw prepare_titlescreen, run_titlescreen
    .dw prepare_hiscore, run_hiscore
    .dw prepare_warning, run_warning
    .dw prepare_console, run_console
    .dw prepare_game_over, run_game_over
    .dw prepare_confirm_reset_data, run_confirm_reset_data
    .dw prepare_splash_screen, run_splash_screen
  ;
  meta_background_table:
    ; Used when preparing a level.
    .dw difficulty_0_background_table, difficulty_1_background_table
    .dw difficulty_2_background_table, difficulty_3_background_table
    .dw difficulty_4_background_table
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
  difficulty_4_background_table:
    .dw background_4, background_4
    .dw background_4, background_4, background_4
    .dw background_4, background_4, background_4
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
  background_4:
    .dw NON_ASCII_AREA_START,
    .dw background_12_tiles_end-background_12_tiles,
    .dw background_12_tiles
    .dw NAME_TABLE_START,
    .dw VISIBLE_NAME_TABLE_SIZE,
    .dw background_12_tilemap
    .dw $0000, $0000                                  ; Filler: 2 words.
  ;
  difficulty_to_background_bank_table:
    .db 0, BACKGROUND_BANK, 1, BACKGROUND_BANK_2
    .db 2, BACKGROUND_BANK_3, 3, BACKGROUND_BANK_4
    .db 4, BACKGROUND_BANK_4
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
    .dw ufo, ufo_init_table, 1
    .dw ufo_bomb, ufo_bomb_init_table, UFO_BOMB_MAX
  batch_game_object_init_table_end:
  ;
  game_object_respawn_horizontal_position_table:
    .rept 4
      .db 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72 ;17
      .db 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136 ;16
      .db 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200 ;16
      .db 204, 208, 212, 216, 220, 224, 228, 232, 236, 225, 72, 136, 196, 238, 238 ;15
    .endr
    ;
  .equ ASTEROID_Y_SPEED_INIT 1
  asteroid_init_table:
    .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db INVISIBLE_AREA_BOTTOM_BORDER-16, 0, GAME_OBJECT_INACTIVE
    .db UNUSED_BYTE, -(4), 0, 16, 16, ASTEROID
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UNUSED_BYTE ; Counter.
    .dw UNUSED_WORD ; Counter handler.
    .db UNUSED_BYTE ; Counter state.
    ;
  update_asteroid_parameters:
    ; Special handler for when the game object is in a specified horizontal zone?
    .db TRUE
    .dw asteroid_hits_ground
    .db ASTEROID_GROUND_ZONE_START, ASTEROID_GROUND_ZONE_END
    ; Special handler for when the game object is in a specified vertical zone?
    .db FALSE
    .dw $0000
    .db 0, 0
    ; Animate object
    .db FALSE
    ; Put in collision table
    .db TRUE
    ;
  asteroid_and_shard_respawn_yspeed_table: ; could vary with difficulty!?
    .rept 120
      .db 1
    .endr
    .rept 80
      .db 2
    .endr
    .rept 56
      .db 3
    .endr
  asteroid_respawn_sprite_table:
    .rept 32
      .db BROWN_ASTEROID, GREY_ASTEROID, PURPLE_ASTEROID
      .db YELLOW_ASTEROID_1, YELLOW_ASTEROID_2, BROWN_ASTEROID
      .db GREY_ASTEROID, YELLOW_ASTEROID_1
    .endr
  ;
  asteroid_respawn_parameters:
    .db ASTEROID_MAX
    .dw asteroid_init_table
    .db TRUE  ; random yspeed?
    .dw asteroid_and_shard_respawn_yspeed_table
    .db FALSE ; random xspeed?
    .dw UNUSED_WORD
    .db TRUE ; random horizontal position?
    .dw game_object_respawn_horizontal_position_table ; pointer to 256 bytes table for rnd xpos.
    .db FALSE ; random vertical position?
    .dw UNUSED_WORD
    .db TRUE ; random sprite?
    .dw asteroid_respawn_sprite_table ; pointer to sprite table
    ;
  shard_init_table:
    .db SPRITE_45, 1, 0, GAME_OBJECT_SHAPE_SMALL
    .db INVISIBLE_AREA_BOTTOM_BORDER-8, 0, GAME_OBJECT_INACTIVE
    .db UNUSED_BYTE, -(1), -(1), 10, 8, SHARD
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UNUSED_BYTE ; Timer.
    .dw UNUSED_WORD ; Timer handler.
    .db DISABLED    ; Timer state.
  ;
  shard_conversion_table:
    .db YELLOW_ASTEROID_1, YELLOW_SHARD
    .db YELLOW_ASTEROID_2, YELLOW_SHARD
    .db BROWN_ASTEROID, BROWN_SHARD
    .db GREY_ASTEROID, GREY_SHARD
    .db PURPLE_ASTEROID, PURPLE_SHARD
  shard_conversion_table_end:
  ;
  update_shard_parameters:
  ; Special handler for when the game object is in a specified horizontal zone?
  .db TRUE
  .dw shard_hits_ground
  .db SHARD_GROUND_ZONE_START, SHARD_GROUND_ZONE_END
  ; Special handler for when the game object is in a specified vertical zone?
  .db TRUE
  .dw shard_hits_blanked_column
  .db BLANKED_COLUMN_START, BLANKED_COLUMN_END
  ; animate?
  .db FALSE
  ; Put in collision table
  .db TRUE
  ;
  shard_respawn_parameters:
    .db SHARD_MAX
    .dw shard_init_table
    .db TRUE  ; random yspeed?
    .dw asteroid_and_shard_respawn_yspeed_table
    .db FALSE ; random xspeed?
    .dw UNUSED_WORD
    .db TRUE ; random horizontal position?
    .dw game_object_respawn_horizontal_position_table ; pointer to 256 bytes table for rnd xpos.
    .db FALSE ; random vertical position?
    .dw UNUSED_WORD
    .db TRUE ; random sprite?
    .dw shard_respawn_sprite_table ; pointer to sprite table
    ;
  shard_respawn_sprite_table:
    .rept 64
      .db YELLOW_SHARD_SPRITE, BROWN_SHARD_SPRITE, GREY_SHARD_SPRITE, PURPLE_SHARD_SPRITE
    .endr
    ;
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
  spinner_init_table:
    .db SPRITE_12, SPINNER_YSPEED_INIT, 0, GAME_OBJECT_SHAPE_TALL
    .db INVISIBLE_AREA_BOTTOM_BORDER-16, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 0, 0, 16, 24, SPINNER
    .db 0
    .dw spinner_anim_table
    .db UNUSED_BYTE ; Timer.
    .dw UNUSED_WORD ; Timer handler.
    .db DISABLED    ; Timer state.
  update_spinner_parameters:
    ; Special handler for when the game object is in a specified horizontal zone?
    .db TRUE
    .dw spinner_hits_ground
    .db SPINNER_GROUND_ZONE_START, SPINNER_GROUND_ZONE_END
    ; Special handler for when the game object is in a specified vertical zone?
    .db FALSE
    .dw $0000
    .db 0, 0
    ; Animate object.
    .db TRUE
    ; Put in collision table
    .db TRUE
  ;
  spinner_respawn_parameters:
    .db SPINNER_MAX
    .dw spinner_init_table
    .db FALSE  ; random yspeed?
    .dw UNUSED_WORD
    .db FALSE ; random xspeed?
    .dw UNUSED_WORD
    .db TRUE ; random horizontal position?
    .dw game_object_respawn_horizontal_position_table ; pointer to 256 bytes table for rnd xpos.
    .db FALSE ; random vertical position?
    .dw UNUSED_WORD
    .db FALSE ; random sprite?
    .dw UNUSED_WORD ; pointer to sprite table


  booster_init_table:
    .db SPRITE_18, 1, 0, GAME_OBJECT_SHAPE_REGULAR
    .db INVISIBLE_AREA_BOTTOM_BORDER-16, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 0, -2, 20, 16, BOOSTER           ; TODO: Might need adjustment!
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UNUSED_BYTE ; Timer.
    .dw UNUSED_WORD ; Timer handler.
    .db DISABLED    ; Timer state.
    ;
  booster_sprite_table:
    ; Note - only first four items are taken into account.
    .db SPRITE_18, SPRITE_19, SPRITE_20, SPRITE_19
    ;
    booster_respawn_sprite_table:
      .rept 64
        .db SPRITE_18, SPRITE_19, SPRITE_20, SPRITE_20
      .endr
    update_booster_parameters:
    ; Special handler for when the game object is in a specified horizontal zone?
    .db TRUE
    .dw booster_hits_ground
    .db BOOSTER_GROUND_ZONE_START, BOOSTER_GROUND_ZONE_END
    ; Special handler for when the game object is in a specified vertical zone?
    .db FALSE
    .dw $0000
    .db 0, 0
    ; Animate object
    .db FALSE
    ; Put in collision table
    .db TRUE
  ;
  booster_respawn_parameters:
    .db BOOSTER_MAX
    .dw booster_init_table
    .db FALSE  ; random yspeed?
    .dw UNUSED_WORD
    .db FALSE ; random xspeed?
    .dw UNUSED_WORD
    .db TRUE ; random horizontal position?
    .dw game_object_respawn_horizontal_position_table ; pointer to 256 bytes table for rnd xpos.
    .db FALSE ; random vertical position?
    .dw UNUSED_WORD
    .db TRUE ; random sprite?
    .dw booster_respawn_sprite_table ; pointer to sprite table
    ;
  missile_init_table:
    .db SPRITE_4, MISSILE_YSPEED, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db UNUSED_BYTE, 1, 4, 8, 14, MISSILE     ; Might need adjustment!
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UNUSED_BYTE ; Timer.
    .dw UNUSED_WORD ; Timer handler.
    .db DISABLED    ; Timer state.
    ;
  missile_respawn_parameters:
    .db MISSILE_MAX
    .dw missile_init_table
    .db FALSE  ; random yspeed?
    .dw UNUSED_WORD
    .db FALSE ; random xspeed?
    .dw UNUSED_WORD
    .db TRUE ; random horizontal position?
    ; could be separate missile table to make it appear in the sides
    .dw game_object_respawn_horizontal_position_table ; pointer to 256 bytes table for rnd xpos.
    .db FALSE ; random vertical position?
    .dw UNUSED_WORD
    .db FALSE ; random sprite?
    .dw UNUSED_WORD ; pointer to sprite table
  update_missile_parameters:
  ; Special handler for when the game object is in a specified horizontal zone?
  .db TRUE
  .dw booster_hits_ground
  .db MISSILE_GROUND_ZONE_START, MISSILE_GROUND_ZONE_END
  ; Special handler for when the game object is in a specified vertical zone?
  .db FALSE
  .dw UNUSED_WORD
  .db UNUSED_BYTE, UNUSED_BYTE
  ; Animate object
  .db FALSE
  ; Put in collision table
  .db TRUE
    ;
  ufo_init_table:
    .db SPRITE_21, UFO_YSPEED_INIT, UFO_XSPEED_INIT, GAME_OBJECT_SHAPE_WIDE
    .db 10, 10, GAME_OBJECT_INACTIVE
    .db %00000000, 0, 0, 32, 16, UFO
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UFO_EXPLODE_DURATION ; Timer.
    .dw deactivate_game_object ; Timer handler.
    .db DISABLED    ; Timer state.
  ;
  reverse_ufo_init_table:
    .db SPRITE_21, UFO_YSPEED_INIT, -(UFO_XSPEED_INIT), GAME_OBJECT_SHAPE_WIDE
    .db 10, 246, GAME_OBJECT_INACTIVE
    .db %00000000, 0, 0, 32, 16, UFO
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UFO_EXPLODE_DURATION ; Timer.
    .dw deactivate_game_object ; Timer handler.
    .db DISABLED    ; Timer state.
  ;
  ;
  update_ufo_parameters:
  ; Special handler for when the game object is in a specified horizontal zone?
  .db FALSE
  .dw UNUSED_WORD
  .db UNUSED_BYTE, UNUSED_BYTE
  ; Special handler for when the game object is in a specified vertical zone?
  .db TRUE
  .dw ufo_hits_blanked_column
  .db UFO_VERTICAL_ZONE_START, UFO_VERTICAL_ZONE_END
  ; Animate object
  .db FALSE
  ; Put in collision table
  .db TRUE
  ;
  ufo_respawn_parameters:
    .db UFO_MAX
    .dw ufo_init_table
    .db FALSE  ; random yspeed?
    .dw UNUSED_WORD
    .db FALSE ; random xspeed?
    .dw UNUSED_WORD
    .db FALSE ; random horizontal position?
    .dw UNUSED_WORD
    .db FALSE ; random vertical position?
    .dw UNUSED_WORD
    .db FALSE ; random sprite?
    .dw UNUSED_WORD ; pointer to sprite table
  reverse_ufo_respawn_parameters:
    .db UFO_MAX
    .dw reverse_ufo_init_table
    .db FALSE  ; random yspeed?
    .dw UNUSED_WORD
    .db FALSE ; random xspeed?
    .dw UNUSED_WORD
    .db FALSE ; random horizontal position?
    .dw UNUSED_WORD
    .db FALSE ; random vertical position?
    .dw UNUSED_WORD
    .db FALSE ; random sprite?
    .dw UNUSED_WORD ; pointer to sprite table
  ;
  ;UFO Bombs:
  ufo_bomb_init_table:
    .db SPRITE_49, UFO_BOMB_Y_SPEED, UFO_BOMB_X_SPEED, GAME_OBJECT_SHAPE_SMALL
    .db INVISIBLE_AREA_BOTTOM_BORDER-8, 0, GAME_OBJECT_INACTIVE
    .db %00000000, 0, 0, 8, 8, UFO_BOMB
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UNUSED_BYTE ; Timer.
    .dw UNUSED_WORD ; Timer handler.
    .db DISABLED    ; Timer state.
  ;
  ufo_gun_trigger_init_table:
    .db 255, UFO_GUN_INTERVAL
    .db DISABLED, DEFAULT_CYCLE_VALUE
  ;
  update_ufo_bomb_parameters:
  ; Special handler for when the game object is in a specified horizontal zone?
  .db TRUE
  .dw deactivate_game_object
  .db SHARD_DEACTIVATE_ZONE_START, SHARD_DEACTIVATE_ZONE_END ; reverse?
  ; Special handler for when the game object is in a specified vertical zone?
  .db FALSE
  .dw UNUSED_WORD
  .dw UNUSED_WORD
  ; animate?
  .db FALSE
  ; Put in collision table
  .db TRUE

  ;
  swabby_init_table:
    .db SWABBY_IDLE_SPRITE, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db SWABBY_Y_INIT, SWABBY_X_INIT, GAME_OBJECT_ACTIVE
    .db UNUSED_BYTE, 8, 5, 6, 8, SWABBY ; (1, 4, 8, 14) before diff. tweak.
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UNUSED_BYTE ; Timer.
    .dw UNUSED_WORD ; Timer handler.
    .db DISABLED    ; Timer state.
  ;
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
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UNUSED_BYTE ; Timer.
    .dw UNUSED_WORD ; Timer handler.
    .db DISABLED    ; Timer state.
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
  swabby_respawn_trigger_init_table:
    .db 255, SWABBY_RESPAWN_TRIGGER_INTERVAL
    .db DISABLED, DEFAULT_CYCLE_VALUE
    ;
  bullet_init_table:
    .db SPRITE_44, BULLET_Y_SPEED, BULLET_X_SPEED, GAME_OBJECT_SHAPE_SMALL
    .db 0, 0, GAME_OBJECT_INACTIVE
    .db UNUSED_BYTE, 0, 1, 6, 8, BULLET
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UNUSED_BYTE ; Timer.
    .dw UNUSED_WORD ; Timer handler.
    .db DISABLED    ; Timer state.
  bullet_collision_table:
    .dw missile, 1
    .dw asteroid, ASTEROID_MAX
    .dw shard, SHARD_MAX
    .dw spinner, 1
    .dw booster, 1
  bullet_collision_table_end:
  update_bullet_parameters:
  ; Special handler for when the game object is in a specified horizontal zone?
  .db TRUE
  .dw bullet_hits_deactivate_zone
  .db BULLET_DEACTIVATE_ZONE_START, BULLET_DEACTIVATE_ZONE_END ; reverse?
  ; Special handler for when the game object is in a specified vertical zone?
  .db FALSE
  .dw UNUSED_WORD
  .dw UNUSED_WORD
  ; animate?
  .db FALSE
  ; Put in collision table
  .db FALSE
  ;
  gun_level_max_string:
    .dw GUN_LEVEL_NAMETABLE_ADDRESS
    .asc "M"
    .db GUN_LEVEL_NAMETABLE_FLAGS
    .asc "A"
    .db GUN_LEVEL_NAMETABLE_FLAGS
    .asc "X"
    .db GUN_LEVEL_NAMETABLE_FLAGS
  gun_level_max_string_end:
  ;
  bee_init_table:
    .db SPRITE_65, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_SMALL
    .db 0, 8, GAME_OBJECT_ACTIVE
    .db %00000000, 0, 0, 1, 1, BEE
    .db UNUSED_BYTE ; Animation counter.
    .dw UNUSED_WORD ; Animation pointer.
    .db UNUSED_BYTE ; Timer.
    .dw UNUSED_WORD ; Timer handler.
    .db DISABLED    ; Timer state.
  ;
  ; Transition trigger:
  transition_trigger_init_table:
    .db 255, 255
    .db DISABLED, TRANSITION_TRIGGER_CYCLE_INIT
  transition_trigger_game_over_init_table:
    .db PERCENT_CHANCE_100, GAME_OVER_INTERVAL
    .db ENABLED, DEFAULT_CYCLE_VALUE
  ;
  transition_trigger_splash_screen_init_table:
    .db PERCENT_CHANCE_100, SPLASH_SCREEN_INTERVAL
    .db ENABLED, DEFAULT_CYCLE_VALUE
  ;
  transition_trigger_hiscore_init_table:
    .db PERCENT_CHANCE_100, HISCORE_INTERVAL
    .db ENABLED, HISCORE_CYCLE_VALUE
  short_interval_transition:
    .db PERCENT_CHANCE_100, 150
    .db ENABLED, 1
  blinking_text_trigger_init_table:
    .db PERCENT_CHANCE_100, 50
    .db ENABLED, 2
  transition_trigger_titlescreen_init_table:
    .db PERCENT_CHANCE_100, TITLESCREEN_INTERVAL
    .db ENABLED, TITLESCREEN_CYCLE_VALUE
  ;
  difficulty_to_trigger_init_tables:
    .dw difficulty_0_batch_trigger_init_table
    .dw difficulty_1_batch_trigger_init_table
    .dw difficulty_2_batch_trigger_init_table
    .dw difficulty_3_batch_trigger_init_table
    .dw difficulty_4_batch_trigger_init_table
  ;
  ; Triggers are batch-disabled to stop enemy spawning.
  batch_wipe_and_disable_all_triggers_table:
    .dw spinner_trigger, trigger_wipe_table
    .dw booster_trigger, trigger_wipe_table
    .dw missile_trigger, trigger_wipe_table
    .dw shard_trigger, trigger_wipe_table
    .dw asteroid_trigger, trigger_wipe_table
    .dw ufo_trigger, trigger_wipe_table
    ;.dw ufo_gun_trigger, trigger_wipe_table
  batch_wipe_and_disable_all_triggers_table_end:
  trigger_wipe_table:
    .db 0, 0, DISABLED
  ;
  swabby_triggers_batch_init_table:
    .dw swabby_death_trigger, swabby_death_trigger_init_table
    .dw swabby_respawn_trigger, swabby_respawn_trigger_init_table
  swabby_triggers_batch_init_table_end:
  ;
  ; Note! First table is used for dimensioning batch inits (lev. startup).
  difficulty_0_batch_trigger_init_table:
    .dw spinner_trigger, spinner_trigger_init_table_0
    .dw booster_trigger, booster_trigger_init_table_0
    .dw missile_trigger, missile_trigger_init_table_0
    .dw shard_trigger, shard_trigger_init_table_0
    .dw asteroid_trigger, asteroid_trigger_init_table_0
    .dw ufo_trigger, ufo_trigger_init_table_0
  difficulty_0_batch_trigger_init_table_end:
  difficulty_1_batch_trigger_init_table:
    .dw spinner_trigger, spinner_trigger_init_table_1
    .dw booster_trigger, booster_trigger_init_table_1
    .dw missile_trigger, missile_trigger_init_table_1
    .dw shard_trigger, shard_trigger_init_table_1
    .dw asteroid_trigger, asteroid_trigger_init_table_1
    .dw ufo_trigger, ufo_trigger_init_table_1
  difficulty_2_batch_trigger_init_table:
    .dw spinner_trigger, spinner_trigger_init_table_2
    .dw booster_trigger, booster_trigger_init_table_2
    .dw missile_trigger, missile_trigger_init_table_2
    .dw shard_trigger, shard_trigger_init_table_2
    .dw asteroid_trigger, asteroid_trigger_init_table_2
    .dw ufo_trigger, ufo_trigger_init_table_2
  difficulty_3_batch_trigger_init_table:
    .dw spinner_trigger, spinner_trigger_init_table_3
    .dw booster_trigger, booster_trigger_init_table_3
    .dw missile_trigger, missile_trigger_init_table_3
    .dw shard_trigger, shard_trigger_init_table_3
    .dw asteroid_trigger, asteroid_trigger_init_table_3
    .dw ufo_trigger, ufo_trigger_init_table_3
  difficulty_4_batch_trigger_init_table:
    .dw spinner_trigger, spinner_trigger_init_table_4
    .dw booster_trigger, booster_trigger_init_table_4
    .dw missile_trigger, missile_trigger_init_table_4
    .dw shard_trigger, shard_trigger_init_table_4
    .dw asteroid_trigger, asteroid_trigger_init_table_4
    .dw ufo_trigger, ufo_trigger_init_table_4
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
  ufo_trigger_init_table_0:
    .db UFO_TRIGGER_CHANCE_INIT_0, UFO_TRIGGER_INTERVAL_INIT_0
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
  ufo_trigger_init_table_1:
    .db UFO_TRIGGER_CHANCE_INIT_1, UFO_TRIGGER_INTERVAL_INIT_1
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
  ufo_trigger_init_table_2:
    .db UFO_TRIGGER_CHANCE_INIT_2, UFO_TRIGGER_INTERVAL_INIT_2
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
  ufo_trigger_init_table_3:
    .db UFO_TRIGGER_CHANCE_INIT_3, UFO_TRIGGER_INTERVAL_INIT_3
    .db ENABLED, DEFAULT_CYCLE_VALUE
  ;
  spinner_trigger_init_table_4:
    .db SPINNER_TRIGGER_CHANCE_INIT_4, SPINNER_TRIGGER_INTERVAL_INIT_4
    .db ENABLED, DEFAULT_CYCLE_VALUE
  booster_trigger_init_table_4:
    .db BOOSTER_TRIGGER_CHANCE_INIT_4, BOOSTER_TRIGGER_INTERVAL_INIT_4
    .db ENABLED, DEFAULT_CYCLE_VALUE
  missile_trigger_init_table_4:
    .db MISSILE_TRIGGER_CHANCE_INIT_4, MISSILE_TRIGGER_INTERVAL_INIT_4
    .db ENABLED, DEFAULT_CYCLE_VALUE
  shard_trigger_init_table_4:
    .db SHARD_TRIGGER_CHANCE_INIT_4, SHARD_TRIGGER_INTERVAL_INIT_4
    .db ENABLED, DEFAULT_CYCLE_VALUE
  asteroid_trigger_init_table_4:
    .db ASTEROID_TRIGGER_CHANCE_INIT_4, ASTEROID_TRIGGER_INTERVAL_INIT_4
    .db ENABLED, DEFAULT_CYCLE_VALUE
  ufo_trigger_init_table_4:
    .db UFO_TRIGGER_CHANCE_INIT_4, UFO_TRIGGER_INTERVAL_INIT_4
    .db ENABLED, DEFAULT_CYCLE_VALUE
  ;
  level_arrange_vdp_parameters:
    .db ALL_SWITCHES_ON               ; Switches.
    .db ASCII_SPACE, TILE_BANK_1      ; Wipe tilebank with character.
    .db DARK_BLUE                     ; Border color
    ;
    .db 2                             ; Number of blocks to load into vram
    .db FONT_BANK                     ; Bank
    .dw font_table                    ; Table in the format used by load_vram_from_table.
    .db SPRITE_BANK
    .dw sprite_load_table
  ;
  titlescreen_string_1:
    .asc "Press start button#"
  titlescreen_string_2:
    .asc "                  #"
  ;
  titlescreen_arrange_vdp_parameters:
    .db ALL_SWITCHES_ON               ; Switches.
    .db ASCII_SPACE, TILE_BANK_1      ; Wipe tilebank with character.
    .db DARK_PURPLE                   ; Border color
    ;
    .db 3                             ; Number of block to load into vram
    .db FONT_BANK                     ; Bank
    .dw alternative_font_table        ; Table in the format used by load_vram_from_table.
    .db TITLESCREEN_BANK
    .dw titlescreen_tiles_table
    .db TITLESCREEN_BANK
    .dw titlescreen_tilemap_table
  ;
  hiscore_arrange_vdp_parameters:
    .db ALL_SWITCHES_ON               ; Switches.
    .db ASCII_SPACE, TILE_BANK_1      ; Wipe tilebank with character.
    .db DARK_PURPLE                   ; Border color
    ;
    .db 4                             ; Number of block to load into vram
    .db SPRITE_BANK
    .dw sprite_load_table             ; Rot! Sprite tiles will be partly over-
    .db FONT_BANK                     ; written by background tiles, but we
    .dw alternative_font_table        ; need the cursor at the end of the
    .db HISCORE_BANK                  ; sprite sheet.
    .dw hiscore_tiles_table
    .db HISCORE_BANK
    .dw hiscore_tilemap_table

  ;
  full_reset_keys:
    .dw is_up_pressed, is_button_1_pressed, is_button_2_pressed
  full_reset_keys_end:
  ;
  both_player_1_buttons:
    .dw is_button_1_pressed, is_button_2_pressed
  both_player_1_buttons_end:
  splash_message_0:
    .asc "HANG-ON ENTERTAINMENT#"
  splash_message_1:
    .asc "proudly presents...#"
  splash_message_2:
    .asc "2018#"
  splash_batch_print_table:
    .dw splash_message_0
    .db 11, 6
    .dw splash_message_1
    .db 13, 7
    .dw splash_message_2
    .db 1, 27
  splash_batch_print_table_end:

  ;
  ; ---------------------------------------------------------------------------
  ; Menu item strings:
  menu_title:
    .asc "ASTROSWAB! debug menu#"
  item_1:
    .asc "Titlescreen#"
  item_2:
    .asc "Hiscore table#"
  item_3:
    .asc "Sandbox#"
  item_4:
    .asc "Console#"
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
  ;
  ;
  .db "(c) Hang-On Entertainment 2018"
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
  sprite_load_table:
    .dw SPRITE_BANK_START,
    .dw sprite_tiles_end-sprite_tiles
    .dw sprite_tiles
  ;
  sprite_tiles:
    .include "bank_3\spritesheet.png_tiles.inc"
  sprite_tiles_end:
  ;
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
  background_12_tiles:
    .include "bank_8\background_12_tiles.inc"
  background_12_tiles_end:
  background_12_tilemap:
    .include "bank_8\background_12_tilemap.inc"
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
  ufo_sound:
    .incbin "bank_5\ufo.psg"
  title_1:
    .incbin "bank_5\title_1.psg"
  grimer:
    .incbin "bank_5\grimer.psg"
  mutef:
    .incbin "bank_5\mutef.psg"


.ends
;
.bank HISCORE_BANK slot 2
; -----------------------------------------------------------------------------
.section "Hiscore assets" free
; -----------------------------------------------------------------------------
  ;
hiscore_init:
  .asc "05000"
  .asc "   "
  .asc "04500"
  .asc "   "
  .asc "04000"
  .asc "   "
  .asc "03500"
  .asc "   "
  .asc "03000"
  .asc "   "
  .asc "02500"
  .asc "   "
  .asc "02000"
  .asc "   "
  .asc "01500"
  .asc "   "
  .asc "01000"
  .asc "   "
  .asc "00500"
  .asc "   "
;
hiscore_background_tiles:
  .include "bank_9\hiscore_background_tiles.inc"
hiscore_background_tiles_end:
hiscore_background_tilemap:
  .include "bank_9\hiscore_background_tilemap.inc"
hiscore_tiles_table:
  .dw NON_ASCII_AREA_START,
  .dw hiscore_background_tiles_end-hiscore_background_tiles,
  .dw hiscore_background_tiles
hiscore_tilemap_table:
  .dw NAME_TABLE_START,
  .dw VISIBLE_NAME_TABLE_SIZE,
  .dw hiscore_background_tilemap
hiscore_track:
  .incbin "bank_5\finale.psg"
tadaa:
  .incbin "bank_5\tadaa.psg"

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


titlescreen_tiles_table:
  .dw NON_ASCII_AREA_START,
  .dw titlescreen_background_tiles_end-titlescreen_background_tiles,
  .dw titlescreen_background_tiles
titlescreen_tilemap_table:
  .dw NAME_TABLE_START,
  .dw VISIBLE_NAME_TABLE_SIZE,
  .dw titlescreen_background_tilemap

.ends
;
.bank WARNING_BANK slot 2
; -----------------------------------------------------------------------------
.section "Warning assets" free
; -----------------------------------------------------------------------------
;
warning_background_tiles:
  .include "bank_3\warning_background_tiles.inc"
warning_background_tiles_end:
warning_background_tilemap:
  .include "bank_3\warning_background_tilemap.inc"
warning_tiles_table:
  .dw NON_ASCII_AREA_START,
  .dw warning_background_tiles_end-warning_background_tiles,
  .dw warning_background_tiles
warning_tilemap_table:
  .dw NAME_TABLE_START,
  .dw VISIBLE_NAME_TABLE_SIZE,
  .dw warning_background_tilemap
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
