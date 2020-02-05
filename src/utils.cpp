#include "utils.hpp"
#include <cmath>


float getAngle(const sf::Vector2f & v)
{
	const float dot = v.x;
	const float det = -v.y;
	return atan2(det, dot);
}
