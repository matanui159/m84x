#include "System.hpp"

System::System()
		: window(sf::VideoMode(512, 512), "M84X", sf::Style::Titlebar | sf::Style::Close) {
	texture.create(DISPLAY_SIZE, DISPLAY_SIZE);
	sprite.setTexture(texture, true);
	window.setView(sf::View(sf::FloatRect(0, 0, DISPLAY_SIZE, DISPLAY_SIZE)));
	window.setFramerateLimit(10);
}

System::Byte System::getInput() const {
	Byte input = dir;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		input |= 0x80;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		input |= 0x40;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		input |= 0x20;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		input |= 0x10;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
		input |= 0x02;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
		input |= 0x01;
	}
	return input;
}

bool System::update(Color* frame) {
	texture.update((Byte*)frame);
	window.draw(sprite);
	window.display();

	sf::Event event;
	while (window.pollEvent(event)) {
		switch (event.type) {
		case sf::Event::Closed:
			window.close();
			break;
		case sf::Event::KeyPressed:
			switch (event.key.code) {
			case sf::Keyboard::Left:
				dir = 0x00;
				break;
			case sf::Keyboard::Up:
				dir = 0x04;
				break;
			case sf::Keyboard::Down:
				dir = 0x08;
				break;
			case sf::Keyboard::Right:
				dir = 0x0C;
				break;
			}
			break;
		}
	}

	return window.isOpen();
}
