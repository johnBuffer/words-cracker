#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <event_manager.hpp>
#include <vector>

#include "grid.hpp"
#include "grid_gui.hpp"


int32_t main()
{
	const uint32_t win_width(1000);
	const uint32_t win_height(530);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "ZTyper2", sf::Style::Default, settings);
	window.setFramerateLimit(60);

	// Generate dict tree
	std::cout << "Loading Dict..." << std::endl;
	TreeNode tree;
	std::ifstream is("../data/dict.txt");
	std::string str;
	while (getline(is, str)) {
		tree.add(str);
	}

	// Create grid
	Grid grid;
	GridGui gui(window, grid, tree);

	sfev::EventManager event_manager(window);
	event_manager.addEventCallback(sf::Event::Closed, [&](sfev::CstEv) {window.close(); });
	event_manager.addKeyReleasedCallback(sf::Keyboard::Space, [&](sfev::CstEv) {gui.nextWord(); });
	event_manager.addKeyReleasedCallback(sf::Keyboard::Tab, [&](sfev::CstEv) {gui.reset(); });
	event_manager.addKeyReleasedCallback(sf::Keyboard::BackSpace, [&](sfev::CstEv) {gui.correct(); });
	event_manager.addEventCallback(sf::Event::TextEntered, [&](sfev::CstEv ev) {gui.addChar(ev.text.unicode); });

	while (window.isOpen()) {
		const sf::Vector2i mouse_position = sf::Mouse::getPosition(window);

		event_manager.processEvents();

		gui.update(0.016f);

		window.clear();

		gui.render();

		window.display();
	}

	return 0;
}