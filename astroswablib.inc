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
  ; -----------------------------------------------------------------------------
  detect_collision:
    ; Entry: IX = Pointer to game object 1
    ;        IY = Pointer to game object 2
    ; Exit: Carry flag is set or reset depending on overlap between the
    ;       hitboxes of the two game objects.
    ; Uses: Everything but IX.
    push ix
      call load_buffer_from_game_objects
      call detect_collision_using_buffer
    pop ix
  ret
  ;
    load_buffer_from_game_objects:
      ; Load the collision buffer with hitbox data from the two game objects.
      ; Entry: IX & IY = Pointer to game objects.
      ;
      ; Exit:
      ; Uses: All registers, including IX & IY, collision_buffer.
      ld hl,collision_buffer
      call load_one_object    ; Load x, y, width and height from first object.
      push iy                 ; Load pointer to second object into IX.
      pop ix
      call load_one_object    ; Load hitbox data from second object.
    ret
    ;
      load_one_object:
        ; Load 4 bytes of hitbox data from object ram structure to buffer.
        ld a,(ix+game_object.x)               ; Get object's x-position.
        ld b,a
        ; Calculate and load hitbox x.
        ld a,(ix+game_object.hitbox_offset)   ; Get hitbox x-offset.
        and %11110000                         ; Apply mask to eliminate
        rra                                   ; y-offset (offset is 4 bits!).
        rra                                   ; Rotate x-offset bits into
        rra                                   ; position.
        rra
        add a,b                               ; Add offset to object's x-pos.
        ld (hl),a                             ; Load value into buffer.
        inc hl                                ; Advance buffer pointer.
        ; Calculate and load hitbox y.
        ld b,(ix+game_object.y)               ; Get and apply y-offset in a
        ld a,(ix+game_object.hitbox_offset)   ; similar way...
        and %00001111
        add a,b
        ld (hl),a
        inc hl
        ; Load hitbox width and height.
        ld a,(ix+game_object.hitbox_width)    ; Just copy to the buffer.
        ld (hl),a
        inc hl
        ld a,(ix+game_object.hitbox_height)
        ld (hl),a
        inc hl
      ret
      ;
    ;
    detect_collision_using_buffer:
      ; Simple form of collision detection: Axis-aligned bounding box.
      ; If (rect1.x < rect2.x + rect2.width &&  (coll1)
      ; rect1.x + rect1.width > rect2.x &&      (coll2)
      ; rect1.y < rect2.y + rect2.height &&     (coll3)
      ; rect1.height + rect1.y > rect2.y)       (coll4)
      ;  --> collision detected!
      ; We perform these four tests on at a time. If any one test fails,
      ; there is no collision, and we return with carry reset. If no test fails
      ; then we have collision, and return with carry set.
      ; Uses: AF, B, collision_buffer
      ld ix,collision_buffer
      call coll1
      ret nc
      call coll2
      ret nc
      call coll3
      ret nc
      call coll4
      ret nc
      ; Collision! Return with carry set.
    ret
      coll1:
        ; rect1.x < rect2.x + rect2.width
        ld a,(ix+4)
        ld b,(ix+6)
        add a,b
        ld b,a
        ld a,(ix+0)
        cp b
      ret
      coll2:
        ; rect1.x + rect1.width > rect2.x
        ld a,(ix+0)
        ld b,(ix+2)
        add a,b
        ld b,a
        ld a,(ix+4)
        sub b
      ret
      coll3:
        ; rect1.y < rect2.y + rect2.height
        ld a,(ix+5)
        ld b,(ix+7)
        add a,b
        ld b,a
        ld a,(ix+1)
        cp b
      ret
      coll4:
        ; rect1.height + rect1.y > rect2.y
        ld a,(ix+3)
        ld b,(ix+1)
        add a,b
        ld b,a
        ld a,(ix+5)
        cp b
      ret
      ;
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

; -----------------------------------------------------------------------------
; GAME OBJECTS
; -----------------------------------------------------------------------------
.struct game_object
  y db
  x db
  sprite db
  yspeed db
  xspeed db
  state db
  shape db
  hitbox_offset db  ;xxxx yyyy DEPRECATED - treat as dead byte
  hitbox_offset_y db
  hitbox_offset_x db
  hitbox_width db
  hitbox_height db
  anim_counter db
  anim_pointer dw
  id db
.endst
;
.equ GAME_OBJECT_ACTIVE 1
.equ GAME_OBJECT_INACTIVE 0
.equ GAME_OBJECT_SHAPE_REGULAR 0
.equ GAME_OBJECT_SHAPE_TALL 1
.equ GAME_OBJECT_SHAPE_WIDE 2
.equ GAME_OBJECT_SHAPE_SMALL 3
; -----------------------------------------------------------------------------
.section "Game objects" free
  ; Functions that work on the Astroswab game_object structure.
  ; What Astroswab game objects are assumed to have in common:
  ; 1. ...
  ; 2. ...
  activate_game_object:
    ; Entry: IX = Pointer to struct.
    ; Registers used: None.
    push af
    ld a,GAME_OBJECT_ACTIVE
    ld (ix+game_object.state),a
    pop af
  ret
  ;
  animate_game_object:
    ; Entry: IX = Pointer to struct.
    SAVE_REGISTERS
    ld a,(ix+game_object.anim_counter)
    inc a
    ld (ix+game_object.anim_counter),a
    ld d,0
    ld e,a
    ld l,(ix+game_object.anim_pointer)
    ld h,(ix+game_object.anim_pointer+1)
    add hl,de
    ld a,(hl)
    ld (ix+game_object.sprite),a
    RESTORE_REGISTERS
  ret
  ;
  batch_collision_detect:
    ; Collision test a game object, pointed to by IX, against a table of other
    ; game objects in a table pointed to by HL. Return relevant info in
    ; registers that allows for further processing in case of a collision.
    ; Entry: HL = Batch collision test table
    ;        A  = Number of objects in collision test table.
    ;        IX = Pointer to game object
    ;
    ; Exit: 1) Carry set on collision, and (in case of collision):
    ;       2) IY points to object of collision.
    ;       3) Holds ID of the object (same as iy+game_object.id).
    ld b,a                                ; For each object in collision table.
    --:
      push bc                             ; Save loop counter.
        ld e,(hl)                         ; Get object pointer from table and
        inc hl                            ; save it in IY...
        ld d,(hl)
        inc hl
        push de
        pop iy
        ld b,(hl)                         ; How many instances of this object
        inc hl                            ; to test?
        inc hl                            ; Skip 1 byte in the collision table.
        -:                                ; For each instance, do...
          push bc                         ; Save counter.
            push iy                       ; Save pointer to object.
              ld a,(iy+game_object.state)
              cp GAME_OBJECT_ACTIVE
              jp nz,+
                push hl
                call detect_collision     ; Perform the collision test.
                pop hl
                jp nc,+                   ; Returns carry on collision.
                  ld a,(iy+game_object.id); Load object id into A.
                  pop iy                  ; Restore IY to point to obj.
                  pop bc                  ; Align the stack...
                  pop bc
                  ret                       ; Exit func with carry set.
              +:                          ; Else - no collision here...
            pop iy                        ; Restore pointer to object.
            ld de,_sizeof_game_object     ; Point to next instance.
            add iy,de
          pop bc                          ; Restore instance loop counter.
        djnz -                            ; Loop through all instances...
      pop bc                              ; Restore object counter.
    djnz --                               ; Loop through all listed objects.
  ret
  ;
  batch_initialize_game_objects:
    ; Initialize a batch of game objects using a table.
    ; HL = Pointer to batch init table
    ; A = number of objects to initialize.
    ; Example of 6 byte wide batch init table:
    ;     .dw swabby, swabby_init_table, 1
    ;     .dw bullet, bullet_init_table, BULLET_MAX
    ; Uses: AF, BC, DE, HL, IX
    ld b,a
    --:
      push bc
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
        ld a,(hl)
        ld b,a
        inc hl
        inc hl
        push hl
          push de
          pop hl
          ld de,_sizeof_game_object
            -:
            call initialize_game_object
            add ix,de
            djnz -
        pop hl
      pop bc
    djnz --
  ret
  batch_draw_game_objects:
    ; Batch draw a number of game objects.
    ; HL = table with game objects.
    ; A = number of game objects to draw.
    ;       Example of batch draw table:
    ;       .dw asteroid, 3, swabby, 1
    ;       The number makes it possible to draw arrayed objects, i.e. bullets.
    ; Uses: AF, BC, DE, HL, IX
    ld b,a
    --:
      push bc
        ld e,(hl)
        inc hl
        ld d,(hl)
        inc hl
        push de
        pop ix
        ld b,(hl)
        inc hl
        inc hl
        ld de,_sizeof_game_object
        -:
          push bc
            call draw_game_object
            add ix,de
          pop bc
        djnz -
      pop bc
    djnz --
  ret
  ;
  draw_game_object:
    ; Entry: IX = Pointer to struct.
    ; Registers used: None.
    SAVE_REGISTERS                ; Because add_metasprite mess with registers!
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    jp nz,draw_game_object_end
      ; Put active objects in the SAT buffer.
      ld a,(ix+game_object.sprite)
      cp FIRST_8x8_SPRITE
      jp nc,small_shape
      ld a,(ix+game_object.shape)
      cp GAME_OBJECT_SHAPE_REGULAR
      jp z,regular_shape
      cp GAME_OBJECT_SHAPE_TALL
      jp z,tall_shape
    draw_game_object_end:
    RESTORE_REGISTERS
  ret
  small_shape:
    ld c,(ix+game_object.x)
    ld b,(ix+game_object.y)
    ld a,(ix+game_object.sprite)
    call add_sprite
  jp draw_game_object_end
  regular_shape:
    call add_metasprite
  jp draw_game_object_end
  tall_shape:
    call add_tall_metasprite
  jp draw_game_object_end
  ;
  deactivate_game_object:
    ; Entry: IX = Pointer to struct.
    ; Registers used: None.
    push af
    ld a,GAME_OBJECT_INACTIVE
    ld (ix+game_object.state),a
    pop af
  ret
  ;
  get_game_object_hitbox:
    ; Entry: IX = pointer to struct.
    ld a,(ix+game_object.shape)
    cp GAME_OBJECT_SHAPE_SMALL
    jp nz,+
      ld a,8
      ret
    +:
    ld a,16
  ret
  ;
  get_game_object_position:
    ; Entry: IX = pointer to struct.
    ; Exit: A = object Y and B = object X.
    ld a,(ix+game_object.y)
    ld b,(ix+game_object.x)
  ret
  ;
  get_game_object_state:
    ; Entry: IX = pointer to struct.
    ; Exit: Returns in A state of the enemy object, which should be either
    ; GAME_OBJECT_INACTIVE or GAME_OBJECT_ACTIVE.
    ld a,(ix+game_object.state)
  ret
  ;
  get_game_object_x:
    ; Entry: IX = pointer to struct.
    ; Exit: Returns in A x of the enemy object.
    ld a,(ix+game_object.x)
  ret
  ;
  get_game_object_xspeed:
    ; Entry: IX = pointer to struct.
    ; Exit: Returns in A  game object xspeed.
    ld a,(ix+game_object.xspeed)
  ret
  ;
  get_inactive_game_object:
    ; Entry: IX = Pointer to first instance of struct.
    ;        A = Instances of this struct.
    ; Exit:  If no inactive object is found, then carry is set.
    ;        Else: IX = Pointer to inactive game object and carry is cleared.
    ld b,a
    ld de,_sizeof_game_object
    -:
      call get_game_object_state
      cp GAME_OBJECT_INACTIVE
      jp z,+
      add ix,de
    djnz -
    ; Here we come if no inactive objects are found...
    ; Set carry to indicate error and return.
    scf
    ret
    +:
      or a        ; Clear carry.
  ret             ; Return with ix pointing...
  ;
  ;
  horizontal_zone_deactivate_game_object:
    ; Test enemy object y to see if it is within the horizontal zone defined
    ; the borders passed in A and B. If true, then deactivate enemy object.
    ; Entry: IX = Pointer to struct.
    ;        A = Zone start. B = Zone end.
    ; Uses: AF, BC.
    ld c,b
    ld b,a
    ld a,(ix+game_object.y)
    cp b
    ret c
    cp c
    ret nc
    call deactivate_game_object
  ret
  ;
  initialize_game_object:
    ; Entry: IX = Pointer to struct.
    ;        HL = Pointer to game object init data:
    ;        (sprite, yspeed, xspeed, shape, y, x, state
    ;        hitbox_offset, hitbox_width, hitbox_height).
    ; Example: asteroid_setup_table:
    ;         .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    ;         .db 0, 0, GAME_OBJECT_INACTIVE, %00000000, 16, 16
    ; Uses: A - but HL is preserved.
    push hl
    ld a,(hl)
    ld (ix+game_object.sprite),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.yspeed),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.xspeed),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.shape),a
    ;
    inc hl
    ld a,(hl)
    ld (ix+game_object.y),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.x),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.state),a
    ;
    inc hl
    ld a,(hl)
    ld (ix+game_object.hitbox_offset),a
    inc hl
    ;
    ld a,(hl)
    ld (ix+game_object.hitbox_offset_y),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.hitbox_offset_x),a
    inc hl
    ;
    ld a,(hl)
    ld (ix+game_object.hitbox_width),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.hitbox_height),a
    inc hl
    ld a,(hl)
    ld (ix+game_object.id),a
    pop hl
  ret
  ;
  load_animation_game_object:
    ; Entry: IX = Pointer to struct.
    ;        HL = Pointer to animation table.
    ld (ix+game_object.anim_pointer),l
    ld (ix+game_object.anim_pointer+1),h
  ret
  ;
  move_game_object:
    ; Apply yspeed to y and xspeed to x.
    ; Entry: IX = Pointer to instance of enemy object.
    ld a,(ix+game_object.yspeed)
    ld b,a
    ld a,(ix+game_object.y)
    add a,b
    ld (ix+game_object.y),a
    ;
    ld a,(ix+game_object.xspeed)
    ld b,a
    ld a,(ix+game_object.x)
    add a,b
    ld (ix+game_object.x),a
  ret
  ;
  position_multiple_game_object_instances:
    ; Entry: IX = Pointer to game object with multiple instances.
    ;        A = number of instances to position.
    ;        HL = pointer to position table, containing Y,X pairs.
    ; Uses: AF, B, DE, HL, IX
    ld b,a
    ld de,_sizeof_game_object
    -:
      ld a,(hl)
      ld (ix+game_object.y),a
      inc hl
      ld a,(hl)
      ld (ix+game_object.x),a
      inc hl
      add ix,de
    djnz -
  ret
  ;
  spawn_game_object_in_invisible_area:
    ; Entry: IX = Pointer to enemy object.
    ; Object gets a random position and is activated!
    ld a,INVISIBLE_AREA_BOTTOM_BORDER-8
    ld (ix+game_object.y),a
    call get_random_number
    and %01111111             ; rnd(128).
    ld b,a
    call get_random_number
    and %00111111             ; rnd(64).
    add a,b
    ld b,a
    call get_random_number
    and %00011111             ; rnd(32).
    add a,b
    add a,8                   ; x = (0-127) + (0-63) + (0-31) + 8.
    ld (ix+game_object.x),a  ; x = 8 - 229.
    call activate_game_object
  ret
  ;
  ;
  set_game_object_position:
    ; Entry: IX = pointer to struct.
    ;        A = Y, B = X.
    ld (ix+game_object.y),a
    ld (ix+game_object.x),b
  ret
  set_game_object_speed:
    ; IX = Pointer to enemy object.
    ; A = Yspeed, B = Xspeed.
    ld (ix+game_object.yspeed),a
    ld (ix+game_object.xspeed),b
  ret
  ;
  set_game_object_sprite:
    ; Entry: IX = Pointer to enemy object.
    ;        A = Sprite index.
    ld (ix+game_object.sprite),a
  ret
  ;
.ends
;
;
; -----------------------------------------------------------------------------
; SANDBOX
; -----------------------------------------------------------------------------
; Work- and testbench.
  .equ SANDBOX_BULLET_Y 0

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
  add_object_to_collision_table:
    ; IX = Pointer to object.
    ld a,(ix+game_object.y)
    cp COLLISION_TABLE_DIVIDER ; adjust to real playfield half!
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
    add a,a
    add a,a
    ld d,0
    ld e,a
    add hl,de
    ;
    ld b,(ix+game_object.y) ; Apply hitbox offset.
    ld a,(ix+game_object.hitbox_offset)
    and %00001111
    add a,b
    ld (hl),a
    inc hl
    ld b,(ix+game_object.x)
    ld a,(ix+game_object.hitbox_offset)
    and %11110000
    rra
    rra
    rra
    rra
    add a,b
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
  detect_collision_using_collision_table:
    ; Simple form of collision detection: Axis-aligned bounding box.
    ; If (rect1.x < rect2.x + rect2.width &&  (coll1)
    ; rect1.x + rect1.width > rect2.x &&      (coll2)
    ; rect1.y < rect2.y + rect2.height &&     (coll3)
    ; rect1.height + rect1.y > rect2.y)       (coll4)
    ;  --> collision detected!
    ; We perform these four tests on at a time. If any one test fails,
    ; there is no collision, and we return with carry reset. If no test fails
    ; then we have collision, and return with carry set.
    ; Uses: AF, B, IY
    ;
    ; Determine relevant collision table.
    ld a,(ix+game_object.hitbox_offset)
    and %00001111
    ld b,(ix+game_object.y)
    add a,b
    cp COLLISION_TABLE_DIVIDER
    jp nc,+
      ld hl,top_collision_table
      jp ++
    +:
      ld hl,bottom_collision_table
    ++:
    push hl
    pop iy
    ;
    call coll1_2
    ret nc
    call coll2_2
    ret nc
    call coll3_2
    ret nc
    call coll4_2
    ret nc
    ; Collision! Return with carry set, and with pointer to object in table
    ; passed in IY:
    ld l,(iy+4)
    ld h,(iy+5)
    push hl
    pop iy
  ret
    coll1_2:
      ; rect1.x < rect2.x + rect2.width
      ld a,(iy+1) ;
      ld b,(iy+2) ; !
      add a,b
      ld b,a
      ld a, (ix+game_object.hitbox_offset)
      and %11110000
      rra
      rra
      rra
      rra
      ld c,(ix+game_object.x)
      add a,c
      cp b
    ret
    coll2_2:
      ; rect1.x + rect1.width > rect2.x
      ld c,(ix+game_object.x)
      ld a,(ix+game_object.hitbox_offset)
      and %11110000
      rra
      rra
      rra
      rra
      add a,c
      ld b,(ix+game_object.hitbox_width)
      add a,b
      ld b,a
      ld a,(iy+1)
      sub b
    ret
    coll3_2:
      ; rect1.y < rect2.y + rect2.height
      ld a,(iy+0)
      ld b,(iy+3)
      add a,b
      ld b,a
      ld c,(ix+game_object.y)
      ld a,(ix+game_object.hitbox_offset)
      and %00001111
      add a,c
      cp b
    ret
    coll4_2:
      ; rect1.height + rect1.y > rect2.y
      ld c,(ix+game_object.y)
      ld a,(ix+game_object.hitbox_offset)
      and %00001111
      add a,c
      ld b,(ix+game_object.hitbox_height)
      add a,b
      ld b,a
      ld a,(iy+0)
      cp b
    ret
    ;
  ;
  v3_detect_collision_using_collision_table:
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
    ; We perform these four tests on at a time. If any one test fails,
    ; there is no collision, and we return with carry reset. If no test fails
    ; then we have collision, and return with carry set.
    ; Uses: AF, B, IY
    ;
    ; Determine relevant collision table.
    ld a,(ix+game_object.hitbox_offset_y)
    cp COLLISION_TABLE_DIVIDER
    jp nc,+
      ld hl,top_collision_table
      jp ++
    +:
      ld hl,bottom_collision_table
    ++:
    push hl
    pop iy
    ;
    call coll1_3
    ret nc
    call coll2_3
    ret nc
    call coll3_3
    ret nc
    call coll4_3
    ret nc
    ; Fall through: Collision!
    ; Return with carry set, and with pointer in IY.
    ld l,(iy+4)
    ld h,(iy+5)
    push hl
    pop iy
  ret
    coll1_3:
      ; rect1.x < rect2.x + rect2.width
      ld a,(iy+1) ;
      ld b,(iy+2) ; ! ADD (II+n)! Optimize
      add a,b
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
      ld a,(iy+1) ; optimize here - something with cp (ii+n)
      sub b
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
