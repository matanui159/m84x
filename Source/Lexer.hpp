#ifndef LEXER_HPP_
#define LEXER_HPP_
#include "System.hpp"
#include <fstream>
#include <string>

class Lexer {
	std::ifstream input;
	short peeked = -1;
	bool cr = false;
	int line = 1;

	char peekChar();
	char nextChar();
	bool isStringChar(char c) const;

public:
	class Token {
		int type;
		std::string string;
		System::Byte number;
		char symbol;

		enum Type {
			NONE,
			STRING,
			NUMBER,
			SYMBOL
		};

	public:
		Token();
		Token(const std::string& string);
		Token(System::Byte number);
		Token(char symbol, bool);

		bool isString() const;
		bool isNumber() const;
		bool isSymbol() const;

		const std::string& getString() const;
		System::Byte getNumber() const;
		char getSymbol() const;
	};

	Lexer(const std::string& input);
	Token nextToken();

	int getLine();
	void setLine(int line);
	std::runtime_error error(const std::string& what) const;
};

#endif
