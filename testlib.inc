; testlib.inc
; Macros, functions and data for test library.
;
; -----------------------------------------------------------------------------
; DEFINITIONS
; -----------------------------------------------------------------------------
.equ LAST_VISIBLE_ROW 23
.equ FIRST_COLUMN 0
;
; -----------------------------------------------------------------------------
;                         M A C R O S
; -----------------------------------------------------------------------------
; -----------------------------------------------------------------------------
.macro assertEqualsA ; (value, fail string)
; -----------------------------------------------------------------------------
  cp \1
  jp z,+
    log \2
  +:
.endm

; -----------------------------------------------------------------------------
.macro assertNotEqualsA ; (value, fail string)
; -----------------------------------------------------------------------------
  cp \1
  jp nz,+
  log \2
  +:
.endm

; -----------------------------------------------------------------------------
.macro assertEqualsHL ; (value, fail string)
; -----------------------------------------------------------------------------
  ld de,\1
  ld a,d
  cp h
  jp nz,+
  ld a,e
  cp l
  jp z,++
  +:
    log \2
  ++:
.endm

; -----------------------------------------------------------------------------
.macro assertEqualsByte ; (byte-sized variable, value, fail string)
; -----------------------------------------------------------------------------
  ld a,(\1)
  ld b,\2
  cp b
  jp z,+
    log \3
  +:
.endm

; -----------------------------------------------------------------------------
.macro assertEqualsWord ; (word-sized variable, value, fail string)
; -----------------------------------------------------------------------------
  ld hl,(\1)
  ld de,\2
  sbc hl,de
  jp z,+
    log \3
  +:
.endm


; -----------------------------------------------------------------------------
.macro assertCarrySet ; (fail string)
; -----------------------------------------------------------------------------
  jp c,+
    log \1
  +:
.endm

; -----------------------------------------------------------------------------
.macro assertCarryReset ; (fail string)
; -----------------------------------------------------------------------------
  jp nc,+
    log \1
  +:
.endm

; -----------------------------------------------------------------------------
.macro log
; -----------------------------------------------------------------------------
  ld a,(logger_current_row)   ; Get current row.
  cp LAST_VISIBLE_ROW+1       ; Is it outside the visible screen?
  jp nz,++                    ; If not then proceed to print message.
    call crash_logger         ; Else, print error message.
    jp logDataEnd\@           ; Skip to end of macro.
  ++:
  ld b,a
  ld c,FIRST_COLUMN
  ld hl,logData\@
  call print
  ld hl,logger_current_row
  inc (hl)
  jp logDataEnd\@
  logData\@:
    .asc "> "
    .asc \1
    .asc "#"
  logDataEnd\@:
.endm

; -----------------------------------------------------------------------------
.ramsection "testlib variables" slot 3
; -----------------------------------------------------------------------------
  logger_current_row db
  ;
.ends

; -----------------------------------------------------------------------------
.section "crash_logger" free
; -----------------------------------------------------------------------------
  ; Crash the logger.
  crash_logger:
    ld b,LAST_VISIBLE_ROW
    ld c,FIRST_COLUMN
    ld hl,logger_error_msg
    call print
  ret
  ;
  logger_error_msg:
    .asc "! Log overflow error#"
.ends
;
; -----------------------------------------------------------------------------
.section "reset_logger" free
; -----------------------------------------------------------------------------
  ; Reset logger.
  ; Entry: B = logger start row
  ; Exit: None.
  ; Uses: AF, BC, HL
  ;       logger_current_row
  reset_logger:
    ; Save initial row data.
    ld a,b
    inc a
    ld (logger_current_row),a
    ; Make divider.
    ld c,0
    ld hl,logger_reset_msg
    call print
  ret
  ;
  logger_reset_msg:
    .asc "--------------------------------#"
.ends
