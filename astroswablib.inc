; astroswablib.inc
; Functions related specifically to astroswab, or with significant dependencies
; on local/contextual design choices.
;
; Definitions for referencing the sprites on the spritesheet.
; Used by functions drawing objects.
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
; . . .
.equ FIRST_8x8_SPRITE 168
.equ SPRITE_43 FIRST_8x8_SPRITE
.equ SPRITE_44 169
.equ SPRITE_45 170
.equ SPRITE_46 171
.equ SPRITE_47 172
;
.equ BATCH_INIT_TABLE_ELEMENT_SIZE 6
;
.ramsection "astroswablib variables" slot 3
  ;
  rnd_seed dw
  ;
  collision_buffer dsb 8
  ;
.ends
; -----------------------------------------------------------------------------
.section "astroswablib" free
; -----------------------------------------------------------------------------
  handle_bullet_collision:
    ; Placeholder function for handling what happens when bullet collides with
    ; other objects.
    ; TODO: Increment score depending on other object's id.
    ; IX = bullet, IY other object..
    ld a,GAME_OBJECT_INACTIVE
    ld (ix+game_object.state),a
    ld (iy+game_object.state),a
  ret
  handle_swabby_collision:
    ; Placeholder function for handling what happens when swabby collides with
    ; other objects.
    ; TODO: Implement DEATH!
    ; IX = bullet, IY other object..
    ld a,GAME_OBJECT_INACTIVE
    ld (iy+game_object.state),a
  ret
  ; ---------------------------------------------------------------------------
  ;
  batch_print:
    ; Batch print strings at different screen positions. Handy for making a
    ; menu, or for setting up text in various places. Strings and positions are
    ; stored in a table in the following format:
    ; pointer to string (word), row (byte), column (byte).
    ; Entry: IX = pointer to table.
    ;        A = number of strings to print.
    ; Uses: AF, BC, HL.
    ; Depends on print (in bluelib.inc), and on #-terminated strings.
    -:
      push af
      ld l,(ix+0)
      ld h,(ix+1)
      ld b,(ix+2)
      ld c,(ix+3)
      call print
      .rept 4
        inc ix
      .endr
      pop af
      dec a
    jp nz,-
  ret
  ; ---------------------------------------------------------------------------
  add_metasprite:
    ; Put a 16x16 metasprite in the SAT.
    ; Entry: IX = Pointer to meta sprite data block, where IX+0 is Y, IX+1 is X
    ;             and IX+2 is the character code.
    ; Uses: AF, BC, DE, HL
    ; Dependencies: add_sprite (in bluelib), and a spritesheet formatted as
    ; 2x21 16x16 images, loaded into SPRITE_BANK_START.
    ld d,4
    ld hl,meta_sprite_table
    -:
      ld a,(ix+0)
      add a,(hl)
      ld b,a
      inc hl
      ld a,(ix+1)
      add a,(hl)
      ld c,a
      inc hl
      ld a,(hl)
      ld e,a
      ld a,(ix+2)
      add a,e
      inc hl
      call add_sprite
      dec d
    jp nz,-
  ret
  meta_sprite_table:
    .db 0, 0, 0
    .db 0, 8, 1
    .db 8, 0, 4
    .db 8, 8, 5

  add_tall_metasprite:
    ; Put a 16x32 metasprite in the SAT.
    ; Entry: IX = Pointer to meta sprite data block, where IX+0 is Y, IX+1 is X
    ;             and IX+2 is the character code.
    ; Uses: AF, BC, DE, HL
    ; Dependencies: add_sprite (in bluelib), and a spritesheet formatted as
    ; 2x21 16x16 images, loaded into SPRITE_BANK_START.
    ld d,8
    ld hl,tall_meta_sprite_table
    -:
      ld a,(ix+0)
      add a,(hl)
      ld b,a
      inc hl
      ld a,(ix+1)
      add a,(hl)
      ld c,a
      inc hl
      ld a,(hl)
      ld e,a
      ld a,(ix+2)
      add a,e
      inc hl
      call add_sprite
      dec d
    jp nz,-
  ret
  tall_meta_sprite_table:
    .db 0, 0, 0
    .db 0, 8, 1
    .db 8, 0, 4
    .db 8, 8, 5
    ;
    .db 16, 0, 6
    .db 16, 8, 7
    .db 24, 0, 10
    .db 24, 8, 11
  ; ---------------------------------------------------------------------------
  profile_vblank:
    ; Set debug meter for profiling the amount of lines consumed by functions
    ; operating on the graphics and expecting to work with the screen blanked.
    ; Make sure this meter shows a line number within the vblank period!
    in a,(V_COUNTER_PORT)                   ; Get current line number.
    .ifdef DEVELOPER_MODE
      cp 185                                  ; Failsafe, go into an endless
      jp nc,+                                 ; loop if we are in the active
        -:                                    ; display (both NTSC and PAL)
          nop
        jp -
      +:                                      ; We are somewhere in vblank...
    .endif
    ld b,a                                  ; Store line number in B.
    ld a,(vblank_update_finished_line)      ; Get highest line number yet.
    cp b                                    ; Is the current line higher?
    ret nc                                  ; No, skip forward.
    ld a,b                                  ; Yes, save current line number as
    ld (vblank_update_finished_line),a      ; the new 'high score'.
  ret                                       ;
  ;
  profile_loop:
    in a,(V_COUNTER_PORT)
    cp PROFILER_MAX_LINE
    ret nc
    ld b,a                                  ; Store line number in B.
    ld a,(loop_finished_line)               ; Get highest line number yet.
    cp b                                    ; Is the current line higher?
    ret nc                                  ; No, skip forward.
    ld a,b                                  ; Yes, save current line number as
    ld (loop_finished_line),a               ; the new 'high score'.
  ret                                       ;
  ;
  reset_profilers:
    ld a,FIRST_LINE_OF_VBLANK
    ld (vblank_update_finished_line),a
    xor a
    ld (loop_finished_line),a
  ret
  ; ---------------------------------------------------------------------------
  get_random_number:
    ; SMS-Power!
    ; Returns an 8-bit pseudo-random number in a
    push hl
    ld hl,(rnd_seed)
    ld a,h         ; get high byte
    rrca           ; rotate right by 2
    rrca
    xor h          ; xor with original
    rrca           ; rotate right by 1
    xor l          ; xor with low byte
    rrca           ; rotate right by 4
    rrca
    rrca
    rrca
    xor l          ; xor again
    rra            ; rotate right by 1 through carry
    adc hl,hl      ; add RandomNumberGeneratorWord to itself
    jr nz,+
      ld hl,$733c  ; if last xor resulted in zero then re-seed.
    +:
    ld a,r         ; r = refresh register = semi-random number
    xor l          ; xor with l which is fairly random
    ld (rnd_seed),hl
    pop hl
  ret              ; return random number in a
  ;
  randomize:
    ; FIXME: Randomize based on player input instead!
    ld hl,randomizer_table
    ld a,(rnd_seed)
    add a,(hl)
    ld (rnd_seed),a
    ld hl,randomizer_table+1
    ld a,(rnd_seed+1)
    add a,(hl)
    ld (rnd_seed+1),a
  ret
  ;
  randomizer_table:
    .dbrnd 2,1,254

.ends
;
; -----------------------------------------------------------------------------
; TRIGGERS
; -----------------------------------------------------------------------------
; Triggers have...
.struct trigger
  timer db
  interval db
  chance db
  state db
  ; add interval modifier here...?
.endst
;
.section "Trigger functions" free
  batch_initialize_triggers:
    ; Initialize a batch of triggers using a table.
    ; HL = Pointer to batch init table
    ; A = number of triggers to initialize.
    ; The table is 2 words: trigger struct, init table.
    ld b,a
    -:
      ; Point IX to ram structure.
      ld e,(hl)
      inc hl
      ld d,(hl)
      inc hl
      push de
      pop ix
      ; Point HL to init table.
      ld e,(hl)
      inc hl
      ld d,(hl)
      inc hl
      push hl
        push de
        pop hl
          call initialize_trigger
      pop hl
    djnz -
  ret
  ;
  enable_trigger:
    ; Entry: IX = Pointer to trigger struct.
    ld a,ENABLED
    ld (ix+trigger.state),a
  ret
  ;
  get_trigger_state:
    ; Entry: IX = Pointer to trigger struct.
    ; Exit: Trigger state (ENABLED/DISABLED bluelib constants) in A.
    ld a,(ix+trigger.state)
  ret
  ;
  initialize_trigger:
    ; Entry: HL = Pointer to init table.
    ;        IX = Pointer to trigger struct.
    ld a,(hl)
    ld (ix+trigger.chance),a
    inc hl
    ld a,(hl)
    ld (ix+trigger.interval),a
    ld (ix+trigger.timer),a
    inc hl
    ld a,(hl)
    ld (ix+trigger.state),a
  ret
  ;
  process_trigger:
    ; Decrement trigger internal timer. If time is up, then roll a dice against
    ; the trigger chance. If successful, return with carry flag set to (time to
    ; or otherwise act on timer event) - else reset carry flag.
    ; Entry: IX = Pointer to trigger struct.
    ; Exit: Carry flag set or reset, depending on timer/trigger logic.
    ld a,(ix+trigger.timer)
    dec a
    ld (ix+trigger.timer),a
    or a                              ; Clear carry and return.
    ret nz                            ; Only action: Timer was decremented.
      ld a,(ix+trigger.interval)      ; Reset timer to interval.
      ld (ix+trigger.timer),a
      ld a,(ix+trigger.chance)        ; Get trigger.chance.
      ld b,a                          ; Store it.
      call get_random_number          ; Roll dice.
      cp b                            ; Compare dice roll to chance.
      jp nc,+                         ;
        scf                           ; Set carry flag!
        ret
    +:
    or a                              ; Clear carry flag.
  ret
.ends
;
;
;
; -----------------------------------------------------------------------------
; SANDBOX
; -----------------------------------------------------------------------------
; Work- and testbench.


; -----------------------------------------------------------------------------
.ramsection "Sandbox variables" slot 3
  ;
.ends
; -----------------------------------------------------------------------------
.section "Sandbox functions" free
  ; Short text here...
  sandbox_function_1:
    ; ...
  ret
  ;
  add_multiple_game_object_instances_to_collision_table:
    ; Entry: IX = Pointer to game object with multiple instances.
    ;        A = number of instances to position.
    ; Uses: AF, B, DE, HL, IX
    ld b,a
    ld de,_sizeof_game_object
    -:
      push bc
      call add_object_to_collision_table
      add ix,de
      pop bc
    djnz -
  ret

  add_object_to_collision_table:
    ; IX = Pointer to object.
    ; Uses: AF, BC, DE, HL
    ld a,(ix+game_object.y)
    add a,(ix+game_object.hitbox_offset_y)

    cp COLLISION_TABLE_DIVIDER
    jp nc,+
      ld hl,objects_in_top_collision_table
      ld a,(hl)
      inc (hl)
      ld hl,top_collision_table
      jp ++
    +:
      ld hl,objects_in_bottom_collision_table
      ld a,(hl)
      inc (hl)
      ld hl,bottom_collision_table
    ++:
    ;
    cp 0
    jp z,+
    ld b,a
    xor a
    -:
      add a,COLLISION_TABLE_ELEMENT_SIZE ; Table element size here!
    djnz -
    ld d,0
    ld e,a
    add hl,de
    +:
    ;
    ld a,(ix+game_object.y) ; Apply hitbox offset.
    add a,(ix+game_object.hitbox_offset_y)
    ld (hl),a
    inc hl
    ld a,(ix+game_object.x)
    add a,(ix+game_object.hitbox_offset_x)
    ld (hl),a
    inc hl
    ld a,(ix+game_object.hitbox_width)
    ld (hl),a
    inc hl
    ld a,(ix+game_object.hitbox_height)
    ld (hl),a
    inc hl
    ;
    ; Add pointer to object
    push ix
    pop de
    ld (hl),e
    inc hl
    ld (hl),d
  ret
  begin_collision_tables:
    xor a
    ld (objects_in_top_collision_table),a
    ld (objects_in_bottom_collision_table),a
    ;
  ret
;
  ;
  detect_collision_using_collision_table:
    ; Version 3 notes:
    ; Assumes game objects to be initialized with
    ; separate bytes for y and x hitbox offset.
    ;
    ; Simple form of collision detection: Axis-aligned bounding box.
    ; If (rect1.x < rect2.x + rect2.width &&  (coll1)
    ; rect1.x + rect1.width > rect2.x &&      (coll2)
    ; rect1.y < rect2.y + rect2.height &&     (coll3)
    ; rect1.height + rect1.y > rect2.y)       (coll4)
    ;  --> collision detected!
    ; We perform these four tests one at a time. If any one test fails,
    ; there is no collision, and we return with carry reset. If no test fails
    ; then we have collision, and return with carry set.
    ; Uses: AF, B, IY
    ;
    ; Determine relevant collision table.
    ld a,(ix+game_object.hitbox_offset_y)
    add a,(ix+game_object.y)
    cp COLLISION_TABLE_DIVIDER
    jp nc,+
      ld hl,top_collision_table
      ld a,(objects_in_top_collision_table)
      or a
      ret z
      jp ++
    +:
      ld hl,bottom_collision_table
      ld a,(objects_in_bottom_collision_table)
      or a
      ret z
    ++:
    push hl
    pop iy
    ld b,a
    -:
      push bc
        ;
        call coll1_3
        jp nc,+
        call coll2_3
        jp nc,+
        call coll3_3
        jp nc,+
        call coll4_3
        jp nc,+
          ; Fall through: Collision!
          ; Return with carry set, and with pointer in IY.
          ld l,(iy+4)
          ld h,(iy+5)
          push hl
          pop iy
          pop bc ; clean stack
          ret
        +:
        ld de,COLLISION_TABLE_ELEMENT_SIZE
        add iy,de
      pop bc
    djnz -

  ret
    coll1_3:
      ; rect1.x < rect2.x + rect2.width
      ld a,(iy+1) ;
      add a,(iy+2)
      ld b,a
      ld a,(ix+game_object.hitbox_offset_x)
      add a,(ix+game_object.x)
      cp b
    ret
    coll2_3:
      ; rect1.x + rect1.width > rect2.x
      ld a,(ix+game_object.x)
      add a,(ix+game_object.hitbox_offset_x)
      add a,(ix+game_object.hitbox_width)
      ld b,a
      ld a,(iy+1)
      cp b
    ret
    coll3_3:
      ; rect1.y < rect2.y + rect2.height
      ld a,(iy+0)
      ld b,(iy+3)
      add a,b
      ld b,a
      ld a,(ix+game_object.y)
      add a,(ix+game_object.hitbox_offset_y)
      cp b
    ret
    coll4_3:
      ; rect1.height + rect1.y > rect2.y
      ld a,(ix+game_object.y)
      add a,(ix+game_object.hitbox_offset_y)
      add a,(ix+game_object.hitbox_height)
      ld b,a
      ld a,(iy+0)
      cp b
    ret
    ;

.ends
;
