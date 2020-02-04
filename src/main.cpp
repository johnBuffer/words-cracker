#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <event_manager.hpp>
#include <vector>


struct TreeNode
{
	TreeNode()
		: terminal(false)
	{
		subs.resize(27u);
		for (TreeNode*& node : subs) {
			node = nullptr;
		}
	}

	void add(const std::string& word)
	{
		const uint8_t last_index = 26u;
		if (word.empty()) {
			terminal = true;
		}
		else {
			const char first_letter = word.front();
			const uint8_t offset = first_letter - 'a';
			if (offset <= last_index) {
				if (!subs[offset]) {
					subs[offset] = new TreeNode();
				}

				subs[offset]->add(word.substr(1u));
			}
		}
	}

	bool isValid(const std::string& word)
	{
		const uint8_t last_index = 26u;
		if (word.empty()) {
			return terminal;
		}
		else {
			const char first_letter = word.front();
			const uint8_t offset = first_letter - 'a';
			if (offset <= last_index) {
				if (!subs[offset]) {
					return false;
				}

				return subs[offset]->isValid(word.substr(1u));
			}
		}
	}

	bool isLetterValid(char c)
	{
		if (c == ' ') {
			return subs[26];
		}
		return subs[c - 'a'];
	}

	bool terminal;
	std::vector<TreeNode*> subs;
};


struct Word
{
	Word()
		: word("")
		, points(0u)
	{
		visited.resize(16u);
		for (uint8_t& v : visited) {
			v = 0u;
		}
	}

	std::vector<uint8_t> visited;
	std::string word;
	uint32_t points;
};


struct Grid
{
	std::vector<char> data;
};


struct GridExplorer
{
	GridExplorer(const Grid& grid_)
		: grid(grid_)
	{}

	std::vector<Word> getWords() const
	{

	}

	void explore(Word& word, uint8_t index)
	{
		
	}

	const Grid& grid;
};


int32_t main()
{
    const uint32_t win_width(1600);
    const uint32_t win_height(900);

	
	std::vector<uint8_t> weights = {
		1,
		2, // B?
		3,
		2,
		1,
		4,
		2,
		4, // H?
		1,
		4, // J?
		8, // K?
		2,
		2,
		1,
		1,
		1, // P
		8,
		1, // R
		1,
		1,
		1,
		2, // V
		10, // W
		10, // X
		10, // Y
		10 // Z
	};
	
	TreeNode tree;

    std::ifstream is("../data/dict.txt");
    std::string str;
    while (getline(is, str)) {
		tree.add(str);
    }

    sf::RenderWindow window(sf::VideoMode(win_width, win_height), "WordsCracker");
	window.setFramerateLimit(60);

	sfev::EventManager event_manager(window);
	event_manager.addEventCallback(sf::Event::Closed, [&](sfev::CstEv) {window.close(); });

    while (window.isOpen()) {
        const sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
		event_manager.processEvents();

        window.clear();
        
        window.display();
    }

    return 0;
}