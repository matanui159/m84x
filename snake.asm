
page0:
	color0:
		LMU ^init
	color1:
		JLM init
	input:
		DAT #
	m0:
		DAT #
	m1:
		DAT #
	addeq.cmp:
	move.dir:
		DAT #
	addeq.to:
	move.pos:
		DAT #
	addeq.from:
		DAT #
	head:
		DAT *#24
	tail:
		DAT *#24
	data:
		DAT *data_
	frame:
		DAT *frame_
	dir_mask:
		DAT *#0C
	zero:
		DAT *#00
	apple:
		DAT *#FF

PAL #
page1:
	rand:
		DAT #
	temp:
		DAT #
	seed:
		DAT *#15
	rand_mask:
		DAT *#3F
	p1_frame:
		DAT *#C0
	p1_zero:
		DAT *#00

addeq:
	STM m0
	LMU ^page0
	CMP addeq.cmp
	BEQ ~addeq.eq
	addeq.ret:
		LDM m0
		JLM #
	addeq.eq:
		LDA addeq.to
		ADD addeq.from
		STA addeq.to
	BEQ ~addeq.ret

move:
	STM m1

	; left
	LMU ^page0
	SET *#FF
	STA addeq.from
	ADI #01 ; SET *#00
	JLM *addeq

	; up
	LMU ^page0
	SET *#F8
	STA addeq.from
	ADI #09 ; SET *#01
	JLM *addeq

	; down
	LMU ^page0
	SET *#08
	STA addeq.from
	SET *#02
	JLM *addeq

	; right
	LMU ^page0
	SET *#01
	STA addeq.from
	ADI #02 ; SET *#03
	JLM *addeq

	LMU ^page0
	LDM m1
	JLM #

spawn_apple:
	STM m0

	; rng
	LMU ^page1
	LDA rand
	XOR seed
	STA rand
	SHL #05
	STA temp
	LDA rand
	SHR #03
	ORA temp
	SUB rand
	STA rand

	spawn_apple.loop:
		; check apple
		LMU ^page1
		STA temp
		AND rand_mask
		ADD p1_frame
		SWP #
		LDA #
		LMU ^page1
		CMP p1_zero
		LDA temp
		BEQ ~spawn_apple.zero
			ADI #0F
			JLM *spawn_apple.loop
		spawn_apple.zero:
			AND rand_mask
			ADD p1_frame
			SWP #
			SET *#FF
			STA #
			LMU ^page0
			LDM m0
			JLM #

main:
	; update input
	LMU ^page0
	LDA input
	AND dir_mask
	SHR #02
	STA move.dir
	LDA head
	STA move.pos
	ADD data
	LDM move.dir
	SWP #
	STA #

	; move head
	JLM *move
	LMU ^page0
	LDA move.pos
	STA head

	; check head
	ADD frame
	SWP #
	LDA #
	LMU ^page0
	CMP zero
	BEQ ~main.zero
		CMP apple
		BEQ ~main.apple
			JLM *end
		main.zero:
			BEQ ~main.zero2
		main.apple:
			JLM *spawn_apple
			JLM *main.draw_head
	main.zero2:

	; clear tail
	LMU ^page0
	LDA tail
	ADD frame
	SWP #
	STA #

	; move tail
	LMU ^page0
	LDA tail
	STA move.pos
	ADD data
	SWP #
	LDA #
	LMU ^page0
	STA move.dir
	JLM *move
	LMU ^page0
	LDA move.pos
	STA tail

main.draw_head:
	; draw head
	LMU ^page0
	LDA head
	ADD frame
	SWP #
	LUI #0A
data_:
	ADI #0A
	STA #

	HLT #
	HLT #
JLM *main

end:
	HLT #
	JLM *end

init:
	LMU ^page0
	SET *#08
	STA color0
	SET *#AC
	STA color1
	JLM *spawn_apple
	JLM *main

PAL #
DAT #
PAL #
frame_:
	; row 0
	DAT *#01
	DAT *#05
	DAT *#05
	DAT *#05
	DAT *#05
	DAT *#05
	DAT *#05
	DAT *#04

	; row 1
	DAT *#11
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#44

	; row 2
	DAT *#11
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#44

	; row 3
	DAT *#11
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#44

	; row 4
	DAT *#11
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#00
	DAT *#44

	; row 5
	DAT *#10
	DAT *#50
	DAT *#50
	DAT *#50
	DAT *#50
	DAT *#50
	DAT *#50
	DAT *#40

	; row 6
	DAT *#A8
	DAT *#33
	DAT *#0C
	DAT *#CC
	DAT *#A8
	DAT *#88
	DAT *#CF
	DAT *#C0

	; row 7
	DAT *#2A
	DAT *#33
	DAT *#30
	DAT *#CC
	DAT *#A8
	DAT *#88
	DAT *#CC
	DAT *#CC
