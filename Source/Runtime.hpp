#ifndef RUNTIME_HPP_
#define RUNTIME_HPP_
#include "System.hpp"
#include <string>

class Runtime {
	static const System::Color COLORS[16];

	System system;
	System::Byte memory[System::BYTE_LIMIT];
	System::Color frame[System::DISPLAY_SIZE * System::DISPLAY_SIZE];
	bool debug;
	System::NByte dgoto = -1;

	System::Byte I = 0;
	System::Byte A = 0;
	System::Byte M = 0;
	bool E = false;

	void updatePixel(int x, int y, System::Byte value);
	void updateBlock(System::Byte address);
	void updateFrame();
	System::Byte get(System::Byte address) const;
	void set(System::Byte address, System::Byte value);
	void debugOutput(const std::string& opcode, System::Byte operand);

public:
	enum Address {
		COLOR0,
		COLOR1,
		INPUT,
		FRAME = 0xC0
	};

	Runtime(const std::string& input, bool debug);
	void run();
};

#endif
