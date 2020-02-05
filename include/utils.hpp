#pragma once
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>


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


template<typename T>
bool isIn(const std::vector<T>& v, const T& elem, std::function<bool(const T&, const T&)> comp)
{
	for (const T& s : v) {
		if (comp(s, elem)) {
			return true;
		}
	}
	return false;
}


float getAngle(const sf::Vector2f& v);
