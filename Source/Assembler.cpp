#include "Assembler.hpp"
#include "Opcode.hpp"
#include <iostream>
#include <cstdio>

Assembler::Assembler(const std::string& input, const std::string& output_)
		: input(input) {
	output.exceptions(std::ofstream::badbit);
	output.open(output_, std::ofstream::binary);
	opcodes.reserve(static_cast<int>(Opcode::LIMIT));

	opcodes["LDA"] = Opcode::LDA;
	opcodes["STA"] = Opcode::STA;
	opcodes["LUI"] = Opcode::LUI;
	opcodes["ADI"] = Opcode::ADI;

	opcodes["ADD"] = Opcode::ADD;
	opcodes["SUB"] = Opcode::SUB;
	opcodes["ORA"] = Opcode::ORA;
	opcodes["AND"] = Opcode::AND;

	opcodes["XOR"] = Opcode::XOR;
	opcodes["EXT"] = Opcode::EXT;
	opcodes["CMP"] = Opcode::CMP;
	opcodes["BEQ"] = Opcode::BEQ;

	opcodes["LDM"] = Opcode::LDM;
	opcodes["STM"] = Opcode::STM;
	opcodes["LMU"] = Opcode::LMU;
	opcodes["JLM"] = Opcode::JLM;

	opcodes["SWP"] = Opcode::SWP;
	opcodes["SHL"] = Opcode::SHL;
	opcodes["HLT"] = Opcode::HLT;
	opcodes["SHR"] = Opcode::SHR;

	opcodes["SET"] = Opcode::SET;
	opcodes["DAT"] = Opcode::DAT;
	opcodes["PAL"] = Opcode::PAL;
}

System::NByte Assembler::tell() {
	return output.tellp();
}

void Assembler::seek(System::Byte address) {
	output.seekp(address, std::ofstream::beg);
}

void Assembler::unexpected(const Lexer::Token& token) const {
	if (token.isString()) {
		throw input.error("unexpected string");
	}
	if (token.isNumber()) {
		throw input.error("unexpected number");
	}
	if (token.isSymbol()) {
		switch (token.getSymbol()) {
		case '\n':
			throw input.error("unexpected newline");
		case '\0':
			throw input.error("unexpected end of file");
		default:
			throw input.error("unexpected symbol");
		}
	}
}

void Assembler::addLabel(const std::string& name, System::Byte value) {
	labels[name] = value;
	std::printf("%s = #%02X\n", name.c_str(), value);

	int line = input.getLine();
	System::Byte address = tell();
	for (auto it = incomplete.begin(); it != incomplete.end();) {
		if (it->label == name) {
			input.setLine(it->line);
			seek(it->address);
			write(it->opcode, value, "", it->mode);
			it = incomplete.erase(it);
		} else {
			++it;
		}
	}
	input.setLine(line);
	seek(address);
}

void Assembler::write(Opcode opcode, System::NByte operand, const std::string& label, char mode) {
	if (operand == -1) {
		if (labels.find(label) == labels.end()) {
			Incomplete inc;
			inc.line = input.getLine();
			inc.address = tell();
			inc.opcode = opcode;
			inc.label = label;
			inc.mode = mode;
			incomplete.push_back(inc);
			output.put(0);
			return;
		} else {
			operand = labels.at(label);
		}
	}

	if (tell() == System::BYTE_LIMIT) {
		throw input.error("program too large");
	}

	switch (mode) {
	case '~':
		operand -= tell() + 1;
		if (operand < -8 || operand > 7) {
			throw input.error("difference too large");
		}
	case '\0':
		operand &= 0xF;
		break;
	case '^':
		operand >>= 4;
		break;
	}

	switch (opcode) {
	case Opcode::SWP:
		opcode = Opcode::EXT;
		if (operand != 0) {
			throw input.error("number too large");
		}
		break;
	case Opcode::SHL:
		opcode = Opcode::EXT;
		if (operand < 1 || operand > 7) {
			throw input.error("number too large");
		}
		break;
	case Opcode::HLT:
		opcode = Opcode::EXT;
		if (operand != 0) {
			throw input.error("number too large");
		}
		operand = 8;
		break;
	case Opcode::SHR:
		opcode = Opcode::EXT;
		if (operand < 1 || operand > 7) {
			throw input.error("number too large");
		}
		operand += 8;
		break;
	}

	output.put(static_cast<System::Byte>(opcode) << 4 | operand);
}

void Assembler::build() {
	Lexer::Token token;
	do {
		if ((token = input.nextToken()).isString()) {
			Lexer::Token token2;
			if ((token2 = input.nextToken()).isSymbol()) {
				if (token2.getSymbol() == ':') {
					addLabel(token.getString(), tell());
					continue;
				} else if (token2.getSymbol() == '=') {
					if ((token2 = input.nextToken()).isNumber()) {
						addLabel(token.getString(), token2.getNumber());
						token = input.nextToken();
						goto end;
					}
					unexpected(token);
				}
			}

			if (opcodes.find(token.getString()) == opcodes.end()) {
				throw input.error("unknown instruction");
			}
			Opcode opcode = opcodes.at(token.getString());

			char mode = '\0';
			if (token2.isSymbol()) {
				if (token2.getSymbol() == '^' || token2.getSymbol() == '~' || token2.getSymbol() == '*') {
					mode = token2.getSymbol();
				} else {
					unexpected(token2);
				}
				token2 = input.nextToken();
			}

			int operand = -1;
			std::string label = "";
			if (token2.isString()) {
				label = token2.getString();
			} else if (token2.isNumber()) {
				operand = token2.getNumber();
			} else {
				unexpected(token2);
			}


			switch (opcode) {
			case Opcode::SET:
				if (mode == '\0' || mode == '~') {
					write(Opcode::LUI, 0, "", '\0');
				} else {
					write(Opcode::LUI, operand, label, '^');
				}
				if (mode != '^') {
					if (mode == '*') {
						mode = '\0';
					}
					write(Opcode::ADI, operand, label, mode);
				}
				break;
			case Opcode::DAT:
				write(static_cast<Opcode>(0), operand, label, mode);
				break;
			case Opcode::PAL:
				while (tell() % 16 != 0) {
					write(static_cast<Opcode>(0), operand, label, mode);
				}
				break;
			default:
				if (mode == '*') {
					write(Opcode::LMU, operand, label, '^');
					mode = '\0';
				}
				write(opcode, operand, label, mode);
				break;
			}

			token = input.nextToken();
		}

	end:
		if (!token.isSymbol() || (token.getSymbol() != '\n' && token.getSymbol() != '\0')) {
			unexpected(token);
		}
	} while (token.getSymbol() != '\0');

	if (!incomplete.empty()) {
		input.setLine(incomplete.front().line);
		throw input.error("unknown label");
	}

	std::cout << "build complete (" << tell() << " bytes used)" << std::endl;
	while (tell() < System::BYTE_LIMIT) {
		output.put(0);
	}
}
