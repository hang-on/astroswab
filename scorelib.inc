; scorelib.inc
; ....
; -----------------------------------------------------------------------------
; SCORE AND HISCORE
; -----------------------------------------------------------------------------
.equ SCORE_ONES 4
.equ SCORE_TENS 3
.equ SCORE_HUNDREDS 2
.equ SCORE_THOUSANDS 1
.equ SCORE_TEN_THOUANDS 0
.equ SCORE_DIGITS_TOTAL 5
.equ LETTERS_IN_INITIALS 3
.equ HISCORE_EXTRAM_ADDRESS EXTRAM_START+10 ; Where in EXTRAM to save table?
;
.struct score_struct
  ten_thousands db
  thousands db
  hundreds db
  tens db
  ones db
  initials dsb LETTERS_IN_INITIALS
.endst
;
.equ NUMBER_OF_HISCORE_ITEMS 10
.equ HISCORE_TABLE_ROW 7
.equ HISCORE_TABLE_COLUMN 10
;

.ramsection "score and hiscore handling - variables" slot 3
  hiscore_item instanceof score_struct NUMBER_OF_HISCORE_ITEMS
  hiscore_workbench instanceof score_struct
  hiscore_table_buffer dsb (_sizeof_score_struct*NUMBER_OF_HISCORE_ITEMS)
  hiscore_table_pointer dw
  hiscore_table_buffer_pointer dw
  ;
.ends
; -----------------------------------------------------------------------------
.section "score and hiscore handling - functions" free
  ;
  save_hiscore_table_to_extram:
    ;
    ;
    SELECT_EXTRAM
      ld hl,hiscore_item.1
      ld de,HISCORE_EXTRAM_ADDRESS
      ld bc,_sizeof_score_struct*NUMBER_OF_HISCORE_ITEMS
      ldir
    SELECT_ROM
  ret
  ;
  load_hiscore_table_from_extram:
    ;
    ;
    SELECT_EXTRAM
      ld hl,HISCORE_EXTRAM_ADDRESS
      ld de,hiscore_item.1
      ld bc,_sizeof_score_struct*NUMBER_OF_HISCORE_ITEMS
      ldir
    SELECT_ROM
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
    ; other digits in the score struct as necessary. Prevents score from going
    ; below zero.
    ; Entry:  A = Digit to subtract from.
    ;         B = Number to subtract (non-ascii!).
    ;        HL = Pointer to score struct.
    ; Exit:
    ; Uses: AF, BC, DE, HL
    ;
    push hl               ; Save ten thousands for underflow test.
      ld d,0
      ld e,score_struct.ten_thousands
      add hl,de
      ld c,(hl)
    pop hl
    push hl               ; Save pointer to score struct for underflow test.
      ld d,0
      add a,score_struct.ten_thousands
      ld e,a
      add hl,de
      ld a,(hl)
      sub b
      ld (hl),a
      cp ASCII_ZERO
      jp nc,+
        add a,10
        ld (hl),a
        -:
          dec hl
          dec (hl)
          ld a,(hl)
          cp ASCII_ZERO
          jp nc,+
            add a,10
            ld (hl),a
        jp -
      +:
      pop hl                  ; Get the score struct pointer back.
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
    reset_score_data:
      .rept SCORE_DIGITS_TOTAL
        .asc "0"
      .endr
  ;
  print_hiscore_table:
    ; Print the hiscore table starting at a specified (row,column).
    ; Entry: A = Row (vertical).
    ;        B = Column (horizontal).
    ; Exit: None.
    ; Uses: AF, BC, DE, HL, IX
    ;
    ld ix,hiscore_item.1
    ld c,NUMBER_OF_HISCORE_ITEMS
    ld de,_sizeof_score_struct
    -:
      call print_one_item
      inc a
      add ix,de
      ;
      dec c
    jp nz,-
  ret
    ;
    print_one_item:
      SAVE_REGISTERS
      push bc
        call set_cursor
        push ix
        pop hl
        ld b,SCORE_DIGITS_TOTAL
        call put_string_at_cursor
      pop bc
      push af
        ld a,SCORE_DIGITS_TOTAL
        add a,b
        ld b,a
      pop af
      push bc
        call set_cursor
        ld hl,hiscore_item_spacer
        ld b,HISCORE_ITEM_SPACER_SIZE
        call put_string_at_cursor
      pop bc
      push af
        ld a,HISCORE_ITEM_SPACER_SIZE
        add a,b
        ld b,a
      pop af
      call set_cursor
      push ix
      pop hl
      ld de,score_struct.initials
      add hl,de
      ld b,LETTERS_IN_INITIALS
      call put_string_at_cursor
      RESTORE_REGISTERS
      ret
      ;
    .equ HISCORE_ITEM_SPACER_SIZE 5
    hiscore_item_spacer:
      .asc "....."
  ;
  rebuild_hiscore_table:
    ; Rebuilds the hiscore table against a new score item pointed to by HL.
    ; If the new score is high enough to be somewhere on the hiscore list,
    ; then it is copied into the hiscore table and all subsequent items are
    ; moved one position down. The last item on the list is destroyed.
    ; If the new score is not at least equal to the last item on the hiscore
    ; list, then the hiscore table stays unchanged.
    ; Entry: HL = Pointer to new score to build table against
    ; Exit: None.
    ; Uses: All registers!
    ; Copy player score into the workbench for testing.
    ld de,hiscore_workbench
    call copy_score_and_increment_pointers
    ld hl,hiscore_item.1
    ; Point to start of hiscore table and hiscore table buffer.
    ld (hiscore_table_pointer),hl
    ld hl,hiscore_table_buffer
    ld (hiscore_table_buffer_pointer),hl
    ; For each hiscore table item, do...
    ld b,NUMBER_OF_HISCORE_ITEMS
    -:
      ; Is score at hiscore table pointer higher than score at workbench?
      ld hl,(hiscore_table_pointer)
      push hl
      pop ix                          ; Put hiscore table pointer in IX
      ld iy,hiscore_workbench         ; Point IY to score at workbench.
      call compare_scores
      jp c,+
        ; Yes - score in IX (item in hiscore table) is higher.
        ; Copy score item from hiscore table to buffer.
        ld hl,(hiscore_table_buffer_pointer)  ; Load hiscore table buffer into
        ex de,hl                              ; destination position (DE).
        push ix                               ; Load hiscore item into source
        pop hl                                ; position (HL).
        call copy_score_and_increment_pointers
        jp ++
      +:
        ; No - score in IY (item at workbench) is higher or even.
        ; Copy score item from workbench to buffer.
        ld hl,(hiscore_table_buffer_pointer)  ; Load hiscore table buffer into
        ex de,hl                              ; destination position (DE).
        ld hl,hiscore_workbench
        call copy_score_and_increment_pointers
        ld hl,(hiscore_table_pointer)
        ld de,hiscore_workbench
        call copy_score_and_increment_pointers
      ++:
        ; Increment pointers.
        ld de,_sizeof_score_struct
        ld hl,(hiscore_table_pointer)
        add hl,de
        ld (hiscore_table_pointer),hl         ; Inc hiscore table pointer.
        ld hl,(hiscore_table_buffer_pointer)
        add hl,de
        ld (hiscore_table_buffer_pointer),hl  ; Inc hiscore table buffer ptr.
    djnz -
    ; Overwrite the hiscore table with the buffer contents.
    ld hl,hiscore_table_buffer
    ld de,hiscore_item.1
    ld bc,_sizeof_score_struct*NUMBER_OF_HISCORE_ITEMS
    ldir
  ret
  ;
  compare_scores:
    ; Compare two score items to each other, passed to this func in IX and IY.
    ; If score in IY is equal or higher then score in IX, then set carry. If
    ; not, then reset carry.
    ; Entry: IX, IY = Pointers to score structs to compare.
    ; Uses: AF
    ld a,(ix+score_struct.ten_thousands)
    cp (iy+score_struct.ten_thousands)
    jp c,iy_is_equal_or_higher
    jp z,+
    jp ix_is_higher
    +:
      ld a,(ix+score_struct.thousands)
      cp (iy+score_struct.thousands)
      jp c,iy_is_equal_or_higher
      jp z,+
      jp ix_is_higher
      +:
        ld a,(ix+score_struct.hundreds)
        cp (iy+score_struct.hundreds)
        jp c,iy_is_equal_or_higher
        jp z,+
        jp ix_is_higher
        +:
          ld a,(ix+score_struct.tens)
          cp (iy+score_struct.tens)
          jp c,iy_is_equal_or_higher
          jp z,+
          jp ix_is_higher
          +:
            ld a,(ix+score_struct.ones)
            cp (iy+score_struct.ones)
            jp c,iy_is_equal_or_higher
            jp z,iy_is_equal_or_higher
            jp ix_is_higher
            ;
    iy_is_equal_or_higher:
      scf
      ret
    ix_is_higher:
      or a
      ret
  ret
  ;
  copy_score_and_increment_pointers:
    ; Copy the contents of one score struct to another.
    ; Entry: Two score struct pointers:
    ;        HL = Source score
    ;        DE = Destination score.
    ; Exit: Increment HL and DE by the size of one score struct.
    ; Uses: None
    push bc
      ld bc,_sizeof_score_struct
      ldir
    pop bc
  ret
  ;
  copy_hiscore_table:
    ; Copy a table of score items (at the size of the hiscore table) to another
    ; table. It can be used for initializing the hiscore table with ROM data,
    ; or when using the hiscore table buffer to rebuild the hiscore table.
    ; Entry: HL = Source (Pointer to first item in table).
    ;        DE = Destination (Pointer to first item in table).
    ; Uses: None
    SAVE_REGISTERS
    ld b,NUMBER_OF_HISCORE_ITEMS
      -:
        push bc
          ld bc,_sizeof_score_struct
          call copy_score_and_increment_pointers
        pop bc
      djnz -
    RESTORE_REGISTERS
  ret
.ends
