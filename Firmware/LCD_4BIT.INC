CSEG

; When using a 16.6MHz oscillator in the N76E003
; one cycle takes 1.0/16.6MHz = 60.24 ns

;---------------------------------;
; Wait 40 microseconds            ;
;---------------------------------;
Wait40uSec:
    PUSH AR0
    MOV R0, #133
L0:
    nop
    DJNZ R0, L0 ; 1+4 cycles->5*60.24ns*133=40us
    POP AR0
    RET

;---------------------------------;
; Wait 'R2' milliseconds          ;
;---------------------------------;
Wait_Milli_Seconds mac
	PUSH AR2
	MOV R2, %0
	LCALL ?Wait_Milli_Seconds
	POP AR2
endmac

;---------------------------------;
; Wait 'R2' milliseconds          ;
;---------------------------------;
?Wait_Milli_Seconds:
    PUSH AR0
    PUSH AR1
L3: MOV R1, #40
L2: MOV R0, #104
L1: DJNZ R0, L1 ; 4 cycles->4*60.24ns*104=25.0us
    DJNZ R1, L2 ; 25us*40=1.0ms
    DJNZ R2, L3 ; number of millisecons to wait passed in R2
    POP AR1
    POP AR0
    RET

;---------------------------------;
; Toggles the 'E' pin in the LCD  ;
;---------------------------------;
LCD_pulse:
	SETB LCD_E
	LCALL Wait40uSec
	CLR LCD_E
    RET

;---------------------------------;
; Writes acc to LCD in 4-bit mode ;
;---------------------------------;
LCD_byte:
	; Write high 4 bits first
	MOV c, ACC.7
	MOV LCD_D7, c
	MOV c, ACC.6
	MOV LCD_D6, c
	MOV c, ACC.5
	MOV LCD_D5, c
	MOV c, ACC.4
	MOV LCD_D4, c
    LCALL LCD_pulse
    LCALL Wait40uSec
	; Write low 4 bits next
	MOV c, ACC.3
	MOV LCD_D7, c
	MOV c, ACC.2
	MOV LCD_D6, c
	MOV c, ACC.1
	MOV LCD_D5, c
	MOV c, ACC.0
	MOV LCD_D4, c
    LCALL LCD_pulse
	RET

;---------------------------------;
; Write data to LCD               ;
;---------------------------------;
WriteData mac
	MOV a, %0
	LCALL ?WriteData
endmac

?WriteData:
	SETB LCD_RS
	LJMP LCD_byte

;---------------------------------;
; Write command to LCD            ;
;---------------------------------;
WriteCommand mac
	MOV a, %0
	LCALL ?WriteCommand
endmac

?WriteCommand:
	CLR LCD_RS
	LJMP LCD_byte

;---------------------------------;
; Configure LCD in 4-bit mode     ;
;---------------------------------;
LCD_4BIT:
	CLR LCD_E   ; Resting state of LCD's enable pin is zero
	; CLR LCD_RW  ; Only writing to the LCD.  Tie this pin to ground

	; After power on, let the LCD start up before initializing
	Wait_Milli_Seconds(#40)

	; First make sure the LCD is in 8-bit mode and then change to 4-bit mode
	WriteCommand(#0x33)
	WriteCommand(#0x33)
	WriteCommand(#0x32) ; change to 4-bit mode

	; Configure the LCD
	WriteCommand(#0x28)
	WriteCommand(#0x0c)
	WriteCommand(#0x01) ;  Clear screen command (takes some time)

    ;Wait for the clear screen command to finish.
	Wait_Milli_Seconds(#2)
    RET

;---------------------------------;
; Send a constant string to LCD   ;
;---------------------------------;
Send_Constant_String mac
	PUSH dph
	PUSH dpl
	PUSH acc
	MOV dptr, %0
	LCALL ?Send_Constant_String
	POP acc
	POP dpl
	POP dph
endmac

?Send_Constant_String:
    CLR a
    movc a, @a+dptr
    JZ ?Send_Constant_String_Done
    LCALL ?WriteData
    inc dptr
    sjmp ?Send_Constant_String
?Send_Constant_String_Done:
    RET

;---------------------------------;
; Set LCD cursor at row, column   ;
;---------------------------------;
Set_Cursor mac
	PUSH acc
	MOV a, #%1
	dec a
	LCALL ?Set_Cursor_%0 ; Select column and row
	POP acc
endmac

?Set_Cursor_2:
	orl a, #01000000B
?Set_Cursor_1:
	orl a, #10000000B
	LJMP ?WriteCommand ; Select column and row

;---------------------------------;
; Display a BCD number in the LCD ;
;---------------------------------;
Display_BCD mac
	PUSH ar0
	MOV r0, %0
	LCALL ?Display_BCD
	POP ar0
endmac

?Display_BCD:
	PUSH acc
	; Write most signIficant digit
	MOV a, r0
	swap a
	anl a, #0fh
	orl a, #30h
	LCALL ?WriteData
	; write least signIficant digit
	MOV a, r0
	anl a, #0fh
	orl a, #30h
	LCALL ?WriteData
	POP acc
	RET

;------------------------------------;
; Display a char in the LCD          ;
;------------------------------------;
Display_char mac
	PUSH acc
	MOV a, %0
	LCALL ?WriteData
	POP acc
endmac
