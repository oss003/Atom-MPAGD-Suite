; Blink collectable blocks on screen
; Added for Forward to the Past

; Reset addresses

	lda #>ScreenAddr	; reset screenaddress
	sta scrptr+2
	sta scrptr1+2

	lda #<MAP		; reset mapaddress
	sta mapptr+1
	lda #>MAP
	sta mapptr+2
	
; Loop through map

vloop:
	lda #31			; set
	sta colptr		; hor. pointer
hloop:
	ldx colptr
mapptr:
	lda MAP,x		; get blocktype
	cmp #COLECT		; is it collectable?
	beq found		; yes, invert
back:
	dec colptr		; decrement hor.pointer
	bpl hloop		; repeat 32 times

	clc			; point to next row in MAP
	lda mapptr+1
	adc #$20
	sta mapptr+1
	lda mapptr+2
	adc #0
	sta mapptr+2

	inc scrptr+2		; point to next blockrow on screen
	inc scrptr1+2
	lda scrptr1+2
	cmp #>$9800
	bne vloop

	rts

; Invert collectable block

found:
	txa			; Update x-address
	sta scrptr+1
	sta scrptr1+1

	ldx #7			; Invert 8 bytes
invloop:
	ldy scrtab,x
scrptr:
	lda ScreenAddr,y
	eor diamond,x
;	eor #$ff
scrptr1:
	sta ScreenAddr,y
	dex
	bpl invloop
	jmp back

colptr:	.byte 0
diamond:	.byte 24,60,102,219,189,165,153,126
