;----------------------------------------------
; USER.INC
; Tents assembler routines
; Atom version by Kees van Oss 2023
;----------------------------------------------

;--------------------------------------------------------------------
; Variables:
;--------------------------------------------------------------------

; Global

global_zp	= $80
fields		= global_zp 
tents		= global_zp + 1
size		= global_zp + 2
seed		= global_zp + 3

; Local

local_zp	= $90
ptr_matrix	= local_zp + 2
ptr_tent	= local_zp + 3
ptr_tree	= local_zp + 4
direction	= local_zp + 5
itteration	= local_zp + 6
tmp		= local_zp + 7
xtent		= local_zp + 8
ytent		= local_zp + 9
rnd_max		= local_zp + 10
array_ptr	= local_zp + 12

;--------------------------------------------------------------------
; Constants
;--------------------------------------------------------------------

field_clear	= 0
field_marked	= 1
field_tent	= 2
field_tree	= 3

;-------------------------------------------------------------
; MPAGD entry 
; A=0, generate grid and fill matrices
; A=1, return gridvalue at scr_matrix(varh,varv) in varopt
; A=2, check if puzzle solved, varopt=0 (no), varopt=1 (yes)
; A=3, action pressed at varh,varx, return gridvalue in varopt
;-------------------------------------------------------------

	bne chk1
	jmp algoritm_generate_grid	; A=0, generate grid
chk1:
	cmp #1
	bne chk2
chk2:
	rts

;--------------------------------------------------------------------
; Algoritm to create grid with tents and trees
;--------------------------------------------------------------------

algoritm_generate_grid:
	jsr algoritm_init
tentloop:
	jsr get_rnd_tent		; Get random tent field
	jsr upd_field_array		; Remove field from free fields array
	bcs error_no_solution		; All fields tested, no solution
	jsr chk_tent_area		; Tents touching?
	bcc :+				; No, place tent
	jmp tentloop			; Yes, get new tent location
:
	ldx ptr_tent			; Point to field in matrix
	lda #field_tent
	sta sol_matrix,x		; Place tent in matrix

	dec tents			; Next tent
	bne tentloop			; If not all tents doen, repeat

	jsr placetrees			; Place trees

	rts

error_no_solution:
	jmp algoritm_generate_grid	; ERROR, no solution possible, restart

;--------------------------------------------------------------------
; Init:
;--------------------------------------------------------------------
; - Calculate fields = width x height
; - Calculate nr of tents = fields / 5
; - Fill sol_matrix and scr_matrix with 0 (empty)
; - Fill random field array with fieldnr's
;--------------------------------------------------------------------

algoritm_init:
	ldx size		; Calculate fields = size x size
	ldy size
	jsr multiply
	sta fields
	sta rnd_max		; Set max random numbers

	ldx fields		; Calculate tents = fields / 5
	ldy #5
	jsr divide
	stx tents

	lda #field_clear		; Clear matrices
	ldx fields
	dex
mat_loop:
	sta sol_matrix,x
	sta scr_matrix,x
	dex
	bpl mat_loop

	ldx #0			; Fill field array
arr_loop:
	txa
	sta field_array,x
	inx
	cpx fields
	bne arr_loop

	rts

;--------------------------------------------------------------------
; GET_RND_TENT:
;--------------------------------------------------------------------
; - Get random field for tent
; - Convert fieldnr to X,Y coordinates in grid
;--------------------------------------------------------------------

get_rnd_tent:

; Get random field from field array

	ldy rnd_max		; Set max free fields
	jsr qrand		; Get free field from field array
	sta array_ptr
	tax
	lda field_array,x
	sta ptr_tent		; Set field pointer

; Convert fieldnr to X,Y coordinates

get_coord_tent:
	tax			; Get fieldnr
	ldy size		; Set size
	jsr divide		; Divide fields / size
	sta xtent		; remainder
	stx ytent		; quotient
	rts

;--------------------------------------------------------------------
; UPD_FIELD_ARRAY:
;--------------------------------------------------------------------
; - Remove field from free fields array
; - Update rnd_max
; - CLC = ok, SEC = no solution
;--------------------------------------------------------------------

upd_field_array:
	lda rnd_max
	beq upd_error
	ldx array_ptr
upd_loop:
	lda field_array+1,x
	sta field_array,x
	inx
	cpx rnd_max
	bne upd_loop
	dec rnd_max
	clc
	rts

upd_error:
	sec
	rts

;--------------------------------------------------------------------
; CHK_TENT_AREA:
;--------------------------------------------------------------------
; - Check for collision horizont/vertical/diagonal with other tents
;--------------------------------------------------------------------

chk_tent_area:

; Fill array with adjacent field pointers

	sec			
	lda ptr_tent
	sbc size
	tax
	dex
	stx dir_ptr		; NW = ptr_tent - size - 1

	inx
	stx dir_ptr+1		;  N = ptr_tent - size 

	inx
	stx dir_ptr+2		; NE = ptr_tent - size + 1

	ldx ptr_tent
	dex
	stx dir_ptr+3		;  W = ptr_tent - 1

	inx
	inx
	stx dir_ptr+4		;  E = ptr_tent + 1

	clc
	lda dir_ptr+4
	adc size
	tax
	stx dir_ptr+5		; SE = ptr_tent + size + 1

	dex
	stx dir_ptr+6		;  S = ptr_tent + size 

	dex
	stx dir_ptr+7		; SW = ptr_tent + size - 1

check_NW:
	lda xtent	
	beq check_N
	lda ytent
	beq check_N
	ldy dir_ptr
	jsr location		; Tent?
	bcs occupied		; Yes, occupied, jump
check_N:
	lda ytent	
	beq check_NE
	ldy dir_ptr+1
	jsr location		; Tent?
	bcs occupied		; Yes, occupied, jump
check_NE:
	ldx xtent	
	inx
	cpx size
	beq check_W
	lda ytent
	beq check_W
	ldy dir_ptr+2
	jsr location		; Tent?
	bcs occupied		; Yes, occupied, jump
check_W:
	lda xtent	
	beq check_E
	ldy dir_ptr+3
	jsr location		; Tent?
	bcs occupied		; Yes, occupied, jump
check_E:
	ldx xtent
	inx
	cpx size	
	beq check_SE
	ldy dir_ptr+4
	jsr location		; Tent?
	bcs occupied		; Yes, occupied, jump
check_SE:
	ldx xtent	
	inx
	cpx size
	beq check_S
	ldx ytent
	inx
	cpx size
	beq check_S
	ldy dir_ptr+5
	jsr location		; Tent?
	bcs occupied		; Yes, occupied, jump
check_S:
	ldx ytent
	inx
	cpx size	
	beq check_SW
	ldy dir_ptr+6
	jsr location		; Tent?
	bcs occupied		; Yes, occupied, jump
check_SW:
	lda xtent	
	beq free
	ldx ytent
	inx
	cpx size
	beq free
	ldy dir_ptr+7
location:
	lda sol_matrix,y
	cmp #field_tent		; Tent?
	bne chk_tree
	jmp occupied
chk_tree:
	cmp #field_tree		; Tree?
	beq occupied
free:
	clc			; Location free
	rts
occupied:
	sec			; Location occupied
	rts

dir_ptr:	.byte -33,-32,-31,-1,31,32,33,1


;--------------------------------------------------------------------
; PlaceTrees:
;--------------------------------------------------------------------
; - Check grid for tent
; - Get direction = rnd(N,E,S,W)
; - Place tree if grid location is free
;--------------------------------------------------------------------

placetrees:
	lda #0			; Reset matrix pointer
	sta ptr_matrix		; Set tent pointer

treeloop:
	ldx ptr_matrix
	lda sol_matrix,x	; Get field value
	cmp #field_tent		; Is it a tent?
	beq :+			; Yes, proces
next_field:
	inc ptr_matrix		; No, next field
	lda ptr_matrix
	cmp fields		; All fields checked?
	bne treeloop		; No, repeat
	rts			; Yes, ready
:
; Check if ortho position is free

	lda ptr_matrix
	jsr get_coord_tent	; Get xtent,ytent

	lda #4			; Reset direction flag
	sta itteration

	ldy #4
	jsr qrand		; Get rnd(0-3) direction
	sta direction
dir_check:
	lda direction
	cmp #0			; direction=0, check N
	bne tset1
	lda ytent		; Topborder?
	beq next_dir		; Yes, skip
	sec
	lda ptr_matrix
	sbc size
	sta ptr_tree		; ptr_tree = ptr_tent - size
	jmp check_ok
tset1:
	lda direction
	cmp #1			; direction=1, check W
	bne tset2
	lda xtent		; Left border?
	beq next_dir		; Yes, next dir
	lda ptr_matrix
	sta ptr_tree
	dec ptr_tree		; ptr_tree = ptr_tent - 1
	jmp check_ok
tset2:
	lda direction
	cmp #2			; direction=2, check S
	bne tset3
	ldy ytent		; Bottomborder?
	iny
	cpy size
	beq next_dir		; Yes, next dir
	clc
	lda ptr_matrix
	adc size
	sta ptr_tree		; ptr_tree = ptr_tent + size
	jmp check_ok
tset3:
	lda direction
	cmp #3			; direction=3, check E
	bne next_dir
	ldy xtent		; Rightborder?
	iny
	cpy size
	beq next_dir		; Yes, next_dir
	lda ptr_matrix
	sta ptr_tree
	inc ptr_tree		; ptr_tent = ptr_tree + 1
	jmp check_ok

next_dir:
	inc direction			; Not ok, test next direction
	lda direction
	and #3
	sta direction	
	dec itteration		; Check for all directions
	jmp dir_check
check_ok:
	ldx ptr_tree
	lda sol_matrix,x
	cmp #field_clear
	bne next_dir
	lda #field_tree		; Mark tree field
	sta sol_matrix,x
	sta scr_matrix,x
	jmp next_field
 
;--------------------------------------------------------------------
; Multiply A = X x Y
;--------------------------------------------------------------------
;  Input:  X,Y (max 15)
;  Output: A = lb result
;          Y = hb result
;--------------------------------------------------------------------

multiply:
	sty tmp
	tya
	ldy #0
	dex
mulloop:
	clc
	adc tmp
	bcc :+
	iny
:	dex
	bne mulloop
	rts

;--------------------------------------------------------------------
; Divide A = X / Y
;--------------------------------------------------------------------
;  Input:  X,Y
;  Output: X = quotient
;          A = remainder
;--------------------------------------------------------------------

divide:
	txa
	ldx #0
	sty tmp
	sec
divloop:
	sbc tmp
	bcc divend
	inx
	jmp divloop
divend:
	clc
	adc tmp
	rts

;-------------------------------------------------------------------------
; Random numbers code.
; Pseudo-random number generator, 8-bit.
;
; Output:
;  RND = random number
;------------------------------------------------

random:
	lda seed
	beq doEor
	asl a
	beq noEor
	bcc noEor
doEor:	eor #$1d
noEor:	sta seed
	rts

qrand:
	dey
	jsr random
	tax
	jsr multiply
	tya
	rts

;-------------------------------------------------------------
; Grid matrices
;-------------------------------------------------------------

sol_matrix:	.res(15*15)
scr_matrix:	.res(15*15) 
field_array:	.res(15*15)

eind_asm:
