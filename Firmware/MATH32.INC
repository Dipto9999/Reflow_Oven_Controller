$NOLIST
;----------------------------------------------------
; math32.asm: Addition, subtraction, multiplication,
; and division of 32-bit integers. Also included are
; binary to bcd and bcd to binary conversion subroutines.
;
; 2011-2013 by Jesus Calvino-Fraga
;
;----------------------------------------------------

CSEG

;----------------------------------------------------
; Converts the 32-bit hex number in 'x' to a
; 10-digit packed BCD in 'bcd' using the
; double-dabble algorithm.
;---------------------------------------------------
hex2bcd:
	PUSH acc
	PUSH psw
	PUSH AR0
	PUSH AR1
	PUSH AR2

	CLR a
	MOV bcd+0, a ; Initialize BCD to 00-00-00-00-00
	MOV bcd+1, a
	MOV bcd+2, a
	MOV bcd+3, a
	MOV bcd+4, a
	MOV r2, #32  ; Loop counter.

hex2bcd_L0:
	; ShIft binary left
	MOV a, x+3
	MOV c, acc.7 ; This way x remains unchanged!
	MOV r1, #4
	MOV r0, #(x+0)
hex2bcd_L1:
	MOV a, @r0
	rlc a
	MOV @r0, a
	inc r0
	DJNZ r1, hex2bcd_L1

	; Perform bcd + bcd + carry using BCD arithmetic
	MOV r1, #5
	MOV r0, #(bcd+0)
hex2bcd_L2:
	MOV a, @r0
	addc a, @r0
	da a
	MOV @r0, a
	inc r0
	DJNZ r1, hex2bcd_L2

	DJNZ r2, hex2bcd_L0

	POP AR2
	POP AR1
	POP AR0
	POP psw
	POP acc
	RET


;----------------------------------------------------
; hex2bcd2:
; Converts the 32-bit hex number in 'x' to a
; 10-digit packed BCD in 'bcd' using the
; double-dabble algorithm.  This is what you would
; have to do in a proccessor without a bcd addition
; instruction.  The 8051 can add bcd number, so
; this function is here for your reference only.  Compare
; to the function above which uses the DA A instruction
; resulting in faster and smaller code.
;---------------------------------------------------
hex2bcd2:
	PUSH acc
	PUSH psw
	PUSH AR0
	PUSH AR1
	PUSH AR2

	CLR a
	MOV bcd+0, a ; Initialize BCD to 00-00-00-00-00
	MOV bcd+1, a
	MOV bcd+2, a
	MOV bcd+3, a
	MOV bcd+4, a
	MOV r2, #32  ; We need process 32 bits

hex2bcd2_L0:
	; ShIft binary left
	MOV a, x+3
	MOV c, acc.7 ; This way x remains unchanged!
	MOV r1, #4
	MOV r0, #(x+0)
hex2bcd2_L1:
	MOV a, @r0
	rlc a
	MOV @r0, a
	inc r0
	DJNZ r1, hex2bcd2_L1

	; ShIf bcd left
	MOV r1, #5	          ; BCD byte count = 5
	MOV r0, #(bcd+0)      ; r0 points to least signIficant bcd digits
hex2bcd2_L2:
	PUSH psw              ; Save carry
	MOV a, @r0
	add a, #33h           ; Pre-correction before shIfting left
	JB acc.7, hex2bcd2_L3 ; If the bcd digit was > 4 keep the correction
	add a, #(100h-30h)    ; Remove the correction to the MSD by subtracting 30h
hex2bcd2_L3:
	JB acc.3, hex2bcd2_L4 ; If the bcd digit was > 4 keep the correction
	add a, #(100h-03h)    ; Remove the correction to the LSD by subtracting 03h
hex2bcd2_L4:
	POP psw               ; Restore carry
	rlc a
	MOV @r0, a
	inc r0
	DJNZ r1, hex2bcd2_L2

	DJNZ r2, hex2bcd2_L0

	POP AR2
	POP AR1
	POP AR0
	POP psw
	POP acc

	RET

;------------------------------------------------
; bcd2hex:
; Converts the 10-digit packed BCD in 'bcd' to a
; 32-bit hex number in 'x'
;------------------------------------------------
bcd2hex:
	PUSH acc
	PUSH psw
	PUSH AR0
	PUSH AR1
	PUSH AR2

	MOV r2, #32  ; We need 32 bits

bcd2hex_L0:
	MOV r1, #5	           ; BCD byte count = 5
	CLR c                  ; clear carry flag
	MOV r0, #(bcd+4)       ; r0 points to most signIficant bcd digits
bcd2hex_L1:
	MOV a, @r0             ; transfer bcd to accumulator
	rrc a                  ; rotate right
	PUSH psw               ; save carry flag
	; BCD divide by two correction
	JNB acc.7, bcd2hex_L2  ; test bit 7
	add a, #(100h-30h)     ; bit 7 is set. Perform correction by subtracting 30h.
bcd2hex_L2:
	JNB acc.3, bcd2hex_L3  ; test bit 3
	add a, #(100h-03h)     ; bit 3 is set. Perform correction by subtracting 03h.
bcd2hex_L3:
	MOV @r0, a             ; store the result
	dec r0                 ; point to next pair of bcd digits
	POP psw                ; restore carry flag
	DJNZ r1, bcd2hex_L1    ; repeat for all bcd pairs

	; rotate binary result right
	MOV r1, #4
	MOV r0, #(x+3)
bcd2hex_L4:
	MOV a, @r0
	rrc a
	MOV @r0, a
	dec r0
	DJNZ r1, bcd2hex_L4

	DJNZ r2, bcd2hex_L0

	POP AR2
	POP AR1
	POP AR0
	POP psw
	POP acc

    RET

;------------------------------------------------
; x = x + y
;------------------------------------------------
add32:
	PUSH acc
	PUSH psw
	MOV a, x+0
	add a, y+0
	MOV x+0, a
	MOV a, x+1
	addc a, y+1
	MOV x+1, a
	MOV a, x+2
	addc a, y+2
	MOV x+2, a
	MOV a, x+3
	addc a, y+3
	MOV x+3, a
	POP psw
	POP acc
	RET

;------------------------------------------------
; x = x - y
;------------------------------------------------
sub32:
	PUSH acc
	PUSH psw
	CLR c
	MOV a, x+0
	SUBB a, y+0
	MOV x+0, a
	MOV a, x+1
	SUBB a, y+1
	MOV x+1, a
	MOV a, x+2
	SUBB a, y+2
	MOV x+2, a
	MOV a, x+3
	SUBB a, y+3
	MOV x+3, a
	POP psw
	POP acc
	RET

;------------------------------------------------
; mf=1 If x < y
;------------------------------------------------
x_lt_y:
	PUSH acc
	PUSH psw
	CLR c
	MOV a, x+0
	SUBB a, y+0
	MOV a, x+1
	SUBB a, y+1
	MOV a, x+2
	SUBB a, y+2
	MOV a, x+3
	SUBB a, y+3
	MOV mf, c
	POP psw
	POP acc
	RET

;------------------------------------------------
; mf=1 If x > y
;------------------------------------------------
x_gt_y:
	PUSH acc
	PUSH psw
	CLR c
	MOV a, y+0
	SUBB a, x+0
	MOV a, y+1
	SUBB a, x+1
	MOV a, y+2
	SUBB a, x+2
	MOV a, y+3
	SUBB a, x+3
	MOV mf, c
	POP psw
	POP acc
	RET

;------------------------------------------------
; mf=1 If x = y
;------------------------------------------------
x_eq_y:
	PUSH acc
	PUSH psw
	CLR mf
	CLR c
	MOV a, y+0
	SUBB a, x+0
	jnz x_eq_y_done
	MOV a, y+1
	SUBB a, x+1
	jnz x_eq_y_done
	MOV a, y+2
	SUBB a, x+2
	jnz x_eq_y_done
	MOV a, y+3
	SUBB a, x+3
	jnz x_eq_y_done
	SETB mf
x_eq_y_done:
	POP psw
	POP acc
	RET

;------------------------------------------------
; mf=1 If x >= y
;------------------------------------------------
x_gteq_y:
	LCALL x_eq_y
	JB mf, x_gteq_y_done
	LJMP x_gt_y
x_gteq_y_done:
	RET

;------------------------------------------------
; mf=1 If x <= y
;------------------------------------------------
x_lteq_y:
	LCALL x_eq_y
	JB mf, x_lteq_y_done
	LJMP x_lt_y
x_lteq_y_done:
	RET

;------------------------------------------------
; x = x * y
;------------------------------------------------
mul32:

	PUSH acc
	PUSH b
	PUSH psw
	PUSH AR0
	PUSH AR1
	PUSH AR2
	PUSH AR3

	; R0 = x+0 * y+0
	; R1 = x+1 * y+0 + x+0 * y+1
	; R2 = x+2 * y+0 + x+1 * y+1 + x+0 * y+2
	; R3 = x+3 * y+0 + x+2 * y+1 + x+1 * y+2 + x+0 * y+3

	; Byte 0
	MOV	a,x+0
	MOV	b,y+0
	mul	ab		; x+0 * y+0
	MOV	R0,a
	MOV	R1,b

	; Byte 1
	MOV	a,x+1
	MOV	b,y+0
	mul	ab		; x+1 * y+0
	add	a,R1
	MOV	R1,a
	CLR	a
	addc a,b
	MOV	R2,a

	MOV	a,x+0
	MOV	b,y+1
	mul	ab		; x+0 * y+1
	add	a,R1
	MOV	R1,a
	MOV	a,b
	addc a,R2
	MOV	R2,a
	CLR	a
	rlc	a
	MOV	R3,a

	; Byte 2
	MOV	a,x+2
	MOV	b,y+0
	mul	ab		; x+2 * y+0
	add	a,R2
	MOV	R2,a
	MOV	a,b
	addc a,R3
	MOV	R3,a

	MOV	a,x+1
	MOV	b,y+1
	mul	ab		; x+1 * y+1
	add	a,R2
	MOV	R2,a
	MOV	a,b
	addc a,R3
	MOV	R3,a

	MOV	a,x+0
	MOV	b,y+2
	mul	ab		; x+0 * y+2
	add	a,R2
	MOV	R2,a
	MOV	a,b
	addc a,R3
	MOV	R3,a

	; Byte 3
	MOV	a,x+3
	MOV	b,y+0
	mul	ab		; x+3 * y+0
	add	a,R3
	MOV	R3,a

	MOV	a,x+2
	MOV	b,y+1
	mul	ab		; x+2 * y+1
	add	a,R3
	MOV	R3,a

	MOV	a,x+1
	MOV	b,y+2
	mul	ab		; x+1 * y+2
	add	a,R3
	MOV	R3,a

	MOV	a,x+0
	MOV	b,y+3
	mul	ab		; x+0 * y+3
	add	a,R3
	MOV	R3,a

	MOV	x+3,R3
	MOV	x+2,R2
	MOV	x+1,R1
	MOV	x+0,R0

	POP AR3
	POP AR2
	POP AR1
	POP AR0
	POP psw
	POP b
	POP acc

	RET

;------------------------------------------------
; x = x / y
; This subroutine uses the 'paper-and-pencil'
; method described in page 139 of 'Using the
; MCS-51 microcontroller' by Han-Way Huang.
;------------------------------------------------
div32:
	PUSH acc
	PUSH psw
	PUSH AR0
	PUSH AR1
	PUSH AR2
	PUSH AR3
	PUSH AR4

	MOV	R4,#32
	CLR	a
	MOV	R0,a
	MOV	R1,a
	MOV	R2,a
	MOV	R3,a

div32_loop:
	; ShIft the 64-bit of [[R3..R0], x] left:
	CLR c
	; First shIft x:
	MOV	a,x+0
	rlc a
	MOV	x+0,a
	MOV	a,x+1
	rlc	a
	MOV	x+1,a
	MOV	a,x+2
	rlc	a
	MOV	x+2,a
	MOV	a,x+3
	rlc	a
	MOV	x+3,a
	; Then shIft [R3..R0]:
	MOV	a,R0
	rlc	a
	MOV	R0,a
	MOV	a,R1
	rlc	a
	MOV	R1,a
	MOV	a,R2
	rlc	a
	MOV	R2,a
	MOV	a,R3
	rlc	a
	MOV	R3,a

	; [R3..R0] - y
	CLR c
	MOV	a,R0
	SUBB a,y+0
	MOV	a,R1
	SUBB a,y+1
	MOV	a,R2
	SUBB a,y+2
	MOV	a,R3
	SUBB a,y+3

	jc	div32_minus		; temp >= y?

	; -> yes;  [R3..R0] -= y;
	; CLR c ; carry is always zero here because of the jc above!
	MOV	a,R0
	SUBB a,y+0
	MOV	R0,a
	MOV	a,R1
	SUBB a,y+1
	MOV	R1,a
	MOV	a,R2
	SUBB a,y+2
	MOV	R2,a
	MOV	a,R3
	SUBB a,y+3
	MOV	R3,a

	; Set the least signIficant bit of x to 1
	orl	x+0,#1

div32_minus:
	DJNZ R4, div32_loop	; -> no

div32_exit:

	POP AR4
	POP AR3
	POP AR2
	POP AR1
	POP AR0
	POP psw
	POP acc

	RET

Load_X MAC
	MOV x+0, #low (%0 % 0x10000)
	MOV x+1, #high(%0 % 0x10000)
	MOV x+2, #low (%0 / 0x10000)
	MOV x+3, #high(%0 / 0x10000)
ENDMAC

Load_y MAC
	MOV y+0, #low (%0 % 0x10000)
	MOV y+1, #high(%0 % 0x10000)
	MOV y+2, #low (%0 / 0x10000)
	MOV y+3, #high(%0 / 0x10000)
ENDMAC

$LIST
