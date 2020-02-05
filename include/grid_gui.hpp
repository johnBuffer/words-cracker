#pragma once

#include <SFML/Graphics.hpp>
#include "grid.hpp"
#include <iostream>


struct Path
{
	Path(const sf::Vector2f& start, const sf::Vector2f& end)
		: current_dist(0.0f)
		, start_position(start)
		, done(false)
	{
		const sf::Vector2f v = end - start;
		dist = sqrt(v.x*v.x + v.y*v.y);
		direction.x = v.x / dist;
		direction.y = v.y / dist;
	}

	void update(float dt)
	{
		const float speed = 1000.0f;
		if (!done) {
			current_dist += speed * dt;
			if (current_dist >= dist) {
				done = true;
				current_dist = dist;
			}
		}
	}

	void draw(sf::RenderTarget& target) const
	{
		const float circle_radius = 32.0f;
		sf::CircleShape circle(circle_radius, circle_radius);
		circle.setOrigin(circle_radius, circle_radius);
		circle.setPosition(start_position);
		circle.setFillColor(sf::Color::Red);
		target.draw(circle);

		circle.setPosition(start_position + current_dist * direction);
		target.draw(circle);

		sf::RectangleShape line(sf::Vector2f(current_dist, 2 * circle_radius));
		line.setOrigin(0, circle_radius);
		line.setPosition(start_position);
		line.setFillColor(sf::Color::Red);
		line.setRotation(-getAngle(direction) * 57.2958f);
		target.draw(line);
	}

	sf::Vector2f start_position;
	sf::Vector2f direction;
	float current_dist;
	float dist;
	bool done;
};


struct GridGui
{
	enum State
	{
		Building,
		Displaying
	};

	GridGui(sf::RenderWindow& window_, Grid& grid_, TreeNode& tree_)
		: window(window_)
		, grid(grid_)
		, tree(tree_)
		, current_state(Building)
		, current_word(0)
		, current_letter(0)
	{
		font.loadFromFile("font.ttf");
	}

	void addChar(const char c)
	{
		if (c >= 'a' && c <= 'z') {
			if (current_state == Building) {
				grid.data.push_back(c);
				if (grid.data.size() == 16u) {
					std::cout << "Starting exploration..." << std::endl;
					current_state = Displaying;
					words = GridExplorer(grid, tree).getWords();
					std::sort(words.begin(), words.end(), [&](const Word& w1, const Word& w2) {return w1.points > w2.points; });
					std::cout << "Done." << std::endl;
				}
			}
		}
	}

	void nextWord()
	{
		++current_word;
		paths.clear();
		current_letter = 0;
		std::cout << words[current_word].word << std::endl;
	}

	sf::Vector2f indexToCoord(uint8_t index)
	{
		const float x = index % 4u;
		const float y = index / 4u;

		return sf::Vector2f(margin + x * (side_size + padding) + side_size * 0.5f, margin + y * (side_size + padding) + side_size * 0.5f);
	}

	void addLetterToPath()
	{
		if (current_word < words.size()) {
			const Word& word = words[current_word];
			if (current_letter < word.word.size() - 1) {
				const uint32_t first_index = word.indexes[current_letter];
				const uint32_t second_index = word.indexes[current_letter + 1];
				paths.emplace_back(indexToCoord(first_index), indexToCoord(second_index));
				++current_letter;
			}
		}
	}

	void update(float dt)
	{
		if (current_state == Displaying) {
			if (paths.empty()) {
				addLetterToPath();
			}
			else if (paths.back().done) {
				addLetterToPath();
			}
			else {
				paths.back().update(dt);
			}
		}
	}

	void render() const
	{
		sf::RectangleShape shape(sf::Vector2f(side_size, side_size));
		
		for (uint8_t x(0); x < 4u; ++x) {
			for (uint8_t y(0); y < 4u; ++y) {
				const float cx = margin + x * (side_size + padding);
				const float cy = margin + y * (side_size + padding);
				shape.setPosition(cx, cy);
				window.draw(shape);
			}
		}

		if (words.size()) {
			sf::Text text;
			text.setFont(font);
			text.setCharacterSize(64);
			text.setFillColor(sf::Color::White);
			text.setString(words[current_word].word);
			const auto rect = text.getGlobalBounds();
			text.setPosition(500, margin);
			window.draw(text);
		}

		for (const Path& p : paths) {
			p.draw(window);
		}

		for (uint8_t x(0); x < 4u; ++x) {
			for (uint8_t y(0); y < 4u; ++y) {
				const float cx = margin + x * (side_size + padding);
				const float cy = margin + y * (side_size + padding);
				const char c = grid.get(x, y);
				if (c) {
					sf::Text text;
					text.setFont(font);
					text.setCharacterSize(48);
					text.setFillColor(sf::Color::Black);
					text.setString(c - 32u);
					const auto rect = text.getGlobalBounds();
					text.setOrigin(rect.width * 0.5f, rect.height * 0.8f);
					text.setPosition(cx + side_size * 0.5f, cy + side_size * 0.5f);
					window.draw(text);
				}
			}
		}
	}

	void reset()
	{
		current_state = Building;
		current_word = 0;
		current_letter = 0;
		paths.clear();
		words.clear();
		grid.data.clear();
	}

	void correct()
	{
		if (grid.data.size()) {
			grid.data.pop_back();
		}
	}

	const float side_size = 100.0f;
	const float margin = 50.0f;
	const float padding = 10.0f;
	const TreeNode& tree;

	State current_state;
	sf::RenderWindow& window;
	Grid& grid;
	std::vector<Word> words;
	sf::Font font;
	std::vector<Path> paths;
	uint32_t current_word;
	uint32_t current_letter;
};
