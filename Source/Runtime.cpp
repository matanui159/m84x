#include "Runtime.hpp"
#include "Opcode.hpp"
#include <fstream>
#include <locale>
#include <cstdio>

const System::Color Runtime::COLORS[16] = {
		{0x00, 0x00, 0x00, 0xFF},
		{0x00, 0x00, 0xAA, 0xFF},
		{0x00, 0xAA, 0x00, 0xFF},
		{0x00, 0xAA, 0xAA, 0xFF},
		{0xAA, 0x00, 0x00, 0xFF},
		{0xAA, 0x00, 0xAA, 0xFF},
		{0xAA, 0x55, 0x00, 0xFF},
		{0xAA, 0xAA, 0xAA, 0xFF},
		{0x55, 0x55, 0x55, 0xFF},
		{0x55, 0x55, 0xFF, 0xFF},
		{0x55, 0xFF, 0x55, 0xFF},
		{0x55, 0xFF, 0xFF, 0xFF},
		{0xFF, 0x55, 0x55, 0xFF},
		{0xFF, 0x55, 0xFF, 0xFF},
		{0xFF, 0xFF, 0x55, 0xFF},
		{0xFF, 0xFF, 0xFF, 0xFF}
};

Runtime::Runtime(const std::string& input, bool debug)
		: debug(debug) {
	std::ifstream file;
	file.exceptions(std::ifstream::badbit);
	file.open(input, std::ifstream::binary);
	file.read((char*)memory, System::BYTE_LIMIT);
	updateFrame();
}

void Runtime::updatePixel(int x, int y, System::Byte value) {
	System::Byte color = 0;
	switch (value) {
	case 0:
		color = memory[COLOR0] >> 4;
		break;
	case 1:
		color = memory[COLOR0] & 0xF;
		break;
	case 2:
		color = memory[COLOR1] >> 4;
		break;
	case 3:
		color = memory[COLOR1] & 0xF;
		break;
	}
	frame[y * System::DISPLAY_SIZE + x] = COLORS[color];
}

void Runtime::updateBlock(System::Byte address) {
	int x = ((address - FRAME) % (System::DISPLAY_SIZE / 2)) * 2;
	int y = ((address - FRAME) / (System::DISPLAY_SIZE / 2)) * 2;
	updatePixel(x, y, memory[address] >> 0x6);
	updatePixel(x + 1, y, (memory[address] >> 0x4) & 0x3);
	updatePixel(x, y + 1, (memory[address] >> 0x2) & 0x3);
	updatePixel(x + 1, y + 1, memory[address] & 0x3);
}

void Runtime::updateFrame() {
	for (System::NByte i = FRAME; i < System::BYTE_LIMIT; ++i) {
		updateBlock(i);
	}
}

System::Byte Runtime::get(System::Byte address) const {
	if (address == INPUT) {
		return system.getInput();
	} else {
		return memory[address];
	}
}

void Runtime::set(System::Byte address, System::Byte value) {
	memory[address] = value;
	if (address == COLOR0 || address == COLOR1) {
		updateFrame();
	} else if (address >= FRAME) {
		updateBlock(address);
	}
}

void Runtime::debugOutput(const std::string& opcode, System::Byte operand) {
	if (debug && (dgoto == -1 || I - 1 == dgoto)) {
		for (int i = 0; i < 0x10; ++i) {
			std::printf("----");
		}

		std::printf("\n\n    ");
		for (int i = 0; i < 0x10; ++i) {
			std::printf(" @_%X", i);
		}
		for (int y = 0; y < 0x10; ++y) {
			std::printf("\n@%X_:", y);
			for (int x = 0; x < 0x10; ++x) {
				std::printf(" #%02X", get(y * 0x10 + x));
			}
		}

		std::printf("\n\nI = #%02X, A = #%02X, M = #%02X, E = #%X\n", I - 1, A, M, (int)E);
		std::printf("%s #%X\n", opcode.c_str(), operand);

		System::Byte rel = operand;
		if (rel & 0x8) {
			rel |= 0xF0;
		}
		rel += I;
		std::printf("\n @0%X = #%02X\n", operand, get(operand));
		std::printf("^@%X0 = #%02X\n", operand, get(operand < 4));
		std::printf("~@%02X = #%02X\n", rel, get(rel));
		std::printf("*@%02X = #%02X\n", M + operand, get(M + operand));
		std::printf("\ngoto: ");
		std::fflush(stdout);

		char c;
		if (isxdigit(c = std::getchar())) {
			dgoto = 0;
			do {
				dgoto <<= 4;
				if (c <= '9') {
					dgoto += c - '0';
				} else if (c <= 'F') {
					dgoto += c - 'A' + 10;
				} else if (c <= 'f') {
					dgoto += c - 'a' + 10;
				}
			} while (isxdigit(c = std::getchar()));
		} else {
			dgoto = -1;
		}
		while (c != '\n') {
			c = std::getchar();
		}
	}
}

void Runtime::run() {
	for (;;) {
		System::Byte ins = get(I++);
		Opcode opcode = static_cast<Opcode>(ins >> 4);
		System::Byte operand = ins & 0xF;
		std::setbuf(stdout, NULL);

		switch (opcode) {
		case Opcode::LDA:
			debugOutput("LDA", operand);
			A = get(M + operand);
			break;
		case Opcode::STA:
			debugOutput("STA", operand);
			set(M + operand, A);
			break;
		case Opcode::LUI:
			debugOutput("LUI", operand);
			A = operand << 4;
			break;
		case Opcode::ADI:
			debugOutput("ADI", operand);
			A += operand;
			break;
		case Opcode::ADD:
			debugOutput("ADD", operand);
			A += get(M + operand);
			break;
		case Opcode::SUB:
			debugOutput("SUB", operand);
			A -= get(M + operand);
			break;
		case Opcode::ORA:
			debugOutput("ORA", operand);
			A |= get(M + operand);
			break;
		case Opcode::AND:
			debugOutput("AND", operand);
			A &= get(M + operand);
			break;
		case Opcode::XOR:
			debugOutput("XOR", operand);
			A ^= get(M + operand);
			break;
		case Opcode::EXT:
			debugOutput("EXT", operand);
			if (operand == 0) {
				A ^= M;
				M ^= A;
				A ^= M;
			} else if (operand < 8) {
				A <<= operand;
			} else if (operand == 8) {
				if (!system.update(frame)) {
					return;
				}
			} else {
				A >>= operand - 8;
			}
			break;
		case Opcode::CMP:
			debugOutput("CMP", operand);
			E = A == get(M + operand);
			break;
		case Opcode::BEQ:
			debugOutput("BEQ", operand);
			if (E) {
				if (operand & 0x8) {
					operand |= 0xF0;
				}
				I += operand;
			}
			break;
		case Opcode::LDM:
			debugOutput("LDM", operand);
			M = get(M + operand);
			break;
		case Opcode::STM:
			debugOutput("STM", operand);
			set(operand, M);
			break;
		case Opcode::LMU:
			debugOutput("LMU", operand);
			M = operand << 4;
			break;
		case Opcode::JLM:
			debugOutput("JLM", operand);
			M += operand;
			I ^= M;
			M ^= I;
			I ^= M;
			break;
		}
	}
}
