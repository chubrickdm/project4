#pragma once
#include "system.h"

class Body : public System{ //класс который служит основой для всех других классов с графикой
protected:
	int w, h; //ширина, высота
	RectangleShape shape; //текстура
	int wTexture, hTexture; //что бы масштабировать текстуру, нужно знать какой именно кусок тайла масштабировать
public:
	int x, y; //координаты
	String name; //имя
public:
	Body (Texture *tmpTexture, String Name, int X, int Y, int W, int H, int WTexture, int HTexture){ //конструктор с текстурой
		x = X; y = Y; w = W; h = H; name = Name;
		wTexture = WTexture; hTexture = HTexture;
		shape.setSize (Vector2f ((float) w, (float) h));
		shape.setPosition ((float) x, (float) y);
		shape.setTexture (tmpTexture);
		shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));	
	}

	Body (String Name, int X, int Y, int W, int H, int WTexture, int HTexture){ //конструктор без текстуры
		x = X; y = Y; w = W; h = H; name = Name;
		wTexture = WTexture; hTexture = HTexture;
	}

	virtual void draw () = 0;
};