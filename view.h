#include <SFML/Graphics.hpp>
using namespace sf;
 
View view;
View setPlayerCoordinateForView(float x,float y) { 
	float tempX = x, tempY = y;
	view.setCenter(tempX, tempY); //������ �� �������
	return view;
}