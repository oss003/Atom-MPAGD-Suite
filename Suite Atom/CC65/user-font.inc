; Character plot routine
;
; Uses store_style to determine how characters are plotted.
;  bit0 = bold
;  bit1 = italic
;  bit2 = underscore
;  bit3 = invert
;  bit4 = gray
;  bit5 = even
;  bit6 = double height
;  bit7 = not used
; Bits may be combined so bold+italic -> store_style = 1+2 = 3
;
; Variables:
;  store_style = byte for style to be used for character plotting
;  spr_data    = byte to be displayed
;  sprtable    = characterdata address
;  screen      = screenaddress 

; USER redirects the character print routine to the new routine

redirect:
	lda #$4c		; 'jmp' opcode
	sta pchark
	lda #<chrplot
	sta pchark+1
	lda #>chrplot
	sta pchark+2
	rts

; CHRPLOT is the new print routine with style
;  bit0 = bold
;  bit1 = italic
;  bit2 = underscore
;  bit3 = invert
;  bit4 = gray
;  bit5 = even
;  bit6 = double height
;  bit7 = not used
; Bits may be combined so bold+italic -> store_style = 1+2 = 3

chrplot:
	jsr gprad		; get screen address.

	ldy #0
	ldx #8
loop:
	lda (spr_table),y	; Get character byte to be displayed
	sta spr_data		; Set character data byte
bold:
	lda store_style
	and #1			; Check bold plotting
	beq italic		; If not, skip

	lda spr_data		; Get character data byte
	lsr a			; Shift right
	ora spr_data		; OR with original byte
	sta spr_data		; Set character data byte
italic:
	lda store_style
	and #2			; Check bold plotting
	beq underscore		; If not, skip

	lda spr_data		; Get character data byte
	cpx #5			; Check for middle pos character
	bmi underscore		; No shift for top
	lsr a			; Shift right for bottom
	sta spr_data		; Set character data byte
underscore:
	lda store_style
	and #4			; Check bold plotting
	beq underscore		; If not, skip

	lda spr_data		; Get character data byte
	cpx #1			; Bottom line?
	bne invert		; No, skip
	lda #255		; Yes, set line
	sta spr_data		; Set character data byte
invert:
	lda store_style
	and #8			; Check bold plotting
	beq gray		; If not, skip

	lda spr_data		; Get character data byte
	eor #255		; Invert 
	sta spr_data		; Set character data byte
gray:
	lda store_style
	and #16			; Check bold plotting
	beq double		; If not, skip

	txa			
	lsr a			; Check for even line
	bcc even		; If so skip
	lda spr_data		; Get character data byte
	and #$55		; Mask byte
	sta spr_data		; Set character data byte
	jmp double
even:
	lda spr_data		; Get character data byte
	and #$aa		; Mask byte
	sta spr_data		; Set character data byte
double:
	lda store_style
	and #32			; Check bold plotting
	beq normal		; If not, skip

	lda spr_data		; Get character data byte
	sta (screen),y		; Display byte
	clc
	lda screen
	adc #32
	sta screen		; Point 1 line below
	bne :+
	inc screen+1
normal:
	sta (screen),y		; Display byte

	inc spr_table		; Point to next character byte
	bne :+
	inc spr_table+1
:
	clc
	lda screen		; Point on screen 1 line below
	adc #32
	sta screen
	bne :+
	inc screen+1
:
	dex			; Next line
	beq end			; Repeat for 8 lines
	jmp loop
end:				; End character plot
	rts