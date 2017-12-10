; main.asm
; Main code for astroswab.
;
; - Abstraction level 3: Generic code.
.include "bluelib.inc"        ; General library with foundation stuff.
                              ; Includes memory map, help functions etc.
.include "testlib.inc"        ; Rudimentary unit testing (depends on bluelib).
.include "psglib.inc"         ; Stand-alone music and SFX lib (c) sverx.
;
; - Abstraction level 2: Semi-general structs and struct-related functions.
;                             ; (The "Astroswab Engine").
.include "objectlib.inc"      ; Game objects like asteroids and Swabby.
.include "triggerlib.inc"     ; Timed triggers controlling enemy respawning.
.include "scorelib.inc"       ; Keeping score and displaying hiscore table.
;
; - Abstraction level 1: Game-specific code and implementations of structs.
.include "astroswablib.inc"   ; Metasprites, collision handling, etc.
;
.include "header.inc"         ; Constants and struct instantiations.
;
; -----------------------------------------------------------------------------
.section "main" free
; -----------------------------------------------------------------------------
  init:
    ; Run this function once (on game load).
    ; Load the pico-8 palette to colors 16-31.
    ld a,SPRITE_PALETTE_START
    ld b,PICO8_PALETTE_SIZE
    ld hl,pico8_palette_sms
    call load_cram
    ; Load the pico-8 palette to colors 0-15.
    ld a,BACKGROUND_PALETTE_START
    ld b,PICO8_PALETTE_SIZE
    ld hl,pico8_palette_sms
    call load_cram
    ; Load the font tiles.
    SELECT_BANK FONT_BANK
    ld hl,font_table
    call load_vram_from_table
    ;
    ;
    call PSGInit
    ; Go to the initial game state specified in the header.
    ld a,INITIAL_GAME_STATE
    ld (game_state),a
  jp main_loop
  ;
  ; ---------------------------------------------------------------------------
  main_loop:
    ; Note: This loop can begin on any line - wait for vblank in the states!
    ld a,(game_state)   ; Get current game state - it will serve as JT offset.
    add a,a             ; Double it up because jump table is word-sized.
    ld h,0              ; Set up HL as the jump table offset.
    ld l,a
    ld de,game_state_jump_table ; Point to JT base address (see footer.inc).
    add hl,de           ; Apply offset to base address.
    ld a,(hl)           ; Get LSB from table.
    inc hl              ; Increment pointer.
    ld h,(hl)           ; Get MSB from table.
    ld l,a              ; HL now contains the address of the state handler.
    jp (hl)             ; Jump to this handler - note, not call!
    ;
  ; ---------------------------------------------------------------------------
  ; S E T U P  G A M E  A N D  S E T U P / R U N  L E V E L                                                        (gameplay)
  ; ---------------------------------------------------------------------------
  .include "gs_level.inc"             ; Prepare and run levels.
  ;
  setup_new_game:
    ;
    ; Perform test: Is this the first time ever this game is run?
    SELECT_EXTRAM
      ld a,(FIRST_GAME_BYTE)
      cp FIRST_GAME_ID
      jp nz,+
        ; Game is running for the first time...
        ld a,FIRST_GAME_ID
        ld (FIRST_GAME_BYTE),a      ; Stamp EXTRAM byte.
        SELECT_ROM
        SELECT_BANK HISCORE_BANK
          ld hl,hiscore_init        ; Hiscore initialization data.
          ld de,hiscore_item.1      ; Start of hiscore table.
          call copy_hiscore_table   ; Initialize hiscore table.
        SELECT_EXTRAM
        ld hl,HISCORE_EXTRAM_ADDRESS
        call save_hiscore_table_to_extram
      +:
    SELECT_ROM
    ; -------------------------------------------------------------------------
    ld a,INITIAL_DIFFICULTY           ; Set difficulty.
    ld (difficulty),a
    ;
    xor a                             ; Reset gun
    ld (gun_level),a
    ld a,TRUE
    ld (gun_level_flag),a             ; Update the gun level counter gfx.
    ld hl,gun_level_char_data_init
    ld de,gun_level_char_data
    ld bc,4
    ldir
    ; Reset player and peak score.
    ld hl,player_score_init
    ld de,player_score
    call copy_score_and_increment_pointers
    ld hl,player_score_init
    ld de,peak_score
    call copy_score_and_increment_pointers
    ld a,FALSE
    ld (new_hiscore),a
    ;
    ld a,GS_PREPARE_LEVEL             ; When this game session is set up, go
    ld (game_state),a                 ; on and prepare a relevant level...
  jp main_loop
  ;
  ; ---------------------------------------------------------------------------
  ; D E V E L O P M E N T  M E N U
  ; ---------------------------------------------------------------------------
  .include "gs_devmenu.inc"
  ;
  ; ---------------------------------------------------------------------------
  ; T I T L E S C R E E N
  ; ---------------------------------------------------------------------------
  .include "gs_titlescreen.inc"
  ;
  ; ---------------------------------------------------------------------------
  ; H I S C O R E
  ; ---------------------------------------------------------------------------
  .include "gs_hiscore.inc"
  ;
  ; ---------------------------------------------------------------------------
  ; S A N D B O X
  ; ---------------------------------------------------------------------------
  .include "gs_sandbox.inc"
.ends
;
.include "footer.inc"
