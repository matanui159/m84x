#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_
#include <SFML/Graphics.hpp>

class System {
	sf::RenderWindow window;
	sf::Texture texture;
	sf::Sprite sprite;

public:
	enum Constants {
		BYTE_MAX = 255,

		// the largest array a byte can index
		BYTE_LIMIT = BYTE_MAX + 1,

		DISPLAY_SIZE = 16
	};

	typedef unsigned char Byte;

	// a type that can store between -1 and BYTE_MAX * 2
	typedef short NByte;

	struct Color {
		Byte red;
		Byte green;
		Byte blue;
		Byte alpha;
	};

	System();
	Byte getInput() const;
	bool update(Color* frame);

private:
	Byte dir = 0x00;
};

#endif
