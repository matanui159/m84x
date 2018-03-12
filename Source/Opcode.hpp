#ifndef OPCODE_HPP_
#define OPCODE_HPP_
#include "System.hpp"

enum class Opcode : System::Byte {
	LDA,
	STA,
	LUI,
	ADI,

	ADD,
	SUB,
	ORA,
	AND,

	XOR,
	EXT,
	CMP,
	BEQ,

	LDM,
	STM,
	LMU,
	JLM,

	SWP,
	SHL,
	HLT,
	SHR,

	SET,
	DAT,
	PAL,

	LIMIT
};

#endif
