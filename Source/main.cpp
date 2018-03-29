#include "Assembler.hpp"
#include "Runtime.hpp"
#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>
#include <cstdio>

static void help() {
	std::cout << std::endl;
	std::cout << "m84x [flags] [arguments]" << std::endl;
	std::cout << "\ta [file]: assembles the file" << std::endl;
	std::cout << "\to [file]: sets output file for assembly" << std::endl;
	std::cout << "\tr [file]: runs the file" << std::endl;
	std::cout << "\td: enables debug during running" << std::endl;
	std::cout << "\th: shows help" << std::endl;
	std::cout << std::endl;
	std::cout << "output file for assembly follows this order:" << std::endl;
	std::cout << "\tif the o option is provided, it will use that" << std::endl;
	std::cout << "\telse, if the r option is provided, it will use that" << std::endl;
	std::cout << "\telse, it will be the input file suffixed with .bin" << std::endl;
	std::cout << std::endl;
	std::cout << "example: m84x ard input.asm output.bin" << std::endl;
	std::cout << "\tthis assembles input.asm to output.bin," << std::endl;
	std::cout << "\tthen runs output.bin," << std::endl;
	std::cout << "\twith debug enabled" << std::endl;
	std::cout << std::endl;
}

static const char* nextArg(int argc, char** argv, int& index) {
	if (index == argc) {
		std::cerr << "not enough arguments" << std::endl;
		help();
		std::exit(EXIT_FAILURE);
	}
	return argv[(index)++];
}

int main(int argc, char* argv[]) {
	std::string input, output, run;
	bool a = false;
	bool o = false;
	bool r = false;
	bool d = false;

	int index = 1;
	for (const char* flags = nextArg(argc, argv, index); *flags != '\0'; ++flags) {
		switch (*flags) {
		case 'a':
			a = true;
			input = nextArg(argc, argv, index);
			if (!o && !r) {
				output = input + ".bin";
			}
			break;
		case 'o':
			o = true;
			output = nextArg(argc, argv, index);
			break;
		case 'r':
			r = true;
			run = nextArg(argc, argv, index);
			if (!o) {
				output = run;
			}
			break;
		case 'd':
			d = true;
			break;
		case 'h':
			help();
			break;
		default:
			std::cerr << "unknown flag " << *flags << std::endl;
			help();
			exit(EXIT_FAILURE);
		}
	}

	if (a) {
		try {
			Assembler assembler(input, output);
			assembler.build();
		} catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (r) {
		Runtime runtime(run, d);
		runtime.run();
	}
}
