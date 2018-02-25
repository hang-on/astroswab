;2018-01-03:
update_game_object:
  ; Move and draw (add here...) game object. Only if active.
  ; IX = Pointer to game object.
  ; IY = Pointer to parameter string.
  ;
  ld a,(ix+game_object.state)
  cp GAME_OBJECT_ACTIVE
  ret nz
    ;
    ld a,(iy+0)       ; Make const for offset!
    cp TRUE           ; TODO: DRY-fix vertical/horiz. handlers
    call z,handle_active_game_objects_in_horizontal_zone
    ;
    ld a,(iy+5)
    cp TRUE           ; TODO: DRY-fix vertical/horiz. handlers
    call z,handle_active_game_objects_in_vertical_zone
    ;
    call move_game_object
    call draw_game_object
    call add_game_object_to_collision_table

ret

; 2017-12-19
subtract_from_score_old:
  ; Subtract a number passed in B from the digit specified in A. Update the
  ; other digits in the score struct as necessary. Prevents score from going
  ; below zero.
  ; Entry:  A = Digit to subtract from.
  ;         B = Number to subtract (non-ascii!).
  ;        HL = Pointer to score struct.
  ; Exit:
  ; Uses: AF, BC, DE, HL, IX
  ;
  push hl
  pop ix                            ; Save score pointer in IX
  ld d,0
  ld e,score_struct.ten_thousands
  add hl,de                         ; HL now points to ten thousands.
  ld c,(hl)                         ; Store this (for underflow test).
  push ix                           ; Retrieve original pointer.
  pop hl
  ld d,0
  add a,score_struct.ten_thousands
  ld e,a
  add hl,de                         ; Point HL to digit to subtract from.
  ld a,(hl)                         ; Get current value of digit.
  sub b                             ; Subtract the required number.
  ld (hl),a                         ; Store it back into the digit.
  cp ASCII_ZERO                     ; Is this digit now below zero?
  jp nc,+                           ;
    add a,10                        ; Yes? - 'borrow' from next digit.
    ld (hl),a                       ; Save in this digit.
    -:
      ; Out of bounds test:         ; Are we pointing outside the score struct now?
      ;push ix                       ; Get the original pointer.
      ;pop de                        ; Store in DE.
      ;push hl                       ; Save pointer to current digit.
        ;inc hl                      ; Increment pointer.
        ;sbc hl,de                   ; Are we at the first digit now?
      ;pop hl                        ; Retrieve pointer to digit.
      ;jp z,+                        ; Skip forward if HL = DE.
      ;                             ; 'Borrow' from next digit (1>10>100>1.000>10.000).
      dec hl                        ; Point to this digit.
      dec (hl)                      ; Decrement it (perform the borrow).
      ld a,(hl)                     ; Is this digit now below zero?
      cp ASCII_ZERO
      jp nc,+                       ; If not, then proceed to finish.
        add a,10                    ; Else, perform another borrow...
        ld (hl),a
    jp -
  +:
  push ix
  pop hl
  ; Underflow test - prevent score from going below zero to 9????.
  ld d,0
  ld e,score_struct.ten_thousands
  add hl,de
  ld a,(hl)
  ;                                       ; Test 1: Is the ten thousands
  cp ASCII_ZERO+9                         ; digit now "9"?
  ret nz                                  ; Yes? - proceed to test 2.
    ld a,ASCII_ZERO                       ; Test 2: Was this digit "0"
    cp c                                  ; before subtraction?
    ret nz                                ; Yes? - reset score.
      ex de,hl                            ; Switch to destination (DE).
      ld hl,reset_score_data              ; Point to reset data.
      ld bc,SCORE_DIGITS_TOTAL            ; Number of digits to reset.
      ldir                                ; Do it.
ret


;2017-12-10
;
; Preset peak score for testing.
;.ifdef DEVELOPER_MODE
;  ld hl,peak_score_test_data
;  ld de,peak_score
;  call copy_score_and_increment_pointers
;  jp +
;    peak_score_test_data:
;      .asc "01900"
;      .asc "   "
;  +:
;.endif

;2017-10-20
test_rebuild_hiscore_table_against_fine_score:
  ld hl,fine_score_init
  ld de,player_score
  call copy_score_and_increment_pointers
  ;
  ld hl,hiscore_init
  ld de,hiscore_item.1
  call copy_hiscore_table
  ;
  ld hl,player_score
  call rebuild_hiscore_table
  ;
  ld ix,hiscore_item.3
  ld a,(ix+score_struct.ones)
  assertEqualsA ASCII_ZERO+3,"Rebuild table (Fine) failed (1)"
  ld ix,hiscore_item.1
  ld a,(ix+score_struct.ones)
  assertEqualsA ASCII_ZERO+5,"Rebuild table (Fine) failed (2)"
ret


call test_copy_score
call test_init_hiscore
call test_compare_scores
call test_rebuild_hiscore_table_against_multe_score
call test_rebuild_hiscore_table_against_felix_score

test_copy_score:
  ld hl,score_1_init
  ld de,score_1
  call copy_score_and_increment_pointers
  ld ix,score_1
  ld a,(ix+score_struct.ones)
  assertEqualsA ASCII_ONE,"copy_score failed"
ret
test_init_hiscore:
  ld hl,hiscore_init
  ld de,hiscore_item
  call copy_hiscore_table
  ;
  ld ix,hiscore_item.1
  ld a,(ix+score_struct.ones)
  assertEqualsA ASCII_ZERO+4,"init_hiscore failed (1)"
  ;
  ld ix,hiscore_item.3
  ld a,(ix+score_struct.ones)
  assertEqualsA ASCII_ZERO+2,"init_hiscore failed (2)"
ret
;
test_compare_scores:
  ld hl,felix_score_init
  ld de,score_1
  call copy_score_and_increment_pointers
  ;
  ld hl,hiscore_init
  ld de,hiscore_item
  call copy_hiscore_table
  ;
  ld iy,score_1
  ld ix,hiscore_item.3
  call compare_scores
  assertCarrySet "Compare scores failed"
ret
;
test_rebuild_hiscore_table_against_multe_score:
  ld hl,multe_score_init
  ld de,player_score
  call copy_score_and_increment_pointers
  ;
  ld hl,hiscore_init
  ld de,hiscore_item
  call copy_hiscore_table
  ;
  ld hl,player_score
  call rebuild_hiscore_table
  ;
  ld ix,hiscore_item.3
  ld a,(ix+score_struct.ones)
  assertEqualsA ASCII_ZERO+2,"Rebuild table (Multe) failed"
ret
;
.equ ASCII_F 38
test_rebuild_hiscore_table_against_felix_score:
  ld hl,felix_score_init
  ld de,player_score
  call copy_score_and_increment_pointers
  ;
  ld hl,hiscore_init
  ld de,hiscore_item
  call copy_hiscore_table
  ;
  ld hl,player_score
  call rebuild_hiscore_table
  ;
  ld ix,hiscore_item.3
  ld a,(ix+score_struct.initials)
  assertEqualsA ASCII_F,"Rebuild table (Felix) failed"
ret


test_increment_pointers:
  ld hl,hiscore_item.1
  ld (hiscore_table_pointer),hl
  ;
  INCREMENT_SCORE_TABLE_POINTER hiscore_table_pointer
  ld hl,(hiscore_table_pointer)
  assertEqualsHL hiscore_item.2,"Increment pointers failed"
ret

.macro INCREMENT_SCORE_TABLE_POINTER
  ; Entry: \1 = Score table pointer.
  ; Exit: Pointer at \1 is incremented by the size of one score struct.
  ; Uses: None
  push de
  push hl
    ld hl,(\1)
    ld de,_sizeof_score_struct
    add hl,de
    ld (\1),hl
  pop hl
  pop de
.endm


;2017-10-19
test_compare_hiscore_table_and_player_score:
  ld a,21
  ld b,4
  call set_cursor


  ; Init hiscore table
  ld a,NUMBER_OF_HISCORE_ITEMS
  ld ix,hi_score
  ld hl,hi_score_init_table
  call init_score_array
  ; Init player score
  ld de,player_score
  ld hl,luna_score
  call init_score
  ;
  ; compare scores
  ld iy,player_score
  ld ix,hi_score ; should be hi_score_table
  ld a,NUMBER_OF_HISCORE_ITEMS
  call calculate_address_and_rank_in_score_table
  jp c,+
    call insert_score_into_score_table
  +:
  ;
  ; Print hiscore table
  ld a,NUMBER_OF_HISCORE_ITEMS
  ld ix,hi_score
  call print_score_array
  ; print player score
  ld a,INCLUDE_INITIALS
  ld hl,player_score
  call print_score

ret


;
insert_score_into_score_table:
  ; IN: IY, IX
  ld bc,_sizeof_score_struct
  push ix
  pop de
  push iy
  pop hl
  ldir
ret
;
calculate_address_and_rank_in_score_table:
  ; IN IY, IX, A = number of hisscore item
  ; Out: Return in IX position of score IY in table that starts at ix.
  ; Carry set IY is not strong enough to be in table.
  ld b,a
  -:
    push bc
      call compare_scores
      jp nc,+
        ld a,(ix+score_struct.name_table_address)
        ld (iy+score_struct.name_table_address),a
        ld a,(ix+score_struct.name_table_address+1)
        ld (iy+score_struct.name_table_address+1),a
        pop bc
        or a
        ret
      +:
      ld de,_sizeof_score_struct
      add ix,de
    pop bc
  djnz -
  scf
ret
;
compare_scores:
  ; if score in iy is equal or higher, then set carry. Else reset.
  ; IY and IX
  ld a,(ix+score_struct.ten_thousands)
  cp (iy+score_struct.ten_thousands)
  jp c,iy_is_higher
  jp z,+
  jp ix_is_higher
  ;
  +:
  ld a,(ix+score_struct.thousands)
  cp (iy+score_struct.thousands)
  jp c,iy_is_higher
  jp z,+
  jp ix_is_higher
  +:
  ld a,(ix+score_struct.hundreds)
  cp (iy+score_struct.hundreds)
  jp c,iy_is_higher
  jp z,+
  jp ix_is_higher
  +:
  ld a,(ix+score_struct.tens)
  cp (iy+score_struct.tens)
  jp c,iy_is_higher
  jp z,+
  jp ix_is_higher
  ;
  +:
  ld a,(ix+score_struct.ones)
  cp (iy+score_struct.ones)
  jp c,iy_is_higher
  jp z,iy_is_higher
  jp ix_is_higher
  ;
  iy_is_higher: ; iy higher or equal.
    scf
    ret
  ix_is_higher:
    or a
    ret
ret
;
print_score_array:
  ld b,a
  -:
    push bc
      push ix
      pop hl
      ld a,INCLUDE_INITIALS
      call print_score
      ;
      ld de,_sizeof_score_struct
      add ix,de
    pop bc
  djnz -
ret
;
;
;
.equ INCLUDE_INITIALS %00000001
.equ EXCLUDE_INITIALS %00000000
.equ INCLUDE_INITIALS_BIT 0
print_score:
  ; Totally unsafe printing of the score digits.
  ; Entry: A = Include initials (bit set/reset)
  ;        HL = Pointer to score struct.
  ; Uses: A, B, DE, HL.
  push af
    ;
    ld d,0
    ld e,score_struct.name_table_address
    add hl,de
    ld a,(hl)
    out (CONTROL_PORT),a
    inc hl
    ld a,(hl)
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    ;
    ld b,SCORE_DIGITS_TOTAL
    -:
      inc hl
      ld a,(hl)
      out (DATA_PORT),a             ; Write digit.
      push ix
      pop ix
      ld a,%00000000
      out (DATA_PORT),a             ; Write 2nd byte to name table.
    djnz -
    pop af
  bit INCLUDE_INITIALS_BIT,a
  ret z
    ; OK - we should also print the initials associated with this score.
    ld a,ASCII_SPACE
    out (DATA_PORT),a             ; Print space as separator.
    push ix
    pop ix
    ld a,%00000000
    out (DATA_PORT),a             ; Write 2nd byte to name table.
    ld b,LETTERS_IN_INITIALS      ; Print the letters in the initials.
    -:
      inc hl
      ld a,(hl)
      out (DATA_PORT),a
      push ix
      pop ix
      ld a,%00000000
      out (DATA_PORT),a
    djnz -
    ;
ret

;
init_score:
  ; Initialize score struct.
  ; Entry: DE = Pointer to struct.
  ;        HL = Pointer to init data, se example below.
  ; Uses: AF, BC, DE, HL.
  ld bc,_sizeof_score_struct
  ldir
ret
  score_init_example:
    .db 0         ; Id.
    .db 1         ; Rank.
    .dw $3800     ; Name table address for fast printing.
    .asc "00000"  ; Init value.
    .asc "FAR"    ; Initials
    ;
init_score_array:
  ; Initialize score items in an array.
  ; IN: A = Number of score items, IX = Pointer to first score item[0].,
  ; HL = Init data.
  ; Uses:....
  ld b,a
  -:
    push bc
      push ix
      pop de
      call init_score
      ld de,_sizeof_score_struct
      add ix,de
    pop bc
  djnz -
ret
;
return_score_digit:
  ; Return in A the value (ASCII) of a specified digit in the score struct.
  ; Entry: A = Digit to return.
  ;        HL = Pointer to score struct.
  ; Exit:  A = Value of digit.
  ; Uses: AF, DE, HL
  add a,score_struct.ten_thousands
  ld d,0
  ld e,a
  add hl,de
  ld a,(hl)
ret
;
add_to_score:
  ; Add a number passed in B to the digit specified in A. Update the other
  ; digits in the score struct as necessary. Credit to Jonathan Cauldwell.
  ; Entry:  A = Digit to add to
  ;         B = Number to add (non-ascii!)
  ;        HL = Pointer to score struct.
  ; Uses: AF, DE, HL
  ld d,0
  add a,score_struct.ten_thousands
  ld e,a
  add hl,de
  ld a,b
  add a,(hl)
  ld (hl),a
  cp ASCII_ZERO+10
  ret c
    sub 10
    ld (hl),a
    -:
      dec hl
      inc (hl)
      ld a,(hl)
      cp ASCII_ZERO+10
      ret c
        sub 10
        ld (hl),a
    jp -
    ;
ret
;
subtract_from_score:
  ; Subtract a number passed in B from the digit specified in A. Update the
  ; other digits in the score struct as necessary.
  ; Entry:  A = Digit to subtract from.
  ;         B = Number to subtract (non-ascii!).
  ;        HL = Pointer to score struct.
  ld d,0
  add a,score_struct.ten_thousands
  ld e,a
  add hl,de
  ld a,(hl)
  sub b
  ld (hl),a
  cp ASCII_ZERO
  ret nc
    add a,10
    ld (hl),a
    -:
      dec hl
      dec (hl)
      ld a,(hl)
      cp ASCII_ZERO
      ret nc
        add a,10
        ld (hl),a
    jp -
    ;
ret


;2017-10-18
set_score_name_table_addresses_from_table:
  ld b,a
  -:
    push bc
      ld a,(hl)
      ld (ix+score_struct.name_table_address),a
      inc hl
      ld a,(hl)
      ld (ix+score_struct.name_table_address+1),a
      inc hl
      ;
      ld de,_sizeof_score_struct
      add ix,de
    pop bc
  djnz -

ret



find_score_item_by_rank:
  ; IN: IX=Base struct array, A, Number of items, B = rank
  push bc
  push de
    ld c,b
    ld b,a
    ld de,_sizeof_score_struct
    -:
      ld a,(ix+score_struct.rank)
      cp c
      jp z,+
        add ix,de
    djnz -
    +:
  pop de
  pop bc
ret
;
set_score_name_table_addresses_from_table_sort_by_rank:
  ; Assign the first address in the table pointed to by HL to the score item
  ; with rank 1. Then assign the next address to item ranked #2, and so on...
  ; IN: A = number of score items, B= highest position (usually 1)
  ; HL = name table address table (source), ix = first item in array (destination).
  ld c,b
  ld b,a
  ld d,a
  -:
    push bc
    push ix
    push de
      ld a,d
      ld b,c
      call find_score_item_by_rank ; Rank should be assumed from position in table!
      ld a,(hl)
      ld (ix+score_struct.name_table_address),a
      inc hl
      ld a,(hl)
      ld (ix+score_struct.name_table_address+1),a
      inc hl
      ;
      ld de,_sizeof_score_struct
      add ix,de
    pop de
    pop ix
    pop bc
    inc c
  djnz -

ret



  ; Reposition and sort. (kind of like 'build hiscore table')
  ;ld a,NUMBER_OF_HISCORE_ITEMS
  ;ld b,HISCORE_TABLE_TOP_POSITION
  ;ld hl,hi_score_name_table_address_table
  ;ld ix,hi_score
  ;call set_score_name_table_addresses_from_table_sort_by_rank


test_print_sorted_and_repositioned_score_array:
  ; Init
  ld a,NUMBER_OF_HISCORE_ITEMS
  ld ix,hi_score
  ld hl,hi_score_init_table
  call init_score_array
  ; TODO

  ;
  ; Reposition and sort.
  ld a,NUMBER_OF_HISCORE_ITEMS
  ld b,HISCORE_TABLE_TOP_POSITION
  ld hl,hi_score_name_table_address_table
  ld ix,hi_score
  call set_score_name_table_addresses_from_table_sort_by_rank
  ; Print
  ld a,NUMBER_OF_HISCORE_ITEMS
  ld ix,hi_score
  call print_score_array
ret


;2017-10-17
test_print_score_w_initials:
  ;
  ld a,INCLUDE_INITIALS
  ld hl,score_init_data_example
  call print_score

ret
test_print_unsorted_score_table:
  ;
  ld a,NUMBER_OF_SCORE_ITEMS
  ld ix,score
  ld hl,score_init_table
  call init_score_array
  ;
  ld b,NUMBER_OF_SCORE_ITEMS
  ld ix,score
  -:
    push bc
      push ix
      pop hl
      ld a,INCLUDE_INITIALS
      call print_score
      ;
      ld de,_sizeof_score_struct
      add ix,de
    pop bc
  djnz -
ret


test_sub_1_from_10:
  ld hl,test_data_1
  ld de,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call subtract_from_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  call return_score_digit
  assertEqualsA ASCII_NINE,"test_sub_1_from_10 failed"
ret
test_data_1:
  .db 0         ; Id.
  .dw $3aa0     ; Name table address for fast printing.
  .asc "00010"  ; Init value.

;
test_add_2_then_sub_1:
  ld hl,score_init_data_example
  ld de,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,2
  call add_to_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call subtract_from_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  call return_score_digit
  assertEqualsA ASCII_ONE,"test_add_2_then_sub_1 failed"
ret
;
test_add_1_then_sub_1:
  ld hl,score_init_data_example
  ld de,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call add_to_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call subtract_from_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  call return_score_digit
  assertEqualsA ASCII_ZERO,"test_add_1_then_sub_1 failed"
ret


;2017-10-16
test_shoot_one_asteroid:
  ld hl,score_init_data_example
  ld de,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_TENS
  ld b,1
  call add_to_score
  ;
  ld hl,player_score
  call print_score
ret


; 2017-10-15

test_add_1_visual:
  ld hl,score_init_data_example
  ld de,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call add_to_score
  ;
  ld hl,player_score
  call print_score
ret

test_add_1_then_9_visual:
  ld hl,score_init_data_example
  ld de,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call add_to_score
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,9
  call add_to_score
  ;
  ld hl,player_score
  call print_score
ret

;
test_add_1_then_9:
  ld hl,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call add_to_score
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,9
  call add_to_score
  ;
  ld a,SCORE_ONES
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"test_add_1_then_9 failed"
  ld a,SCORE_TENS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ONE,"test_add_1_then_9 failed"
ret


test_add_1:
  ld hl,player_score
  call init_score
  ;
  ld hl,player_score
  ld a,SCORE_ONES
  ld b,1
  call add_to_score
  ;
  ld a,SCORE_ONES
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ONE,"test_add_1_failed"
ret
;
test_all_digits_reset:
  ld hl,player_score
  call init_score
  ld a,SCORE_ONES
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_all_digits_reset"
  ld a,SCORE_TENS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_all_digits_reset"
  ld a,SCORE_HUNDREDS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_all_digits_reset"
  ld a,SCORE_THOUSANDS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_all_digits_reset"
  ld a,SCORE_TEN_THOUANDS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_all_digits_reset"
ret

test_resetted_ones:
  ld hl,player_score
  call init_score
  ld a,SCORE_ONES
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_resetted_ones"
ret
;
test_resetted_tens:
  ld hl,player_score
  call init_score
  ld a,SCORE_TENS
  ld hl,player_score
  call return_score_digit
  assertEqualsA ASCII_ZERO,"! test_resetted_tens"
ret


; 2017-09-10
test_10:
  ld a,3
  ld b,4
  ld hl,converter_test_table
  call convert_byte
  assertEqualsA 30, "Test 10 failed"
  ld a,2
  ld b,4
  ld hl,converter_test_table
  call convert_byte
  assertCarryReset "Test 10.2 failed"
  ;assertEqualsA 20, "Test 10 failed"
  ld a,5
  ld b,4
  ld hl,converter_test_table
  call convert_byte
  ;assertEqualsA 5, "Test 10 failed"
  assertCarrySet "Test 10.1 failed"

ret


; 2017-09-08

test_1:
  ld ix,sandbox_trigger
  ld a,(ix+trigger.state)
  cp ENABLED
  jp nz,++
    call process_trigger
    jp nc,+
      ;
      push ix
        ld ix,asteroid
        call explode_asteroid_into_shards
      pop ix
      ld a,DISABLED
      ld (ix+trigger.state),a
    +:
  ++:
  ;
  ld b,2
  ld ix,shard
  -:
    push bc
    ld a,(ix+game_object.state)
    cp GAME_OBJECT_ACTIVE
    jp nz,+
      call move_game_object              ; Move shard.
      ld a,SHARD_DEACTIVATE_ZONE_START
      ld b,SHARD_DEACTIVATE_ZONE_END
      call horizontal_zone_deactivate_game_object
      ld a,VERTICAL_DEACTIVATE_ZONE_START
      ld b,VERTICAL_DEACTIVATE_ZONE_END
      call vertical_zone_deactivate_game_object
      ;
      call draw_game_object              ; Put it in the SAT.
    ;
    +:
    ld de,_sizeof_game_object
    add ix,de
    pop bc
  djnz -

  ld ix,asteroid
  ld a,(ix+game_object.state)
  cp GAME_OBJECT_ACTIVE
  jp nz,+
    call move_game_object
    call draw_game_object
  +:
  call is_reset_pressed
  jp nc,+
    ld a,GS_PREPARE_SANDBOX
    ld (game_state),a
  +:
ret
  sandbox_game_object_init_table:
    .dw shard, sandbox_shard_init_table, SHARD_MAX
    .dw asteroid, sandbox_asteroid_init_table, 1
  sandbox_shard_init_table:
    .db SPRITE_45, 0, 0, GAME_OBJECT_SHAPE_SMALL
    .db 2, 0, GAME_OBJECT_INACTIVE
    .db %00010001, 7, 7, SHARD
  sandbox_asteroid_init_table:
    .db SPRITE_8, 1, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 0, 200, GAME_OBJECT_ACTIVE
    .db %00000000, 15, 15, ASTEROID
  sandbox_shard_position_table:
    .db 35, 70, 45, 80
  sandbox_trigger_init_table:
    .db 255, 40



ld hl,sandbox_game_object_init_table
ld a,2
call batch_initialize_game_objects
ld a,2
ld ix, shard
ld hl,sandbox_shard_position_table
call position_multiple_game_object_instances
ld hl,sandbox_trigger_init_table
ld ix,sandbox_trigger
call initialize_trigger
ld a,ENABLED
ld (ix+trigger.state),a


  apply_offset_to_game_object:
    ; IX = Pointer, B=Y, C= X
    ld a,(ix+game_object.y)
    add a,b
    ld (ix+game_object.y),a
    ;
    ld a,(ix+game_object.x)
    add a,c
    ld (ix+game_object.x),a
  ret



; 2017-09-06
get_trigger_state:
  ; Entry: IX = Pointer to trigger struct.
  ; Exit: Trigger state (ENABLED/DISABLED bluelib constants) in A.
  ld a,(ix+trigger.state)
ret
;


;2017-09-05
test_1:
  ld ix,bee
  ld hl,bee_init_table
  call initialize_game_object
  ;
  ld a,COLLISION_TABLE_DIVIDER
  call wait_for_scanline
  in a,(V_COUNTER_PORT)
  assertEqualsA COLLISION_TABLE_DIVIDER, "Test 1 failed"
  ;
  ld ix,bee
  call mark_scanline
ret

;2017-09-03
;
handle_collision:
  ld a,TRUE
  ld (temp_byte),a
  push iy
  pop hl
  ld (temp_word),hl
ret

sandbox_swabby_init_table:
.db SWABBY_IDLE_SPRITE, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
.db 135, 80, GAME_OBJECT_ACTIVE
.db %00000001, 0, 1, 15, 14, SWABBY
sandbox_asteroid_init_table:
.db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
.db 20, 20, GAME_OBJECT_ACTIVE
.db %00000000, 0, 0, 16, 16, ASTEROID
sandbox_bullet_init_table:
.db SPRITE_44, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_SMALL
.db 25, 24, GAME_OBJECT_ACTIVE
.db %00100000, 2, 0, 4, 8, BULLET
sandbox_game_object_init_table:
.dw bullet, sandbox_bullet_init_table, 5
.dw asteroid, sandbox_asteroid_init_table, 3
sandbox_batch_draw_table:
.dw bullet, 5
.dw asteroid, 3
sandbox_bullet_position_table:
.db 10, 10, 30, 20, 50, 30, 70, 40, 90, 50
sandbox_asteroid_position_table:
.db 150, 20, 100, 20, 100, 50

test_6:
; Test 6 is a clone of test 5, testing new coll.detect function.
ld ix,swabby
ld hl,sandbox_swabby_init_table
call initialize_game_object
call draw_game_object
;
;
ld hl,sandbox_game_object_init_table
ld a,2
call batch_initialize_game_objects
;
ld hl,sandbox_bullet_position_table
ld a,5
ld ix,bullet
call position_multiple_game_object_instances
;
call begin_collision_tables
;
ld hl,sandbox_asteroid_position_table
ld a,3
ld ix,asteroid
call position_multiple_game_object_instances
ld a,3
ld ix,asteroid
call add_multiple_game_object_instances_to_collision_table

ld a,2
ld hl,sandbox_batch_draw_table
call batch_draw_game_objects

;
ld ix,bullet
ld b,BULLET_MAX
-:
  ; Only do this for active bullets!
  push bc
  call detect_collision_using_collision_table
  call c,handle_collision
  pop bc
  ld de,_sizeof_game_object
  add ix,de
djnz -
assertEqualsByte temp_byte, TRUE, "Test 6.1 failed"
assertEqualsWord temp_word, asteroid, "Test 6.2 failed"
ret


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
    ld hl,collision_table
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


; 2017-09-02

test_4:

  ld ix,swabby
  ld hl,sandbox_swabby_init_table
  call initialize_game_object
  call draw_game_object
  ;
  ld ix,bullet
  ld hl,sandbox_bullet_init_table
  call initialize_game_object
  call draw_game_object
  ;
  call begin_collision_tables
  ;
  ld ix,asteroid
  ld hl,sandbox_asteroid_init_table
  call initialize_game_object
  call draw_game_object
  ld ix,asteroid
  call add_game_object_to_collision_table
  ;
  ld ix,bullet
  call detect_collision_using_collision_table
  push iy
  pop hl
  assertCarrySet "Test 4.1 failed"
  assertEqualsHL asteroid, "Test 4.2 failed"
ret


test_5:
  ld ix,swabby
  ld hl,sandbox_swabby_init_table
  call initialize_game_object
  call draw_game_object
  ;
  ;
  ld hl,sandbox_game_object_init_table
  ld a,1
  call batch_initialize_game_objects
  ld hl,sandbox_position_table
  ld a,5
  ld ix,bullet
  call position_multiple_game_object_instances
  ld a,5
  ld hl,sandbox_batch_draw_table
  call batch_draw_game_objects
  ;
  call begin_collision_tables
  ;
  ld ix,asteroid
  ld hl,sandbox_asteroid_init_table
  call initialize_game_object
  call draw_game_object
  ld ix,asteroid
  call add_game_object_to_collision_table
  ;
  ld ix,bullet
  ld b,BULLET_MAX
  -:
    ; Only do this for active bullets!
    push bc
    call detect_collision_using_collision_table
    call c,handle_collision
    pop bc
    ld de,_sizeof_game_object
    add ix,de
  djnz -
  assertEqualsByte temp_byte, TRUE, "Test 5.1 failed"
  assertEqualsWord temp_word, asteroid, "Test 5.2 failed"
ret


sandbox_shard_init_table:
  .db SPRITE_45, 1, 0, GAME_OBJECT_SHAPE_SMALL
  .db 0, 0, GAME_OBJECT_ACTIVE
  .db %00010001, 7, 7, SHARD           ; TODO: Might need adjustment!
sandbox_missile_init_table:
  .db SPRITE_4, 2, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
  .db 20, 10, GAME_OBJECT_ACTIVE
  .db %00010001, 14, 14, MISSILE     ; Might need adjustment!
sandbox_booster_init_table:
  .db SPRITE_18, 1, 0, GAME_OBJECT_SHAPE_REGULAR
  .db 30, 90, GAME_OBJECT_ACTIVE
  .db %00000000, 16, 16, BOOSTER           ; TODO: Might need adjustment!
sandbox_spinner_init_table:
  .db SPRITE_12, 1, 0, GAME_OBJECT_SHAPE_TALL
  .db 120, 180, GAME_OBJECT_ACTIVE
  .db %00000000, 16, 24, SPINNER

  sandbox_game_object_init_table:
    .dw asteroid, sandbox_asteroid_init_table, 1
  sandbox_asteroid_init_table_2:
    .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
    .db 90, 40, GAME_OBJECT_ACTIVE
    .db %00000000, 16, 16, ASTEROID
  sandbox_game_object_init_table_2:
    .dw asteroid, sandbox_asteroid_init_table_2, 1
    .dw bullet, sandbox_bullet_init_table, 1
    .dw swabby, sandbox_swabby_init_table, 1
    .dw booster, sandbox_booster_init_table, 1

    ;
    sandbox_batch_draw_table:
      .dw bullet, 1
      .dw shard, 2
      .dw swabby, 1
    sandbox_bullet_position_table:
      .db 50, 80, 90, 130
    sandbox_shard_position_table:
      .db 40, 70, 40, 90, 100, 100, 100, 140
    sandbox_batch_collision_test_table:
      .dw shard, 4
    ;
    sandbox_game_object_init_table_3:
      .dw asteroid, sandbox_asteroid_init_table_3, 1
      .dw bullet, sandbox_bullet_init_table, 1
      .dw swabby, sandbox_swabby_init_table, 1
    sandbox_asteroid_init_table_3:
      .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
      .db 0, 0, GAME_OBJECT_ACTIVE
      .db %00000000, 15, 15, ASTEROID


test_3_bullet_collides:
  ld hl,sandbox_game_object_init_table_2
  ld a,3
  call batch_initialize_game_objects

  ld ix,swabby
  call draw_game_object
  ;
  ld ix,bullet
  call draw_game_object
  ;
  call begin_collision_tables
  ;
  ld ix,asteroid
  call draw_game_object
  call add_game_object_to_collision_table
  ;
  ld ix,bullet
  ld hl,bottom_collision_table
  call test_game_object_against_collision_table
  assertCarrySet "Test 3.1 failed"
  ;
  ;
  ;
ret

; 2017-09-01
test_1_asteroid_in_top_table:
  ld hl,sandbox_game_object_init_table
  ld a,1
  call batch_initialize_game_objects

  ld ix,asteroid
  ;call draw_game_object
  ;
  call begin_collision_tables
  ld a,(objects_in_collision_table)
  assertEqualsA 0, "Test 1.0 failed"
  ;
  call add_game_object_to_collision_table
  ;
  ;
  call await_frame_interrupt
  ld a,(collision_table)
  assertEqualsA 40, "Test 1.1 failed"
  ld a,(collision_table+2)
  assertEqualsA 16, "Test 1.2 failed"

ret

test_2_asteroid_in_bottom_table:
  ld hl,sandbox_game_object_init_table_2
  ld a,1
  call batch_initialize_game_objects

  ld ix,asteroid
  call draw_game_object
  ;
  call begin_collision_tables
  ld a,(objects_in_bottom_collision_table)
  assertEqualsA 0, "Test 2.0 failed"
  ;
  call add_game_object_to_collision_table
  ;
  ;
  call await_frame_interrupt
  ld a,(bottom_collision_table)
  assertEqualsA 90, "Test 2.1 failed"
  ld a,(bottom_collision_table+2)
  assertEqualsA 16, "Test 2.2 failed"
ret


ld hl,sandbox_bullet_position_table
ld a,2 ; check!
ld ix,bullet
call position_multiple_game_object_instances

ld hl,sandbox_shard_position_table
ld a,4 ; check!
ld ix,shard
call position_multiple_game_object_instances

ld hl,sandbox_batch_draw_table
ld a,3 ; check!
call batch_draw_game_objects


; 2017-08-28

test_batch_collision_dectection:
  ld hl,sandbox_game_object_init_table
  ld a,2
  call batch_initialize_game_objects
  ld hl,sandbox_batch_draw_table
  ld a,2
  call batch_draw_game_objects
  ld ix,bullet
  ld hl,sandbox_batch_collision_test_table
  ld a,1
  call batch_collision_detect
  push iy
  pop hl
  assertCarrySet "Test 1.1 failed"
  assertEqualsHL asteroid, "Test 1.2 failed"
ret
;
test_2_no_collision:
  ld hl,sandbox_game_object_init_table_2
  ld a,2
  call batch_initialize_game_objects
  ld hl,sandbox_batch_draw_table_2
  ld a,2
  call batch_draw_game_objects
  ld ix,bullet
  ld hl,sandbox_batch_collision_test_table_2
  ld a,1
  call batch_collision_detect
  assertCarryReset "Test 2.1 failed"
ret

test_3_multiple_asteroids_no_collision:
  ld hl,sandbox_game_object_init_table_3
  ld a,2
  call batch_initialize_game_objects

  ld hl,sandbox_position_table_3
  ld a,3 ; check!
  ld ix,asteroid
  call position_multiple_game_object_instances

  ld a,2 ; check!
  ld hl,sandbox_batch_draw_table_3
  call batch_draw_game_objects

  ld ix,bullet
  ld hl,sandbox_batch_collision_test_table_3
  ld a,1
  call batch_collision_detect
  assertCarryReset "Test 3.1 failed"
ret


test_4_multiple_asteroids_and_collision:
  ld hl,sandbox_game_object_init_table_3
  ld a,2
  call batch_initialize_game_objects

  ld hl,sandbox_position_table_4
  ld a,3 ; check!
  ld ix,asteroid
  call position_multiple_game_object_instances

  ld a,2 ; check!
  ld hl,sandbox_batch_draw_table_3
  call batch_draw_game_objects

  ld ix,bullet
  ld hl,sandbox_batch_collision_test_table_3
  ld a,1
  call batch_collision_detect
  assertCarrySet "Test 4.1 failed"
  assertEqualsA ASTEROID, "Test 4.2 failed"
ret


sandbox_game_object_init_table:
  .dw bullet, sandbox_bullet_init_table, 1
  .dw asteroid, sandbox_asteroid_init_table, 1
sandbox_batch_draw_table:
  .dw bullet, 1
  .dw asteroid, 1
sandbox_batch_collision_test_table:
  .dw asteroid, 1
sandbox_bullet_init_table:
  .db SPRITE_44, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_SMALL
  .db 7, 7, GAME_OBJECT_ACTIVE
  .db %00100000, 4, 8
sandbox_asteroid_init_table:
  .db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
  .db 4, 4, GAME_OBJECT_ACTIVE
  .db %00000000, 16, 16
;
sandbox_game_object_init_table_2:
.dw bullet, sandbox_bullet_init_table_2, 1
.dw asteroid, sandbox_asteroid_init_table_2, 1
sandbox_batch_draw_table_2:
.dw bullet, 1
.dw asteroid, 1
sandbox_batch_collision_test_table_2:
.dw asteroid, 1
sandbox_bullet_init_table_2:
.db SPRITE_44, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_SMALL
.db 7, 7, GAME_OBJECT_ACTIVE, BULLET
.db %00100000, 4, 8
sandbox_asteroid_init_table_2:
.db SPRITE_8, NOT_MOVING, NOT_MOVING, GAME_OBJECT_SHAPE_REGULAR
.db 24, 24, GAME_OBJECT_ACTIVE
.db %00000000, 16, 16, ASTEROID


sandbox_game_object_init_table_3:
  .dw bullet, sandbox_bullet_init_table_2, 1
  .dw asteroid, sandbox_asteroid_init_table_2, 3
sandbox_batch_draw_table_3:
  .dw bullet, 1
  .dw asteroid, 3
sandbox_batch_collision_test_table_3:
  .dw asteroid, 3
sandbox_position_table_3:
  .db 20, 10, 20, 30, 20, 50
sandbox_position_table_4:
  .db 20, 10, 5, 5, 20, 50



; ---
test_rect1_separate_from_rect2:
  ld ix,rect1
  ld hl,rect1_init_data_2
  call init_rect
  ld ix,rect2
  ld hl,rect2_init_data_2
  call init_rect
  ld ix,rect1
  ld iy,rect2
  call detect_collision
  assertCarryReset "Test 2 failed"
ret
test_rect2_under_rect1:
  ld ix,rect1
  ld hl,rect1_init_data_3
  call init_rect
  ld ix,rect2
  ld hl,rect2_init_data_3
  call init_rect
  ld ix,rect1
  ld iy,rect2
  call detect_collision
  assertCarryReset "Test 3 failed"
ret
test_rect2_overlaps_rect1:
  ld ix,rect1
  ld hl,rect1_init_data_4
  call init_rect
  ld ix,rect2
  ld hl,rect2_init_data_4
  call init_rect
  ld ix,rect1
  ld iy,rect2
  call detect_collision
  assertCarrySet "Test 4 failed"
ret
test_rect1_overlaps_rect2_version2:
  ld hl,test5_data
  call load_buffer
  call detect_collision_using_buffer
  assertCarrySet "Test 5 failed"
ret
test_rect1_separate_from_rect2_version2:
  ld hl,test6_data
  call load_buffer
  call detect_collision_using_buffer
  assertCarryReset "Test 6 failed"
ret
test_rect1_overlaps_rect2:
  ld ix,rect1
  ld hl,rect1_init_data
  call init_rect
  ld ix,rect2
  ld hl,rect2_init_data
  call init_rect
  ld ix,rect1
  ld iy,rect2
  call detect_collision
  assertCarrySet "Test 1 failed"
ret

test_rect1_overlaps_rect2:
  ld ix,bullet
  ld hl,sandbox_bullet_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_BULLET_Y
  ld b,SANDBOX_BULLET_X
  call set_game_object_position
  call activate_game_object
  ;call draw_game_object

  ld ix,asteroid
  ld hl,sandbox_asteroid_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_ASTEROID_Y
  ld b,SANDBOX_ASTEROID_X
  call set_game_object_position
  call activate_game_object
  ;call draw_game_object

  ld ix,bullet
  ld iy,asteroid
  call detect_collision
  assertCarrySet "Test 1 failed"
ret
;
test_rect1_separate_from_rect2:
  ld ix,bullet
  ld hl,sandbox_bullet_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_BULLET_Y
  ld b,SANDBOX_BULLET_X
  call set_game_object_position
  call activate_game_object
  ;call draw_game_object

  ld ix,asteroid
  ld hl,sandbox_asteroid_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_ASTEROID_Y_2
  ld b,SANDBOX_ASTEROID_X_2
  call set_game_object_position
  call activate_game_object
  ;call draw_game_object

  ld ix,bullet
  ld iy,asteroid
  call detect_collision
  assertCarryReset "Test 2 failed"
ret
;
test_rect2_separate_from_rect1:
  ld ix,bullet
  ld hl,sandbox_bullet_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_BULLET_Y
  ld b,SANDBOX_BULLET_X_2
  call set_game_object_position
  call activate_game_object
  call draw_game_object

  ld ix,asteroid
  ld hl,sandbox_asteroid_setup_table
  call set_game_object_from_table
  ld a,SANDBOX_ASTEROID_Y
  ld b,SANDBOX_ASTEROID_X
  call set_game_object_position
  call activate_game_object
  call draw_game_object

  ld ix,bullet
  ld iy,asteroid
  call detect_collision
  assertCarryReset "Test 3 failed"
ret


rect1_init_data:
  .db 0, 0, 8, 8
rect2_init_data:
  .db 4, 0, 8, 8
rect1_init_data_2:
  .db 0, 0, 8, 8
rect2_init_data_2:
  .db 9, 0, 8, 8
rect1_init_data_3:
  .db 0, 0, 8, 8
rect2_init_data_3:
  .db 0, 9, 8, 8
rect1_init_data_4:
  .db 4, 4, 8, 8
rect2_init_data_4:
  .db 0, 0, 8, 8
test5_data:
  .db 0, 0, 8, 8
  .db 4, 0, 8, 8
test6_data:
  .db 0, 0, 8, 8
  .db 9, 0, 8, 8

  rect1 instanceof rect
  rect2 instanceof rect

  .struct rect
    x db
    y db
    width db
    height db
  .endst


  bullet_init_data:
    .db 0, 0
  asteroid_init_data:
    .db 4, 0
  bullet_init_data_2:
    .db 0, 0
  asteroid_init_data_2:
    .db 9, 0


    detect_collision:
      call coll1
      ret nc
      call coll2
      ret nc
      call coll3
      ret nc
      call coll4
      ret nc
      ; return with carry set.
    ret
      coll1:
        ; rect1.x < rect2.x + rect2.width
        ld a,(iy+0)
        ld b,(iy+2)
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
        ld a,(iy+0)
        sub b
      ret
      coll3:
        ; rect1.y < rect2.y + rect2.height
        ld a,(iy+1)
        ld b,(iy+3)
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
        ld a,(iy+1)
        cp b
      ret

    init_rect:
      ; ix = rect, HL = init data
      ld a,(hl)
      ld (ix+0),a
      inc hl
      ld a,(hl)
      ld (ix+1),a
      inc hl
      ld a,(hl)
      ld (ix+2),a
      inc hl
      ld a,(hl)
      ld (ix+3),a
      inc hl
    ret
    ;
    load_buffer:
      ; hl = pointer to buffer
      ld de,collision_buffer
      ld bc,8
      ldir
    ret
