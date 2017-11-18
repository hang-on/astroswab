; triggerlib.inc
; .
; -----------------------------------------------------------------------------
; TRIGGERS
; -----------------------------------------------------------------------------
; Triggers have....
;
.struct trigger
  timer db
  interval db
  chance db
  state db
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
    ld a,(ix+trigger.state)
    cp DISABLED
    ret z
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
        jp nz,+
          scf                           ; Set carry flag if roll = chance!
          ret
      +:
        jp nc,+                         ;
          scf                           ; Set carry flag if roll < chance!
          ret
      +:
      or a                              ; Clear carry flag.
    ret
.ends
