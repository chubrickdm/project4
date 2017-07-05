#pragma once
#include <SFML/Graphics.hpp>
 
sf::View view;
sf::View setCoordinateForView (float x, float y) { 
	float tempX = x, tempY = y;
	view.setCenter (tempX, tempY); //следим за игроком
	return view;
}