#pragma once
#include <cstdint>
#include <string>
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
		const char first_letter = word.front();
		const uint8_t offset = first_letter - 'a';
		if (offset <= last_index) {
			TreeNode*& next = subs[offset];
			if (!next) {
				next = new TreeNode();
			}
			if (word.size() == 1u) {
				next->terminal = true;
			}
			else {
				next->add(word.substr(1u));
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
		}

		return false;
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
