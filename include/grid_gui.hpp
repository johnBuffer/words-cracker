#pragma once

#include <SFML/Graphics.hpp>
#include "grid.hpp"


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
		if (!done) {
			current_dist += dt;
			if (current_dist >= dist) {
				done = true;
				current_dist = dist;
			}
		}
	}

	void draw(sf::RenderTarget& target) const
	{
		const float circle_radius = 24.0f;
		sf::CircleShape circle(circle_radius, circle_radius);
		circle.setOrigin(circle_radius, circle_radius);
		circle.setPosition(start_position);
		target.draw(circle);

		circle.setPosition(start_position + current_dist * direction);
		target.draw(circle);
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
	{
		font.loadFromFile("font.ttf");
	}

	void addChar(const char c)
	{
		if (current_state == Building) {
			grid.data.push_back(c);
			if (grid.data.size() == 16u) {
				current_state = Displaying;
				words = GridExplorer(grid, tree).getWords();
				std::sort(words.begin(), words.end(), [&](const Word& w1, const Word& w2) {return w1.points > w2.points; });
			}
		}
	}

	sf::Vector2f indexToCoord(uint8_t index)
	{
		const float x = index % 4u;
		const float y = index / 4u;

		return sf::Vector2f(margin + x * (side_size + padding) + side_size * 0.5f, margin + y * (side_size + padding) + side_size * 0.5f);
	}

	void update(float dt)
	{
		if (paths.empty()) {
			current_letter = 0u;
			const Word& next_word = words[current_word];
			const uint32_t first_index = next_word.indexes[0];
			const uint32_t second_index = next_word.indexes[1];
			paths.emplace_back(indexToCoord(first_index), indexToCoord(second_index));
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

				const char c = grid.get(x, y);
				if (c) {
					sf::Text text;
					text.setFont(font);
					text.setCharacterSize(48);
					text.setFillColor(sf::Color::Black);
					text.setString(c - 32u);
					const auto rect = text.getGlobalBounds();
					text.setOrigin(rect.width * 0.5f, rect.height * 0.65f);
					text.setPosition(cx + side_size * 0.5f, cy + side_size * 0.5f);
					window.draw(text);
				}
			}
		}

		for (const Path& p : paths) {
			p.draw(window);
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
