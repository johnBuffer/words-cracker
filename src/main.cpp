#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <event_manager.hpp>
#include <vector>

#include "grid.hpp"
#include "grid_gui.hpp"


int32_t main()
{
	const uint32_t win_width(1600);
	const uint32_t win_height(900);

	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "ZTyper2");
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
	event_manager.addKeyReleasedCallback(sf::Keyboard::Space, [&](sfev::CstEv) {});
	event_manager.addEventCallback(sf::Event::TextEntered, [&](sfev::CstEv ev) {gui.addChar(ev.text.unicode); });

	// Set up explorer
	std::cout << "Starting exploration..." << std::endl;
	/*GridExplorer explorer(grid, tree);
	auto words = explorer.getWords();
	std::cout << words.size() << " words found" << std::endl;

	std::sort(words.begin(), words.end(), [&](const Word& w1, const Word& w2) {return w1.points > w2.points; });
	const uint64_t count = std::min(words.size(), std::size_t(50));
	for (uint64_t i(0); i < count; ++i) {
		std::cout << words[i].points << " " << words[i].word << std::endl;
	}*/

	while (window.isOpen()) {
		const sf::Vector2i mouse_position = sf::Mouse::getPosition(window);

		event_manager.processEvents();

		window.clear();

		gui.render();

		window.display();
	}

	return 0;
}