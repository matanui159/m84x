#include "Lexer.hpp"
#include <sstream>
#include <locale>

// Lexer::Token
Lexer::Token::Token()
		: type(NONE) {
}

Lexer::Token::Token(const std::string& string)
		: type(STRING), string(string) {
}

Lexer::Token::Token(System::Byte number)
		: type(NUMBER), number(number) {
}

Lexer::Token::Token(char symbol, bool)
		: type(SYMBOL), symbol(symbol) {
}

bool Lexer::Token::isString() const {
	return type == STRING;
}

bool Lexer::Token::isNumber() const {
	return type == NUMBER;
}

bool Lexer::Token::isSymbol() const {
	return type == SYMBOL;
}

const std::string& Lexer::Token::getString() const {
	return string;
}

System::Byte Lexer::Token::getNumber() const {
	return number;
}

char Lexer::Token::getSymbol() const {
	return symbol;
}



// Lexer
Lexer::Lexer(const std::string& input_) {
	input.exceptions(std::ifstream::badbit);
	input.open(input_, std::ifstream::binary);
}

char Lexer::peekChar() {
	if (peeked == -1) {
		int c = input.get();
		if (c == '\n') {
			if (cr) {
				c = input.get();
			}
			cr = false;
		}
		switch (c) {
		case std::ifstream::traits_type::eof():
			peeked = '\0';
			break;
		case '\r':
			cr = true;
			peeked = '\n';
			break;
		default:
			peeked = std::toupper(c);
			break;
		}
	}
	return peeked;
}

char Lexer::nextChar() {
	char c = peekChar();
	peeked = -1;
	return c;
}

bool Lexer::isStringChar(char c) const {
	return std::isalnum(c) || c == '_' || c == '.';
}

Lexer::Token Lexer::nextToken() {
	char c;
	while (std::isblank(c = peekChar())) {
		nextChar();
	}
	if (c == ';') {
		while ((c = peekChar()) != '\n' && c != '\0') {
			nextChar();
		}
	}

	if (isStringChar(c)) {
		std::ostringstream string;
		do {
			string << nextChar();
		} while (isStringChar(c = peekChar()));
		return Token(string.str());
	}

	if (c == '#') {
		nextChar();
		System::NByte number = 0;
		while (isxdigit(c = peekChar())) {
			nextChar();
			number <<= 4;
			if (c <= '9') {
				number += c - '0';
			} else if (c <= 'F') {
				number += c - 'A' + 10;
			} else if (c <= 'f') {
				number += c - 'a' + 10;
			}
			if (number > System::BYTE_MAX) {
				throw error("number too large");
			}
		}
		return Token(number);
	}

	const std::string symbols = ":=^~*\n";
	if (symbols.find(c) != std::string::npos || c == '\0') {
		if (c == '\n') {
			++line;
		}
		return Token(nextChar(), false);
	}

	throw error("unknown character");
}

int Lexer::getLine() {
	return line;
}

void Lexer::setLine(int line) {
	this->line = line;
}

std::runtime_error Lexer::error(const std::string& what) const {
	std::ostringstream string;
	string << line << ": " << what;
	return std::runtime_error(string.str());
}
