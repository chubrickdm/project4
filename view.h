#include <SFML/Graphics.hpp>
using namespace sf;
 
View view;
View setCoordinateForView (float x, float y) { 
	float tempX = x, tempY = y;
	view.setCenter (tempX, tempY); //следим за игроком
	return view;
}