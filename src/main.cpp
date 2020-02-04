#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <event_manager.hpp>
#include <vector>

const std::vector<uint8_t> weights = {
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
		const char first_letter = word.front();
		const uint8_t offset = first_letter - 'a';
		if (offset <= last_index) {
			if (!subs[offset]) {
				subs[offset] = new TreeNode();
			}
			if (word.size() == 1u) {
				subs[offset]->terminal = true;
			}
			else {
				subs[offset]->add(word.substr(1u));
			}
		}
	}

	bool isValid(const std::string& word) const
	{
		const uint8_t last_index = 26u;
		const char first_letter = word.front();
		const uint8_t offset = first_letter - 'a';
		if (offset <= last_index) {
			const TreeNode* next_node = subs[offset];
			if (next_node) {
				if (word.size() == 1u) {
					if (next_node->terminal) {
						return true;
					}
				}
				else {
					return next_node->isValid(word.substr(1u));
				}
			}
			else {
				return false;
			}
		}
	}

	const TreeNode* getSub(const char c) const
	{
		const uint8_t offset = c - 'a';
		return subs[offset];
	}

	bool isLetterValid(char c) const
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
	Word(const TreeNode& node)
		: word("")
		, points(0u)
		, current_node(&node)
	{
		visited.resize(16u);
		for (uint8_t& v : visited) {
			v = 0u;
		}
	}

	const TreeNode* current_node;
	std::vector<uint8_t> visited;
	std::string word;
	uint32_t points;
};


struct Grid
{
	const char get(uint8_t i) const
	{
		if (i < data.size()) {
			return data[i];
		}
		return 0u;
	}

	const char get(int8_t x, int8_t y) const
	{
		if (x < 0 || y < 0) {
			return 0u;
		}
		const uint8_t index = y * 4u + x;
		return get(index);
	}

	std::vector<char> data;
};


bool isIn(const std::vector<Word>& v, const std::string& w)
{
	for (const Word& s : v) {
		if (s.word == w) {
			return true;
		}
	}
	return false;
}


struct GridExplorer
{
	GridExplorer(const Grid& grid_, const TreeNode& tree_)
		: grid(grid_)
		, tree(tree_)
	{}

	std::vector<Word> getWords() const
	{
		std::vector<Word> results;
		const uint8_t side_cells = 4u;
		for (uint8_t x(0); x < side_cells; ++x) {
			for (uint8_t y(0); y < side_cells; ++y) {
				Word word(tree);
				explore(word, x, y, results);
			}
		}

		return results;
	}

	void explore(Word word, int8_t x, int8_t y, std::vector<Word>& results) const
	{
		const int8_t index = x + y * 4;
		if (index >= 0 && index < 16) {
			// If word valid, we add it to results
			if (word.current_node->terminal) {
				if (word.word.size() > 1u && !isIn(results, word.word)) {
					results.push_back(word);
				}
			}
			// Keep searching
			const char c = grid.get(x, y);
			if (word.current_node->isLetterValid(c)) {
				word.word += c;
				word.current_node = word.current_node->getSub(c);
				word.points += weights[c - 'a'];
				word.visited[index] = 1u;
				for (int8_t ox(-1); ox < 2; ++ox) {
					for (int8_t oy(-1); oy < 2; ++oy) {
						if (ox != x || oy != y) {
							const int8_t nx = x + ox;
							const int8_t ny = y + oy;
							if (nx >= 0 && nx < 4 && ny >= 0 && ny < 4) {
								const int8_t next_index = nx + ny * 4;
								if (next_index >= 0 && next_index < 16) {
									if (!word.visited[next_index]) {
										Word next_word = word;
										explore(next_word, nx, ny, results);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	const Grid& grid;
	const TreeNode& tree;
};


int32_t main()
{
    const uint32_t win_width(1600);
    const uint32_t win_height(900);
	
	// Generate dict tree
	std::cout << "Loading Dict..." << std::endl;
	TreeNode tree;
    std::ifstream is("../data/dict.txt");
    std::string str;
    while (getline(is, str)) {
		tree.add(str);
    }

	// Create grid
	std::string raw_grid;
	std::cout << "GRID: ";
	std::cin >> raw_grid;
	Grid grid;
	//grid.data = { 'a', 'c', 'i', 'e', 'n', 'u', 'r', 'u', 'i', 'a', 'p', 't', 'l', 'm', 'r', 'a' };
	for (char c : raw_grid) {
		grid.data.push_back(c);
	}

	// Set up explorer
	std::cout << "Starting exploration..." << std::endl;
	GridExplorer explorer(grid, tree);
	auto words = explorer.getWords();
	std::cout << words.size() << " words found" << std::endl;

	std::sort(words.begin(), words.end(), [&](const Word& w1, const Word& w2) {return w1.points > w2.points; });
	const uint32_t count = std::min(words.size(), std::size_t(50));
	for (uint32_t i(0); i < count; ++i) {
		std::cout << words[i].points << " " << words[i].word << std::endl;
	}

    return 0;
}