#pragma once
#include <cstdint>
#include "tree.hpp"
#include "utils.hpp"


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

	void add(uint8_t index, char c)
	{
		word += c;
		current_node = current_node->getSub(c);
		points += weights[c - 'a'];
		visited[index] = 1u;
		indexes.push_back(index);
	}

	const TreeNode* current_node;
	std::vector<uint8_t> visited;
	std::vector<uint8_t> indexes;
	std::string word;
	uint32_t points;
};


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
		// If word valid, we add it to results
		if (word.current_node->terminal) {
			if (word.word.size() > 1u && !isIn<Word>(results, word, [&](const Word& w1, const Word& w2) {return w1.word == w2.word; })) {
				results.push_back(word);
			}
		}
		// Keep searching
		const char c = grid.get(x, y);
		if (word.current_node->isLetterValid(c)) {
			word.add(index, c);
			for (int8_t ox(-1); ox < 2; ++ox) {
				for (int8_t oy(-1); oy < 2; ++oy) {
					if (ox != x || oy != y) {
						const int8_t nx = x + ox;
						const int8_t ny = y + oy;
						if (nx >= 0 && nx < 4 && ny >= 0 && ny < 4) {
							const int8_t next_index = nx + ny * 4;
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

	const Grid& grid;
	const TreeNode& tree;
};
