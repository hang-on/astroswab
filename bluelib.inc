; bluelib.inc - GG/SMS project boilerplate and standard library.
; Latest revision: Spring 2017 for project astroswab.

; *****************************************************************************
;                              BASE LIBRARY
; *****************************************************************************
.equ ROMSIZE 256                      ; Make a 128k or 256k rom.
.equ INCLUDE_VDP_INTERRUPT_HANDLER    ; Comment out to exclude it.
.equ INCLUDE_PAUSE_BUTTON_HANDLER     ; Comment out to exclude it.
; -----------------------------------------------------------------------------
; HARDWARE DEFINITIONS
; -----------------------------------------------------------------------------
; Video memory and initialization:
.equ NAME_TABLE_START $3800
.equ VISIBLE_NAME_TABLE_SIZE 2*32*24
.equ FULL_NAME_TABLE_SIZE 2*32*28
.equ SPRITE_BANK_START $2000
.equ BACKGROUND_BANK_START $0000
.equ SPRITE_PALETTE_START 16
.equ BACKGROUND_PALETTE_START 0
.equ SAT_Y_START $3f00
.equ SAT_XC_START SAT_Y_START+64+64
.equ SPRITE_TERMINATOR $d0
.equ NTSC 0
.equ PAL 1
.equ FIRST_LINE_OF_VBLANK 192
;
.equ REGISTER_0 0
.equ REGISTER_1 1
.equ REGISTER_2 2
.equ REGISTER_3 3
.equ REGISTER_4 4
.equ REGISTER_5 5
.equ REGISTER_6 6
.equ REGISTER_7 7
.equ REGISTER_8 8
.equ REGISTER_9 9
.equ REGISTER_10 10
.equ REGISTER_11 11
;
; Memory map and initialization:
.equ RAM_START $c000
.equ STACK_INIT_ADDRESS $dff0

; Port communication and control:
.equ SYSTEM_CONTROL_PORT $00          ; GG: start, region, ntsc/pal.
.equ START_BUTTON_BIT %10000000       ; 0 = Switch is on!
.equ INPUT_PORT_1 $dc
.equ INPUT_PORT_2 $dd
.equ PSG_PORT $7f
.equ V_COUNTER_PORT $7e
.equ CONTROL_PORT $BF
.equ DATA_PORT $BE
.equ VRAM_WRITE_COMMAND %01000000
.equ VRAM_READ_COMMAND %00000000
.equ REGISTER_WRITE_COMMAND %10000000
.equ CRAM_WRITE_COMMAND %11000000

; --- NB! Developed for GG -------------------
; Register 0 settings:
.equ SCROLL_0_LINE_0_SPRITES_0 %00000110
; Register 1 settings:
.equ DISPLAY_1_FRAME_1_SIZE_0 %11100000
.equ DISPLAY_0_FRAME_1_SIZE_0 %10100000
.equ DISPLAY_0_FRAME_0_SIZE_0 %10000000
.equ DISPLAY_1_FRAME_1_SIZE_0_ZOOM %11100001
; ---------------------------------------------
.equ SCROLL_0__LCB_1_LINE_0_SPRITES_0 %00100110
.equ SCROLL_0__LCB_0_LINE_0_SPRITES_0 %00000110

;
.equ HORIZONTAL_SCROLL_REGISTER 8
.equ VERTICAL_SCROLL_REGISTER 9
.equ RASTER_INTERRUPT_REGISTER 10

; Banks / ROM / External RAM control:
.equ SET_EXTRAM_BIT %00001000
.equ RESET_EXTRAM_BIT %11110111
.equ EXTRAM_START $8000
.equ EXTRAM_SIZE $4000
.equ SLOT_2_CONTROL $ffff
.equ BANK_CONTROL $fffc

; CRT screen values:
.equ CRT_LEFT_BORDER 0
.equ CRT_RIGHT_BORDER 255
.equ CRT_TOP_BORDER 0
.equ CRT_BOTTOM_BORDER 191

; LCD screen values:
.equ LCD_RIGHT_BORDER (6*8)+(20*8)
.equ LCD_LEFT_BORDER 6*8
.equ LCD_TOP_BORDER 3*8
.equ LCD_BOTTOM_BORDER (3*8)+(18*8)
.equ LCD_WIDTH 20*8
.equ LCD_HEIGHT 18*8

; Invisible area:
.equ INVISIBLE_AREA_TOP_BORDER 192
.equ INVISIBLE_AREA_BOTTOM_BORDER 224
; -----------------------------------------------------------------------------
; SOFTWARE DEFINITIONS
; -----------------------------------------------------------------------------
.equ TRUE $ff
.equ FALSE 0
.equ ENABLED $ff
.equ DISABLED 0
.equ FLAG_SET $ff
.equ FLAG_RESET $00
.equ PAUSE_FLAG_RESET $00
.equ PAUSE_FLAG_SET $ff
.equ UNUSED_BYTE $0
.equ UNUSED_WORD $0000
.equ PERCENT_CHANCE_50 128
.equ PERCENT_CHANCE_100 255
;
.equ BYTES_PER_TILE 32
.equ ONE_TILEMAP_ROW 32*2
.equ END_OF_TABLE $ff

; Font / printing support.
.equ CHARS_IN_ASCII_SET 91
.equ STRING_TERMINATOR 3        ; Terminate strings with #
.equ ASCII_ZERO 16        ; Where in the tilebank is the ASCII zero?
.equ ASCII_ONE 17
.equ ASCII_NINE ASCII_ZERO+9
.equ ASCII_SPACE 0
.equ ASCII_EXCLAMATION 1
.equ ASCII_TABLE_START 0
.asciitable
  map " " to "z" = ASCII_TABLE_START
.enda
; =============================================================================
; M A C R O S
; =============================================================================
; -----------------------------------------------------------------------------
.macro SELECT_EXTRAM
; -----------------------------------------------------------------------------
  ; Select external RAM: Now memory addresses from $8000 - $c000 (slot 2)
  ; are mapped to the battery-backed RAM, and thus its contents are saved
  ; between sessions.
  push af
  ld a,(BANK_CONTROL)
  or SET_EXTRAM_BIT
  ld (BANK_CONTROL),a
  pop af
.endm
; -----------------------------------------------------------------------------
.macro SELECT_ROM
; -----------------------------------------------------------------------------
  ; Select ROM: Used to switch mapping in slot 2 ($8000 - $c000) back to ROM
  ; if external RAM was selected.
  push af
  ld a,(BANK_CONTROL)
  and RESET_EXTRAM_BIT
  ld (BANK_CONTROL),a
  pop af
.endm
; -----------------------------------------------------------------------------
.macro SELECT_BANK
; -----------------------------------------------------------------------------
  ; Select a bank for slot 2, i.e. SELECT_BANK 4.
  push af
  ld a,\1
  ld (SLOT_2_CONTROL),a
  pop af
.endm
; -----------------------------------------------------------------------------
.macro SELECT_BANK_IN_REGISTER_A
; -----------------------------------------------------------------------------
  ; Select a bank for slot 2, - put value in register A.
  ld (SLOT_2_CONTROL),a
.endm
; -----------------------------------------------------------------------------
.macro SAVE_REGISTERS
; -----------------------------------------------------------------------------
  ; Save all registers, except IX and IY
  push af
  push bc
  push de
  push hl
  push ix
  push iy
.endm
; -----------------------------------------------------------------------------
.macro RESTORE_REGISTERS
; -----------------------------------------------------------------------------
  ; Restore all registers, except IX and IY
  pop iy
  pop ix
  pop hl
  pop de
  pop bc
  pop af
.endm

; -----------------------------------------------------------------------------
.memorymap
; -----------------------------------------------------------------------------
  defaultslot 0
  slotsize $4000
  slot 0 $0000
  slot 1 $4000
  slot 2 $8000
  slotsize $2000
  slot 3 RAM_START
.endme
.if ROMSIZE == 128
  .rombankmap ; 128K rom
    bankstotal 8
    banksize $4000
    banks 8
  .endro
.endif
.if ROMSIZE == 256
  .rombankmap ; 256K rom
    bankstotal 16
    banksize $4000
    banks 16
  .endro
.endif
; -----------------------------------------------------------------------------
.ramsection "bluelib global variables" slot 3
; -----------------------------------------------------------------------------
  VDPStatus db
  pause_flag db
  input_ports dw
  tv_type db
  ;
.ends

.org 0
.bank 0 slot 0
; -----------------------------------------------------------------------------
.section "Boot" force
; -----------------------------------------------------------------------------
  boot:
  di
  im 1
  ld sp,STACK_INIT_ADDRESS

  ; Initialize the standard Sega mapper.
  ld de,$fffc
  ld hl,_MapperInitData
  ld bc,$0004
  ldir

  call ClearRam ; Except the top (stack, slot/bank registers, etc.)
  call ClearVRam

  call detect_tv_type ; Beware! Messes with interrupts and vdp-registers.
  ld (tv_type),a

  call InitializeVDPRegisters

  jp init

  _MapperInitData:
    .db $00,$00,$01,$02
.ends

.ifdef INCLUDE_VDP_INTERRUPT_HANDLER
  .org $0038
  ; ---------------------------------------------------------------------------
  .section "!VDP interrupt" force
  ; ---------------------------------------------------------------------------
    push af
      in a,CONTROL_PORT
      ld (VDPStatus),a
    pop af
    ei
    reti
  .ends
.endif

.ifdef INCLUDE_PAUSE_BUTTON_HANDLER
  .org $0066
  ; ---------------------------------------------------------------------------
  .section "!Pause interrupt" force
  ; ---------------------------------------------------------------------------
    push af
      ld a,(pause_flag)
      cpl
      ld (pause_flag),a
    pop af
    retn
  .ends
.endif


; =============================================================================
; H E L P E R  F U N C T I O N S                        (sorted alphabetically)
; =============================================================================

; -----------------------------------------------------------------------------
.section "await_frame_interrupt" free
; -----------------------------------------------------------------------------
  ; Await frame interrupt.
  ; Repeatedly test bit 7 of VDPStatus until it is set (by the VDP interrupt
  ; handler). Then reset this bit and return.
  ; Entry: None
  ; Exit: Bit 7 of variable VDPStatus is cleared.
  ; Uses: VDPStatus (1 byte)
  await_frame_interrupt:
    push af
      -:
        ld a,(VDPStatus)
        bit 7,a
      jp z,-
      in a,(V_COUNTER_PORT)
      ; Check this post: http://www.smspower.org/forums/8113-WaitForVBlankSlowerOnNTSCThanPALSystematicProblemWithWaitForVBlankNoInt
      cp FIRST_LINE_OF_VBLANK+1
      jp nz,-
        res 7,a
        ld (VDPStatus),a
    pop af
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "hl_ (call hl)" free
; -----------------------------------------------------------------------------
  hl_: ; "call _hl"
    ; Jump to where HL is pointing to. Assume it is a handler that ends in
    ; ret.
    jp (hl)
.ends
;
; -----------------------------------------------------------------------------
.section "clear_extram" free
; -----------------------------------------------------------------------------
  ; Clear external ram by writing zeroes to all bytes.
  ; Uses AF, BC, HL
  clear_extram:
    SELECT_EXTRAM
    ld bc,EXTRAM_SIZE               ; Every byte in external ram.
    ld hl,EXTRAM_START              ; Begin from the first byte.
    -:
      xor a                         ; Write zeroes over all external ram bytes.
      ld (hl),a
      inc hl
      dec bc
      ld a,b
      or c
    jp nz,-
    SELECT_ROM
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "ClearRam" free
; -----------------------------------------------------------------------------
  ; Overwrite ram with zeroes from RAM_START to STACK_INIT_ADDRESS - 16
  ; Uses AF, BC, DE, HL
  ClearRam:
    ld hl,RAM_START
    ld bc,STACK_INIT_ADDRESS-16-RAM_START
    ld a,0
    call _FillMemory
  ret

  _FillMemory:
    ld (hl),a
    ld d,h
    ld e,l
    inc de
    dec bc
    ld a,b
    or c
    ret z
    ldir
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "ClearVRam" free
; -----------------------------------------------------------------------------
  ; Write 00 to all vram positions. Assumes disabled display.
  ; Uses AF, BC
  ClearVRam:
    xor a
    out (CONTROL_PORT),a
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    ld bc,$4000 ; 16K
    -:
      xor a
      out (DATA_PORT),a
      dec bc
      ld a,b
      or c
    jp nz,-
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "convert_byte" free
; -----------------------------------------------------------------------------
  convert_byte:
    ; Convert a byte passed in A to a value from a specified conversion table.
    ; Entry: A = Byte-sized value to convert.
    ;        B = Values in conversion table (failsafe).
    ;       HL = Ptr to conversion table (value, converted value, value etc..)
    ; Exit:  A = result of conversion (if matching value found in table).
    ;       Carry flag set if no matching value found in table.
    ; Uses: AF, BC, HL
    -:
      ld c,(hl)
      cp c
      inc hl
      jp nz,+
        ld a,(hl)
        ret
      +:
      inc hl
    djnz -
    scf
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "cp_word" free
; -----------------------------------------------------------------------------
  cp_word:
    ; Compare a word-sized variable at HL to a word-sized value in DE.
    ; Entry: HL = Pointer to variable.
    ;        BC = Value to compare.
    ; Exit: Zero flag is set/reset.
    ;
    ld a,(hl)
    inc hl
    ld h,(hl)
    ld l,a
    sbc hl,bc
  ret
.ends
; -----------------------------------------------------------------------------
.section "detect_tv_type" free
; -----------------------------------------------------------------------------
  detect_tv_type:
    ; Returns a=0 for NTSC, a=1 for PAL
    ; Uses: a, hl, de
    ; From SMS-Power!
    di             ; disable interrupts
    ld a,%01100000 ; set VDP such that the screen is on
    out ($bf),a    ; with VBlank interrupts enabled
    ld a,$81
    out ($bf),a
    ld hl,$0000    ; init counter
-:  in a,($bf)     ; get VDP status
    or a           ; inspect
    jp p,-         ; loop until frame interrupt flag is set
-:  in a,($bf)     ; do the same again, in case we were unlucky and came in just
    or a           ;   before the start of the VBlank with the flag already set
    jp p,-
    ; the VDP must now be at the start of the VBlank
-:  inc hl         ; (6 cycles) increment counter until interrupt flag comes on again
    in a,($bf)     ; (11 cycles)
    or a           ; (4 cycles)
    jp p,-         ; (10 cycles)
    xor a          ; reset carry flag, also set a=0
    ld de,2048     ; see if hl is more or less than 2048
    sbc hl,de
    ret c          ; if less, return a=0
    ld a,1
    ret            ; if more or equal, return a=1
.ends
;
; -----------------------------------------------------------------------------
.section "fast_put_char" free
; -----------------------------------------------------------------------------
  ; Put one character on the nametable with minimum overhead.
  ; Entry: HL = Pointer to 4-byte table with char data, in this format:
  ;             Address (word), char (byte), flags (byte)
  ; Exit: None
  ; Uses: AF, HL
  .equ FAST_PUT_CHAR_OFFSET 2 ; Used to alter just the char being put.
  fast_put_char:
    ld a,(hl)
    out (CONTROL_PORT),a
    inc hl
    ld a,(hl)
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    inc hl
    ld a,(hl)
    out (DATA_PORT),a
    inc hl
    ld a,(hl)
    out (DATA_PORT),a             ; Write 2nd byte to name table.
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "get_absolute_value" free
; -----------------------------------------------------------------------------
  ;
  get_absolute_value:
    ;
    bit 7,a
    ret z
      neg
      ret
.ends
;
; -----------------------------------------------------------------------------
.section "get_byte_from_table" free
; -----------------------------------------------------------------------------
  get_byte_from_table:
    ; Retrieve a byte item from a table of bytes.
    ; Entry: A = Index.
    ;        HL = Base address of byte table (256 bytes).
    ; Exit: A = Byte item.
    ; Uses: None.
    push hl
    push de
      ;add a,a
      ld d,0
      ld e,a
      add hl,de
      ld a,(hl)
      ;inc hl
      ;ld h,(hl)
      ;ld l,a
    pop de
    pop hl
  ret
;
.ends
;
; -----------------------------------------------------------------------------
.section "get_input_ports" free
; -----------------------------------------------------------------------------
  ; Set input_ports (word) to mirror current state of ports $dc and $dd.
  ; Uses: AF
  get_input_ports:
    in a,(INPUT_PORT_1)
    ld (input_ports),a
    in a,(INPUT_PORT_2)
    ld (input_ports+1),a
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "get_word_from_table" free
; -----------------------------------------------------------------------------
  get_word_from_table:
    ; Retrieve a word item from a table of words.
    ; Entry: A = Index.
    ;        HL = Base address of word table. (128 words in table)
    ; Exit: HL = Word.
    ; Uses: AF, DE, HL.
    add a,a
    ld d,0
    ld e,a
    add hl,de
    ld a,(hl)
    inc hl
    ld h,(hl)
    ld l,a
  ret
;
.ends
; -----------------------------------------------------------------------------
.section "InitializeVDPRegisters" free
; -----------------------------------------------------------------------------
; Initialize the 11 VDP registers with the preset data block.
; Uses: AF, B, HL
  _VDPRegisterInitData:
    .db SCROLL_0_LINE_0_SPRITES_0
    .db DISPLAY_0_FRAME_0_SIZE_0
    .db $ff,$ff,$ff,$ff,$ff,$00,$00,$00,$ff

  InitializeVDPRegisters:
    ld hl,_VDPRegisterInitData
    xor b
    -:
      ld a,(hl)
      out (CONTROL_PORT),a
      inc hl
      ld a,b
      or REGISTER_WRITE_COMMAND
      out (CONTROL_PORT),a
      cp REGISTER_WRITE_COMMAND|10
      ret z
      inc b
    jr -
.ends
;
; -----------------------------------------------------------------------------
.section "is_dpad_pressed" free
; -----------------------------------------------------------------------------
  is_dpad_pressed:
    ld a,(input_ports)
    and %00001111   ; Isolate the dpad bits.
    cpl             ; Invert bits; now 1 = keypress!
    and %00001111   ; Get rid of garbage from cpl in last four bits.
    cp 0            ; Now, is any dpad key preseed?
    ret z           ; No, then return with carry flag reset (by the AND).
    scf             ; Yes, then set carry flag and...
  ret               ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "is_button_1_pressed" free
; -----------------------------------------------------------------------------
  is_button_1_pressed:
    ld a,(input_ports)
    and %00010000
    ret nz            ; Return with carry flag reset
    scf
  ret                 ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "is_player_2_button_1_pressed" free
; -----------------------------------------------------------------------------
  is_player_2_button_1_pressed:
    ld a,(input_ports+1)
    and %00000100
    ret nz            ; Return with carry flag reset
    scf
  ret                 ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "is_button_2_pressed" free
; -----------------------------------------------------------------------------
  is_button_2_pressed:
    ld a,(input_ports)
    and %00100000
    ret nz          ; Return with carry flag reset
    scf
  ret               ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "is_player_2_button_2_pressed" free
; -----------------------------------------------------------------------------
  is_player_2_button_2_pressed:
    ld a,(input_ports+1)
    and %00001000
    ret nz            ; Return with carry flag reset
    scf
  ret                 ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "is_down_pressed" free
; -----------------------------------------------------------------------------
  is_down_pressed:
    ld a,(input_ports)
    and %00000010
    ret nz          ; Return with carry flag reset
    scf
  ret               ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "is_left_pressed" free
; -----------------------------------------------------------------------------
  is_left_pressed:
    ld a,(input_ports)
    and %00000100
    ret nz          ; Return with carry flag reset
    scf
  ret               ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "is_reset_pressed" free
; -----------------------------------------------------------------------------
  is_reset_pressed:
    ld a,(input_ports+1)
    and %00010000
    ret nz          ; Return with carry flag reset
    scf
  ret               ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "is_right_pressed" free
; -----------------------------------------------------------------------------
  is_right_pressed:
    ld a,(input_ports)
    and %00001000
    ret nz          ; Return with carry flag reset
    scf
  ret               ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "is_up_pressed" free
; -----------------------------------------------------------------------------
  is_up_pressed:
    ld a,(input_ports)
    and %00000001
    ret nz          ; Return with carry flag reset
    scf
  ret               ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "is_start_pressed" free
; -----------------------------------------------------------------------------
  is_start_pressed:
    in a,(SYSTEM_CONTROL_PORT)
    and START_BUTTON_BIT
    ret nz          ; Return with carry flag reset
    scf
  ret               ; Return with carry flag set.
.ends
;
; -----------------------------------------------------------------------------
.section "kill_psg" free
; -----------------------------------------------------------------------------
  ; Manually silence all sound.
  ; Entry: None.
  ; Saves registers used.
  kill_psg:
    push af
    ld a,$9f
    out (PSG_PORT),a
    ld a,$bf
    out (PSG_PORT),a
    ld a,$df
    out (PSG_PORT),a
    pop af
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "load_cram" free
; -----------------------------------------------------------------------------
  ; Load a number of colors into color ram (SMS).
  ; Entry: A = Palette index (0-31)
  ;        B = Number of colors to load
  ;        HL = Base address of source data (color bytes, SMS)
  ; Uses: AF, BC, HL
  ; Assumes blanked display and interrupts off.
  load_cram:
    out (CONTROL_PORT),a
    ld a,CRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    -:
      ld a,(hl)
      out (DATA_PORT),a
      inc hl
    djnz -
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "load_cram_gg" free
; -----------------------------------------------------------------------------
  ; Load a number of colors into color ram (GG).
  ; Entry: A = Palette index (0-31)
  ;        B = Number of colors to load
  ;        HL = Base address of source data (color words, GG)
  ; Uses: AF, BC, HL
  ; Assumes blanked display and interrupts off.
  load_cram_gg:
    sla a
    out (CONTROL_PORT),a
    ld a,CRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    -:
      ld a,(hl)
      out (DATA_PORT),a
      inc hl
      ld a,(hl)
      out (DATA_PORT),a
      inc hl
    djnz -
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "load_vdp_registers" free
; -----------------------------------------------------------------------------
  load_vdp_registers:
    ; Load all 11 vdp registers with preset values.
    ; Entry: HL pointing to init data block (11 bytes).
    xor b
    -:
      ld a,(hl)
      out (CONTROL_PORT),a
      inc hl
      ld a,b
      or REGISTER_WRITE_COMMAND
      out (CONTROL_PORT),a
      cp REGISTER_WRITE_COMMAND|10
      ret z
      inc b
    jr -
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "load_vram" free
; -----------------------------------------------------------------------------
  ; Load a number of bytes from a source address into vram.
  ; Entry: BC = Number of bytes to load
  ;        DE = Destination address in vram
  ;        HL = Source address
  ; Exit:  DE = Next free byte in vram.
  ; Uses: AF, BC, DE, HL,
  load_vram:
    ld a,e
    out (CONTROL_PORT),a
    ld a,d
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    -:
      ld a,(hl)
      out (DATA_PORT),a
      inc hl
      dec bc
      ld a,c
      or b
    jp nz,-
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "load_vram_from_table" free
; -----------------------------------------------------------------------------
  ; Load a number of bytes from a source address into vram.
  ; Entry: HL = Pointer to table, w. data in the following format:
  ;        Destination address in vram (word).
  ;        Amount of bytes (word).
  ;        Source address (word).
  ; Exit:  HL = Next item in table.
  ; Uses: AF, BC, DE, HL,
  load_vram_from_table:
    push hl
    ld a,(hl)
    out (CONTROL_PORT),a
    inc hl
    ld a,(hl)
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    inc hl
    ld c,(hl)
    inc hl
    ld b,(hl)
    inc hl
    ld a,(hl)
    inc hl
    ld h,(hl)
    ld l,a
    -:
      ld a,(hl)
      out (DATA_PORT),a
      inc hl
      dec bc
      ld a,c
      or b
    jp nz,-
    ld de,6
    pop hl
    add hl,de
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "put_string" free
; -----------------------------------------------------------------------------
  ; Put a string of multiple characters on the nametable with minimum overhead.
  ; Entry: B = Number og characters in string.
  ;        HL = Pointer to string: Nametable address (word), then pairs of
  ;             bytes [char], [flags]
  ; Exit: None
  ; Uses: AF, B, HL
  put_string:
    ld a,(hl)
    out (CONTROL_PORT),a
    inc hl
    ld a,(hl)
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    -:
      inc hl
      ld a,(hl)
      out (DATA_PORT),a
      inc hl
      ld a,(hl)
      out (DATA_PORT),a             ; Write 2nd byte to name table.
    djnz -
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "ReadVRam" free
; -----------------------------------------------------------------------------
  ; Read a number of bytes from vram into a buffer.
  ; Entry: BC = Number of bytes to read
  ;        DE = Destination address in ram (buffer)
  ;        HL = Source address in vram
  ; Uses: AF, BC, DE, HL,
  ReadVRam:
    ld a,l
    out (CONTROL_PORT),a
    ld a,h
    or VRAM_READ_COMMAND
    out (CONTROL_PORT),a
    -:
      in a,(DATA_PORT)
      ld (de),a
      inc de
      dec bc
      ld a,c
      or b
    jp nz,-
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "Set register (vdp)" free
; -----------------------------------------------------------------------------
  ; Write to target register.
  ; Entry: A = byte to be loaded into vdp register.
  ;        B = target register 0-10.
  ; Uses: AF, B
  set_register:
    out (CONTROL_PORT),a
    ld a,REGISTER_WRITE_COMMAND
    or b
    out (CONTROL_PORT),a
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "set_vram_address" free
; -----------------------------------------------------------------------------
  ; Set vram address.
  ; Entry: HL = Address.
  ; Exit:
  ; Uses:
  set_vram_address:
    ld a,l
    out (CONTROL_PORT),a
    ld a,h
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
  ret
.ends

; -----------------------------------------------------------------------------
.section "Wait for scanline" free
; -----------------------------------------------------------------------------
  ; Wait for a specific scanline passed in A.
  ; Entry: A = Scanline to wait for.
  ; Uses: AF, B
  wait_for_scanline:
    ld b,a
    -:
      in a,(V_COUNTER_PORT)
      cp b
    jp nz,-
  ret
.ends
;
; *****************************************************************************
; B L U E L I B  S P R I T E  R O U T I N E S
; *****************************************************************************
;
.equ PLAYER_SIZE 4          ; Number of tiles not part of asc/desc flicker.
.equ ASCENDING 0
.equ DESCENDING $ff
;
.ramsection "Spritelib Variables" slot 3
  SpriteBufferY dsb 64
  SpriteBufferXC dsb 128
  NextFreeSprite db
  SATLoadMode db             ; Ascending or descending - for flickering.
.ends
;
; -----------------------------------------------------------------------------
.section "add_sprite" free
; -----------------------------------------------------------------------------
  ; Add a sprite of size = 1 character to the SAT.
  ; Entry: A = Character code (character in bank 2).
  ;        B = vertical position, C = horizontal position.
  ; Exit:
  ; Uses: None - all registers saved
  add_sprite:
    ; Test for sprite overflow (more than 64 hardware sprites at once).
    SAVE_REGISTERS
    ld d,a                    ; Save the tile in unused register.
    ld a,(NextFreeSprite)
    inc a
    cp 65
    jp nc,exit_add_sprite
    ld a,d                    ; Restore tile in A.
    ;
    push af
    push bc
    ; Point DE to SpriteBufferY[NextFreeSprite].
    ld a,(NextFreeSprite)
    ld de,SpriteBufferY
    add a,e
    ld e,a
    ld a,0
    adc a,d
    ld d,a

    ; Retrieve Y and X coords.
    pop bc
    ld a,b
    ld (de),a               ; Write the Y to the sprite buffer.

    ; Point DE to SpriteBufferXC[NextFreeSprite].
    ld a,(NextFreeSprite)
    add a,a               ; Table elements are words!
    ld de,SpriteBufferXC
    add a,e
    ld e,a
    ld a,0
    adc a,d
    ld d,a
    ;
    ld a,c                ; Get the x-pos.
    ld (de),a             ; Write it to the buffer.
    inc de
    pop af                ; Retrieve the charcode.
    ld (de),a             ; Write it to the buffer

    ld hl,NextFreeSprite
    inc (hl)
    ;
    exit_add_sprite:
    RESTORE_REGISTERS
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "begin_sprites" free
; -----------------------------------------------------------------------------
  WipeData:
    .rept 64
      .db INVISIBLE_AREA_TOP_BORDER
    .endr

  begin_sprites:
    ; Reset the NextFreeSprite index at the beginning of every frame
    xor a
    ld (NextFreeSprite),a

    ; Toggle ascending/descending sat load mode.
    ld a,(SATLoadMode)
    cpl
    ld (SATLoadMode),a

    ; Wipe Y-buffer with $e0 to get unused sprites out of the way.
    ld hl,WipeData
    ld de,SpriteBufferY
    ld bc,64
    ldir
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "load_sat" free
; -----------------------------------------------------------------------------
  ; Load the vram sat with the SatY and SatXC buffers.
  ; Sonic 2 inspired flicker engine is in place: Flicker sprites by loading the
  ; SAT in ascending/descending order every other frame.
  load_sat:
    ld a,(SATLoadMode)
    cp DESCENDING
    jp z,_DescendingLoad
      ; If not descending, then fall through to ascending load mode.

      ; Load y-coordinates.
      ld hl,SAT_Y_START
      ld a,l
      out (CONTROL_PORT),a
      ld a,h
      or VRAM_WRITE_COMMAND
      out (CONTROL_PORT),a
      ld hl,SpriteBufferY
      ld c,DATA_PORT
      .rept 64
        outi
      .endr

      ; Load x-coordinates and character codes.
      ld hl,SAT_XC_START
      ld a,l
      out (CONTROL_PORT),a
      ld a,h
      or VRAM_WRITE_COMMAND
      out (CONTROL_PORT),a
      ld hl,SpriteBufferXC
      ld c,DATA_PORT
      .rept 128
        outi
      .endr
  ret

    _DescendingLoad:
      ; Load y-coordinates.
      ld hl,SAT_Y_START
      ld a,l
      out (CONTROL_PORT),a
      ld a,h
      or VRAM_WRITE_COMMAND
      out (CONTROL_PORT),a
      ld c,DATA_PORT
      ld hl,SpriteBufferY
      .rept PLAYER_SIZE
        outi
      .endr

      ld hl,SpriteBufferY+63    ; Point to last y-value in buffer.
      .rept 64-PLAYER_SIZE
        outd                    ; Output and decrement HL, thus going
      .endr                     ; backwards (descending) through the buffer.

      ; Load x-coordinates and character codes
      ld hl,SAT_XC_START
      ld a,l
      out (CONTROL_PORT),a
      ld a,h
      or VRAM_WRITE_COMMAND
      out (CONTROL_PORT),a
      ld c,DATA_PORT
      ld hl,SpriteBufferXC
      .rept PLAYER_SIZE
        outi
        outi
      .endr

      ld hl,SpriteBufferXC+126
      ld de,-4
      .rept 64-PLAYER_SIZE
        outi
        outi
        add hl,de
      .endr
  ret
.ends
; =============================================================================
; E X T E N D E D  F U N C T I O N S                        (grouped in themes)
; =============================================================================
; -----------------------------------------------------------------------------
.ramsection "extended functions variables" slot 3
; -----------------------------------------------------------------------------
  ;
.ends
;
; -----------------------------------------------------------------------------
.section "tilemap / name table editing" free
; -----------------------------------------------------------------------------
  copy_tilemap_rect_to_buffer:
    ; Copy the tilemap words under a rectangle to a buffer.
    ; A = Rect width.
    ; B = Rect height.
    ; HL = Top left tile (address) of rectangle to copy.
    ; DE = Destination buffer.
    ; All registers are preserved.
    SAVE_REGISTERS
    add a,a                 ; Double width - the table elements are words.
    ld c,a                  ; Save width in C for counting.
    ; For each 'rect-height' do the following...
    --:
      ld a,l
      out (CONTROL_PORT),a
      ld a,h
      or VRAM_READ_COMMAND
      out (CONTROL_PORT),a
      push de
        ld de,ONE_TILEMAP_ROW
        add hl,de
      pop de
      push bc               ; Save rect width*2.
      ; For each 'rect-width*2' do the following...
        -:
          in a,(DATA_PORT)  ; Get byte from tilemap.
          ld (de),a         ; Store it in the buffer.
          inc de            ; Update buffer pointer.
          dec c             ; Decrement 'for counter'.
          xor a             ; Do the rect-width*2 for-loop logic...
          cp c
        jp nz,-
      pop bc                ; Restore rect-width*2.
    djnz --                 ; The rect-height for-loop.
    RESTORE_REGISTERS
  ret
  ; ---------------------------------------------------------------------------
  copy_buffer_to_tilemap_rect:
    ; Reverse of copy_tilemap_rect_to_buffer.
    ; A = Rect width.
    ; B = Rect height.
    ; HL = Base address of buffer.
    ; DE = Address of top left tile of tilemap rectangle.
    ; Preserves all registers used.
    ; Time: 11 lines.
    SAVE_REGISTERS
    add a,a                 ; Double width - the table elements are words.
    ld c,a                  ; Save width in C for counting.
    ; For each 'rect-height' do the following...
    --:
      ld a,e
      out (CONTROL_PORT),a
      ld a,d
      or VRAM_WRITE_COMMAND
      out (CONTROL_PORT),a
      push hl
        ld hl,ONE_TILEMAP_ROW
        add hl,de
        ex de,hl
      pop hl
      push bc               ; Save rect width*2.
      ; For each 'rect-width*2' do the following...
        -:
          ld a,(hl)
          out (DATA_PORT),a
          inc hl
          dec c             ; Decrement 'for counter'.
          xor a             ; Do the rect-width*2 for-loop logic...
          cp c
        jp nz,-
      pop bc                ; Restore rect-width*2.
    djnz --                 ; The rect-height for-loop.
    RESTORE_REGISTERS
  ret
  ; ---------------------------------------------------------------------------
  blank_tilemap_rect:
    ; Write $0000 to the tilemap words under a rectangle.
    ; A = Rect width.
    ; B = Rect height.
    ; HL = Address of first tile to blank.
    add a,a                 ; Double width - the table elements are words.
    ld c,a                  ; Save width in C for counting.
    ; For each 'rect-height' do the following...
    --:
      ld a,l
      out (CONTROL_PORT),a
      ld a,h
      or VRAM_WRITE_COMMAND
      out (CONTROL_PORT),a
      ld de,ONE_TILEMAP_ROW
      add hl,de
      ld a,$00
      push bc               ; Save rect width*2.
      ; For each 'rect-width*2' do the following...
        -:
          out (DATA_PORT),a
          dec c             ; Decrement 'for counter'.
          cp c              ; A is already $00 = blanked.
        jp nz,-
      pop bc                ; Restore rect-width*2.
    djnz --                 ; The rect-height for-loop.
  ret

  reset_name_table:
    ; Overwrite all name table entries with a specified tile.
    ; Entry: A = Tile index
    ;        B = Bank (1/2) - selects corresponding palette.
    ; Uses: None.
    SAVE_REGISTERS
    push af
    dec b
    jp z,+
      ld b,%00001001
    +:
    ld hl,NAME_TABLE_START
    ld a,l
    out (CONTROL_PORT),a
    ld a,h
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    pop af
    ld h,a                                    ; Index of tile.
    ld l,b                                    ; 2nd byte, bank and pal.
    ld de,32*28                               ; 32 columns, 28 rows.
    -:
      ld a,h
      out (DATA_PORT),a                       ; Write 1st word to name table.
      ld a,l
      out (DATA_PORT),a                       ; Write 2nd word to name table.
      dec de
      ld a,e
      or d
    jp nz,-
    RESTORE_REGISTERS
  ret
.ends
;
; -----------------------------------------------------------------------------
.ramsection "bluelib printing variables" slot 3
; -----------------------------------------------------------------------------
  cursor_y db
  cursor_x db
.ends
; -----------------------------------------------------------------------------
.section "Printing" free
  ; Functions for printing ascii chars.
  ; Put this ascii map in header:
  ;   .asciitable
  ;      map " " to "z" = 0
  ;    .enda
  ; ---------------------------------------------------------------------------
  ; Assumes ASCII character set loaded into vram, like this:
  ;   SELECT_BANK FONT_BANK
  ;   ld hl,font_table
  ;   call load_vram_from_table
  ;   .... in footer:
  ;   font_table:
  ;  .dw BACKGROUND_BANK_START
  ;  .dw CHARS_IN_ASCII_SET*BYTES_PER_TILE
  ;  .dw dd_revolution_tiles
  ;   ....
  ;   dd_revolution_tiles:
  ;  .include "bank_2\dd_revolution_tiles.inc"
  ; ---------------------------------------------------------------------------
  batch_print:
    ; Batch print strings at different screen positions. Handy for making a
    ; menu, or for setting up text in various places. Strings and positions are
    ; stored in a table in the following format:
    ; pointer to string (word), row (byte), column (byte).
    ; Entry: IX = pointer to table.
    ;        A = number of strings to print.
    ; Uses: AF, BC, HL.
    ; Depends on print (in bluelib.inc), and on #-terminated strings.
    .equ SIZE_OF_BATCH_PRINT_ITEM 4
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
  ;
  print:
    ; Print a string to the name table. Terminate with # (ascii).
    ; N.B. Make sure that this function is not interrupted!
    ; Observes vram writing delays.
    ; Entry:
    ;        B = row, C = column.
    ;        HL = Pointer to string.
    ; Uses: AF, BC, DE, HL
    push hl                       ; Save pointer and string length for later.
    ld hl,NAME_TABLE_START        ; Get ready to calculate destination address.
    ld a,b
    cp 0
    jp z,add_row_offset_end
      ld de,$40                   ; For each row, add $40 to the dest. address.
      -:
        add hl,de
      djnz -
    add_row_offset_end:
    ld a,c
    cp 0
    jp z,add_column_offset_end
      ld b,a
      ld de,2                     ; Each column adds 1 word to dest. address.
      -:
        add hl,de
      djnz -
    add_column_offset_end:
    ld a,l                        ; Setup vram to load bytes at dest. address.
    out (CONTROL_PORT),a
    ld a,h
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    ;
    pop hl                          ; Get pointer to string from stack.
    -:
      ld a,(hl)                     ; Get character from string.
      cp STRING_TERMINATOR
      ret z
      out (DATA_PORT),a             ; Write it to name table.
      push ix
      pop ix
      ld a,%00000000                ; Select background palette for this char.
      out (DATA_PORT),a             ; Write 2nd byte to name table.
      push ix
      pop ix
      inc hl                        ; Point to next char.
    djnz -
  ret
  ;
  set_cursor:
    ; Set cursor used by put_char.
    ; Entry: A = cursor_y / row, B = cursor_x / column.
    ; Exit: None. Contents of A and B stay unchanged.
    ; Uses: None.
    push af
      ld (cursor_y),a
      ld a,b
      ld (cursor_x),a
    pop af
  ret
  ;
  put_char:
    ; Put char at cursor y,x and move cursor right.
    ; N.B. Make sure that this function is not interrupted!
    ; Entry: A = tile number (0-255 - in tile bank 1).
    ; Uses: A, B and C, but saves the rest. - is this still true?!
    SAVE_REGISTERS
    push af                       ;
    ld hl,NAME_TABLE_START        ; Get ready to calculate destination address.
    ld a,(cursor_y)
    ld b,a
    cp 0
    jp z,+
      ld de,$40                   ; For each row, add $40 to the dest. address.
      -:
        add hl,de
      djnz -
    +:
    ld a,(cursor_x)
    cp 0
    jp z,+
      ld b,a
      ld de,2                     ; Each column adds 1 word to dest. address.
      -:
        add hl,de
      djnz -
    +:
    inc a
    ld (cursor_x),a               ; Inc cursor x position.
    ld a,l                        ; Setup vram to load bytes at dest. address.
    out (CONTROL_PORT),a
    ld a,h
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    ;
    pop af                        ; Get tile.
    out (DATA_PORT),a             ; Write it to name table.
    push ix
    pop ix
    ld a,%00000000                ; Select background palette for this char.
    out (DATA_PORT),a             ; Write 2nd byte to name table.
  RESTORE_REGISTERS
  ret
  ;
  put_string_at_cursor:
    ; Put a string of multiple characters on the nametable with minimum
    ; overhead. Use current cursor position. Does not depend on '#' to
    ; terminate. Use only chars in background bank. Should be pretty interrupt-
    ; safe with di/ei and VDP access delays.
    ; Entry: B = Number of characters in string.
    ;        HL = pointer to string
    ; Exit: None
    ; Uses: None
    SAVE_REGISTERS
    push bc
    push hl                       ;
    ld hl,NAME_TABLE_START        ; Get ready to calculate destination address.
    ld a,(cursor_y)
    ld b,a
    cp 0
    jp z,+
      ld de,$40                   ; For each row, add $40 to the dest. address.
      -:
        add hl,de
      djnz -
    +:
    ld a,(cursor_x)
    cp 0
    jp z,+
      ld b,a
      ld de,2                     ; Each column adds 1 word to dest. address.
      -:
        add hl,de
      djnz -
    +:
    inc a
    ld (cursor_x),a               ; Inc cursor x position.
    ld a,l                        ; Setup vram to load bytes at dest. address.
    di
    out (CONTROL_PORT),a
    ld a,h
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    ei
    ;
    pop hl
    pop bc                        ; Get tile.
    -:
      ld a,(hl)
      inc hl
      out (DATA_PORT),a           ; Write it to name table.
      push ix
      pop ix
      ld a,%00000000              ; Select background palette for this char.
      out (DATA_PORT),a           ; Write 2nd byte to name table.
    djnz -
    RESTORE_REGISTERS
  ret
  ;
  print_register_hl:
    ; Print the contents of register HL as 5 ascii digits using put_char at
    ; position cursor (y,x).
    ; Entry: HL = value to print.
    ; Uses AF, BC, HL
  	ld	bc,-10000
  	call	+
  	ld	bc,-1000
  	call	+
    ld	bc,-100
  	call	+
  	ld	c,-10
  	call +
  	ld	c,-1
    +:
      ld	a,ASCII_ZERO-1 ; Maybe 16 here?
      -:
        inc	a
  	    add	hl,bc
  	  jr	c,-
  	sbc	hl,bc
  	call put_char
  ret
  ;
  print_register_a:
    ; Print the contents of register A as 3 ascii digits using put_char at
    ; position cursor (y,x).
    ; Entry: A = value to print.
    ; Uses AF, BC, HL
    ld h,0
    ld l,a
    ld	bc,-100
  	call	+
  	ld	c,-10
  	call +
  	ld	c,-1
    +:
      ld	a,ASCII_ZERO-1 ;
      -:
        inc	a
  	    add	hl,bc
  	  jr	c,-
  	sbc	hl,bc
  	call put_char
  ret
.ends
;
