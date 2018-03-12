#ifndef ASSEMBLER_HPP_
#define ASSEMBLER_HPP_
#include "System.hpp"
#include "Opcode.hpp"
#include "Lexer.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

class Assembler {
	Lexer input;
	std::ofstream output;

	std::unordered_map<std::string, Opcode> opcodes;
	std::unordered_map<std::string, System::Byte> labels;

	struct Incomplete {
		int line;
		System::Byte address;
		Opcode opcode;
		std::string label;
		char mode;
	};
	std::vector<Incomplete> incomplete;

	System::NByte tell();
	void seek(System::Byte address);

	void unexpected(const Lexer::Token& token) const;
	void addLabel(const std::string& name, System::Byte value);
	void write(Opcode opcode, System::NByte operand, const std::string& label, char mode);

public:
	Assembler(const std::string& in, const std::string& out);
	void build();
};

#endif
