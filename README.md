# M84X
## About
This started as a challenge I gave myself when I was thinking about the old 8-bit processes like the 6502 (used in the NES and C64) and the Z80 (used in the GameBoy and ZX Spectrum).

The thing that bugs me about these is that they aren't *really* 8-bit.

For example, these days, you get a 32-bit processor and:
* it works on 32-bits at a time,
* its address space is 32-bit (hence why you could only have 4GB of memory),
* its instructions are 32-bit

Same with a 64-bit processor:
* 64-bit arithmetic,
* 64-bit address space,
* 64-bit instructions

However, an 8-bit processor was different. At least for the 6502 (I'm unsure about the Z80):
* it works on 8-bits at a time,
* but the address space is 16-bits,
* and the instructions are variable size (ranging from 8-bits to 24-bits)

It was obvious why they did this, it would be difficult fitting all the opcodes and operands into 8-bit instructions and an 8-bit address space means you only have 256 bytes of memory to work with...

*...and this is where I come in... ^-^*

I gave myself the challenge to design a theoretical *pure* 8-bit processor, called the M84X, create an assembler and emulator for it, and make a game in it. Along with this, to add to the challenge I set a few extra rules:
* it had to be Harvard style (as in, the instructions have to take up the same memory)
* it needed input and visuals
* memory/code paging **wasn't** allowed

That means I had to share my limited 256 bytes between input, video, code, and well, memory.

This is the repository for the assembler/emulator (I made it one program) and the SNAK game I made (the game is pretty much snake but I didn't have enough room on the screen to write an E).

## Building the Project
### M84X
Sorry I didn't provide any build files, the IDE I use doesn't use them. However, it should work find with any compiler. I used C++11, and it has been tested with MinGW-W64. It requires SFML Graphics, Window and System.

### SNAK
Once M84X has been compiled, just run `m84x ao snake.asm snake.bin` to compile SNAK.

## How to use the M84X software
The way the program handles flags is a bit weird. The first argument is a list of flags where each flag is a letter that are not preceded by anything. Then any flag that requires extra arguments will read the remaining arguments in order. For example, if you ran:
```
m84x ao snake.asm snake.bin
```
the `a` flag would use the argument `snake.asm` and the `o` flag would use the argument `snake.bin`.

The full details of the program is provided by running `m84x h`:
```
m84x [flags] [arguments]
	a [file]: assembles the file
	o [file]: sets output file for assembly
	r [file]: runs the file
	d: enables debug during running
	h: shows help

output file for assembly follows this order:
	if the o option is provided, it will use that
	else, if the r option is provided, it will use that
	else, it will be the input file suffixed with .bin

example: m84x ard input.asm output.bin
	this assembles input.asm to output.bin
	then runs output.bin
	with debug enabled
```

## The Instruction Set
All instructions are split into two nybbles (4-bits). The upper nybble is the opcode and the lower nybble is the operand. The M84X has 4 registers:
* `I`: the instruction pointer
* `A`: the accumulator
* `M`: the memory pointer
* `E`: the 1-bit comparison result flag

Letting `x` be the 4-bit operand and `n` being the memory array, all the opcodes are as follows:
```
0_ LDA: A = n[M + x] (LoaD A)
1_ STA: n[M + x] = A (STore A)
2_ LUI: A = x << 4 (Load Upper Immediate)
3_ ADI: A = A + x (Add Immediate)

4_ ADD: A = A + n[M + x] (ADD)
5_ SUB: A = A - n[M + x] (SUBtract)
6_ ORA: A = A | n[M + x] (OR A)
7_ AND: A = A & n[M + x] (AND)

8_ XOR: A = A ^ n[M + x] (eXclusive OR)
9_ EXT: depends on operand: (EXTended)
	90 SWP: swap A and M
	91-97 SHL: A = A << x
	98 HLT: waits until screen is drawn
	99-9F SHR: A = A >> (x - 8)
A_ CMP: sets E to 1 if A == n[M + x], 0 otherwise (CoMPare)
B_ BEQ: if E == 1, I = I + x (sign extended) (Branch if EQual)

C_ LDM: M = n[M + x] (LoaD M)
D_ STM: n[x] = M (STore M)
E_ LMU: M = x << 4 (Load M Upper immediate)
F_ JLM: M = M + x, then swaps the value of M and I (Jump and Link M)
```


## The Assembly Language
### Numbers
First thing to understand is that all numbers are in hexadecimal and must start with `#`. For example:
```
label = #5A
```

### Labels
There are two forms of a label. Theres the standard type:
```
label:
	; code goes here
```
And theres the constant:
```
label = ; some number here
```
You can't assign one label to another label. You can have multiple standard labels on the same line. A label name can have letters, numbers, underscores and fullstops. It's ok for a label to start with a number. Labels are case insensitive.

### Instructions
Instructions will always follow the standard form:
```
[OPCODE] [OPERAND]
```
For example:
```
LDA #05
STA label
```
Instruction opcodes are also case insensitive. All instructions must have an operand. If the instruction doesn't need an operand, just put `#`, which evaluates to 0. For example:
```
HLT #
```

### Instruction Modifiers
By default, an instruction will use the lowest nybble (4-bits) of the value given. If you don't want this and you want something different, you can use an instruction modifier. You put the modifier after the opcode and before the operand, for example:
```
LDA ^#5A
```
`^` will use the upper nybble instead of the lower nybble.

`~` will use the difference between the value and the current address.

`*` will put an `LMU ^[value]` before the instruction.

### Instruction Opcodes
The assembler supports all opcodes listed above in the "Instruction Set" section (including the extended opcodes, it will just convert them to the respective EXT instructions), along with the following pseudo opcodes:

`SET` will set the value of the `A` register to the value. If used with `^`, it will just insert a `LUI` instruction. If used with any other modifier (including no modifier), it will insert both `LUI` and `ADI` instructions.

`DAT` will just insert that raw byte into the compiled result. This opcode also takes note of modifiers, so given:
```
DAT #5A
DAT *#5A
```
The first will insert `0x0A`, and the second will insert `0x5A`.

`PAL` will repeatedly insert `DAT` instructions with given value until the address reaches a multiple of 16 (Page ALign).

## The Runtime
### Visuals
The screen is 16x16 and supports 4 colors out of a palette of 16 (RGBI). The video memory starts at address `C0` and takes up 64 bytes. Each byte represents a 2x2 block. This allows one to program easily using an 8x8 display by just filling up the 2x2 block with all the same color in a single command. However, if one wanted to, they could use extra instructions to use the full 16x16 display.

The first two bits of the byte are the top left pixel of the block, the next two bits are the top right block, the next are the bottom left and the final 2 are the bottom right. The colors 0 and 1 are stored in the upper and lower nybble of address `00` respectively, with the colors 2 and 3 in address `01`. The supported colors are:
```
0: Black
1: Blue
2: Green
3: Cyan
4: Red
5: Purple
6: Brown
7: Light Grey
8: Grey
9: Light Blue
A: Light Green
B: Light Cyan
C: Light Red
D: Pink
E: Yellow
F: White
```

### Input
In a similar fassion to the visuals, I wanted to design the input in a way such that they would be easy to use, however if someone wanted to go further, they could. The input controllers supports a single DPAD (arrow keys) and two extra buttons: A (Z key) and B (X key). There is also a direction value which tells the last DPAD button pressed:
```
0: left
1: up
2: down
3: right
```

The input is stored in the address `02` and is layed out like so:
```
0: left button
1: up button
2: down button
3: right button
4-5: direction
6: A button
7: B button
```

### Code
The code starts at address `00`. For this reason it is recommended to insert a `JLM *[start address]` at the start, allowing you to later overwrite the color values and also so you don't run into the possibly random input byte at address `03`.

### Extra Note
The `HLT` instruction will poll events, draw the frame, and wait up to a tenth of a second. For this reason, the runtime (and by extension the SNAK game) may freeze if the code doesn't reach another `HLT` instruction.

## The Debugger
Everytime the debugger stops, it will show a dump of all 256 bytes of memory, the current register values, and the decompiled line it's about to run. It also shows certain predictions of what this value could have been along with the values in memory they point to.

After which it asks the user which line they want to goto. If the user doesn't enter anything and just presses enter, it will stop at the next instruction, otherwise it will keep running until it hits the address provided.

## The SNAK Game
Finally, just a couple of comments about the game. For this game I needed an array as large as the play area to keep track of certain information. This is the reason the play area is so small. Any larger and you would notice weird artifacts popping up in the top right of the screen when you move to the bottom right as the data chunk is right before the video memory. I took this as an advantage however and "tried" writing snake at the bottom by hardcoding it to show off the possibility of a 16x16 resolution.

## Known Issues
There is a problem where the assembler sometimes won't work properly if the first line is a label (for some reason), to fix this simply add a newline before the first label. I don't know why this happens, if someone finds and fixes this issue via a pull request I will be very grateful.