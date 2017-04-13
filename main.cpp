#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "forMcText.h"
//#include <string.h>
//#include "algorithm.h"
using namespace std;
using namespace sf;

enum StateList {menu, mode, admin, player, backToMenu, setting, exitt};

#define W_WIN 700 //ширина окна
#define H_WIN 500 //высота окна
#define EDGE 20 //размер одной клетки
#define INDENTATION 60 //отступ поля от границ окна по вертикали и горизонатли в обе стороны
#define NUM_H_LINE (H_WIN - 2 * INDENTATION + EDGE) / EDGE //количество горизонтальных прямых, которые создают поле
#define NUM_V_LINE (W_WIN - 2 * INDENTATION + EDGE) / EDGE //количество вертикальных прямых, которые создают поле

class Wall{
public:
	int x, y;
	int w, h; //ширина, высота
	bool drawThis;
	Texture texture; //текстура
	Sprite sprite; //спрайт
	String name; //имя
public:
	Wall (Image &image, String Name, int X, int Y, int W, int H){ //конструктор с именем
		x = X; y = Y; w = W; h = H; name = Name; drawThis = true;
		texture.loadFromImage (image);
		sprite.setTexture (texture);
		sprite.setTextureRect (IntRect (0, 0, w, h));
	}

	Wall (Image &image, int X, int Y, int W, int H){ //конструктор без имени
		x = X; y = Y; w = W; h = H; name = "0"; drawThis = true;
		texture.loadFromImage (image);
		sprite.setTexture (texture);
		sprite.setTextureRect (IntRect (0, 0, w, h));
		sprite.setPosition (x, y);
	}

	FloatRect getRect (){ //функция возвращающая прямоугольник, нужно для проверки пересечения спрайтов
		return FloatRect(x, y, w, h); //ф-ция нужна для проверки столкновений 
	}
};

class Button{
	public:
	int x, y;
	int w, h; //ширина, высота
	bool drawThis, buttPressed, buttClick;
	Texture texture; //текстура
	Sprite sprite; //спрайт
	mcText *text;
	String name;
	StateList state;
public:
	Button (Image &image, String tmpT, Font &font, int X, int Y, int W, int H){ //конструктор с именем
		x = X; y = Y; w = W; h = H; drawThis = true; 
		name = tmpT; buttPressed = false; state = menu;
		buttClick = false;
		texture.loadFromImage (image); 
		sprite.setTexture (texture);
		sprite.setTextureRect (IntRect (0, 0, w, h));
		sprite.setPosition (x, y);
		text = new mcText (&font);
		text -> changeSize (30);
		text -> add (tmpT);
		float tmp = tmpT.getSize ();
		tmp = x + 50 - (tmp / 2) * 12;
		text -> setPosition (tmp, y - 5);

		if (tmpT == "Player" || tmpT == "Admin" || tmpT == "Back"){
			drawThis = false; state = mode;
		}
	}

	void draw (RenderWindow &window){
		window.draw (sprite);
		text -> draw (&window);
	}

	FloatRect getRect (){ //функция возвращающая прямоугольник, нужно для проверки пересечения спрайтов
		return FloatRect(x, y, w, h); //ф-ция нужна для проверки столкновений 
	}

	void checkCursor (Vector2i mousePosWin){
		buttClick = false;
		if ((mousePosWin.x >= x) && (mousePosWin.x <= x + w) && (mousePosWin.y >= y) && (mousePosWin.y <= y + h)){
			if (Mouse::isButtonPressed (Mouse::Left)){
				buttPressed = true; 
			}
			else{
				if (buttPressed)
					buttClick = true; 
				buttPressed = false;
			}
			sprite.setTextureRect (IntRect (0, 60, w, h));
		}
		else{
			buttPressed = false; 
			sprite.setTextureRect (IntRect (0, 0, w, h));
		}
	}
};

void control (Vector2i &mousePosWin, float &timer, bool **CoordWall, Wall **Arr, int &NumWall, Image &wallImage){
	int tmpX, tmpY, tmpX2, tmpY2;
	int tmp;
	if (Mouse::isButtonPressed (Mouse::Left)){
		if (mousePosWin.x >= INDENTATION && mousePosWin.x <= W_WIN -INDENTATION && mousePosWin.y >= INDENTATION && mousePosWin.y <= H_WIN - INDENTATION){
			if (timer > 200){
				timer = 0;	
				tmpX = mousePosWin.x; tmp = tmpX % EDGE; tmpX -= tmp; 
				tmpY = mousePosWin.y; tmp = tmpY % EDGE; tmpY -= tmp; 
				tmpX2 = tmpX / EDGE; tmpY2 = tmpY / EDGE;
				if (CoordWall [tmpY2 - INDENTATION / EDGE][tmpX2 - INDENTATION / EDGE]){
					for (int i = 0; i < NumWall; i++)
						if (Arr [i] -> x == tmpX && Arr [i] -> y == tmpY){
							Arr [i] -> drawThis = false;
							CoordWall [tmpY2 - INDENTATION / EDGE][tmpX2 - INDENTATION / EDGE] = false;
						}
				}
				else{
					bool tmpB = true;
					for (int i = 0; i < NumWall; i++){
						if (Arr [i] -> x == tmpX && Arr [i] -> y == tmpY){
							Arr [i] -> drawThis = true;
							tmpB = false;
						}
					}
					if (tmpB)
						Arr [NumWall++] = new Wall (wallImage, tmpX, tmpY, EDGE, EDGE);
					CoordWall [tmpY2 - INDENTATION / EDGE][tmpX2 - INDENTATION / EDGE] = true;
				}
			}
			//cout << NumWall << endl;
		}
	}

	if (Keyboard::isKeyPressed (Keyboard::O) && Keyboard::isKeyPressed (Keyboard::LControl)){
		cout << "Enter name of file which you want save:" << endl;
		char tmpC [50];
		cin >> tmpC;
		ofstream outF (tmpC);
		int tmp = 0;
		for (int i = 0; i < NumWall; i++){
			if (Arr [i] -> drawThis)
				tmp++;
		}
		outF << tmp << endl;
		for (int i = 0; i < NumWall; i++){
			if (Arr [i] -> drawThis)
				outF << Arr [i] -> x << " " << Arr [i] -> y << endl;
		}
	}
	if (Keyboard::isKeyPressed (Keyboard::LControl) && Keyboard::isKeyPressed (Keyboard::I)){
		cout << "Enter name of file which you want open:" << endl;
		char tmpC [50];
		cin >> tmpC;
		if (NumWall != 0){
			for (int i = 0; i < NumWall; i++)
				Arr [i] -> ~Wall ();
				}
		ifstream inF (tmpC);
		inF >> NumWall;
		for (int i = 0; i < NumWall; i++){
			inF >> tmpX >> tmpY;
			Arr [i] = new Wall (wallImage, tmpX, tmpY, EDGE, EDGE);
			Arr [i] -> drawThis = true;
		}
	}
}

int main (){
	RenderWindow window (VideoMode (W_WIN, H_WIN), "LABYRINTH PRO"); //создание окна

	VertexArray lines (Lines, (NUM_H_LINE + NUM_V_LINE + 2) * 2); //массив на 600 линий
	int i = 0; //i-счетчик линий занесенных в массив
    for (; i < NUM_V_LINE * 2; i += 2){ //создание вертикальных линий
        lines [i].position = Vector2f (i * EDGE / 2 + INDENTATION, INDENTATION);
        lines [i + 1].position = Vector2f (i * EDGE / 2 + INDENTATION, H_WIN - INDENTATION);
	}
	for (int k = 0; i < (NUM_H_LINE + NUM_V_LINE) * 2; i += 2, k += 2){ //создание горизонтальных линий
        lines [i].position = Vector2f (INDENTATION, EDGE * k / 2 + INDENTATION);
        lines [i + 1].position = Vector2f (W_WIN - INDENTATION, EDGE * k / 2 + INDENTATION);
	}


	StateList state = menu;


	Image wallImage; //загрузка спрайта стен
	wallImage.loadFromFile ("wall.png");
	
	int NumWall = 0;
	Wall *Arr [1000];
	bool **CoordWall;
	//cout << NUM_H_LINE << "-horizont " << NUM_V_LINE << "-vertical" << endl;
	CoordWall = new bool* [NUM_H_LINE];
	for (int i = 0; i < NUM_H_LINE; i++){
		CoordWall [i] = new bool [NUM_V_LINE];
		for (int j = 0; j < NUM_V_LINE; j++)
			CoordWall [i][j] = false;
	}


	Font font; //шрифт
	font.loadFromFile ("modeka.otf");

	Image buttonImage; //загрузка спрайта стен
	buttonImage.loadFromFile ("button.png");

	int NumButton = 0;
	Button *button [10];
	button [NumButton++] = new Button (buttonImage, "Go!", font, 300, 100, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Mode", font, 300, 150, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Setting", font, 300, 200, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Exit", font, 300, 250, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Player", font, 300, 100, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Admin", font, 300, 150, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Back", font, 300, 200, 120, 30);

	Clock clock; //время

	while (window.isOpen ()){
		float time = clock.getElapsedTime ().asMicroseconds(); //время каждый раз обновляется
		static float timer = 0;
		clock.restart ();
		time = time / 800;
		timer += time;

		Vector2i mousePosWin = Mouse::getPosition (window); //координаты мыши относ. окна
		Vector2f posMouse = window.mapPixelToCoords (mousePosWin); //координаты мыши относ. карты

		Event event; //обработчи закрытия окна !нужно для корректного закрытия окна
		while (window.pollEvent (event)){
			if (event.type == Event::Closed || Keyboard::isKeyPressed (Keyboard::Escape)) 
				window.close (); 
		}		

		switch (state){
			case admin:
				control (mousePosWin, timer, CoordWall, Arr, NumWall, wallImage);
				break;
			case player:
				if (Keyboard::isKeyPressed (Keyboard::LControl) && Keyboard::isKeyPressed (Keyboard::I))
					control (mousePosWin, timer, CoordWall, Arr, NumWall, wallImage);
				break;
			case menu:
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> drawThis){
						button [i] -> checkCursor (mousePosWin);
						if (button [i] -> buttClick && button [i] -> name == "Mode")
							state = mode; 
						if (button [i] -> buttClick && button [i] -> name == "Exit")
							state = exitt;
					}
				break;
			case mode:
				for (int i = 0; i < NumButton; i++){
					if (button [i] -> state == mode)
						button [i] -> drawThis = true;
					else
						button [i] -> drawThis = false;
				}
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> drawThis){
						button [i] -> checkCursor (mousePosWin);
						if (button [i] -> buttClick && button [i] -> name == "Admin")
							state = admin;
						if (button [i] -> buttClick && button [i] -> name == "Player")
							state = player;
						if (button [i] -> buttClick && button [i] -> name == "Back")
							state = backToMenu;
					}
				break;
			case backToMenu:
				for (int i = 0; i < NumButton; i++){
					if (button [i] -> state == mode)
						button [i] -> drawThis = false;
					else
						button [i] -> drawThis = true;
				}
				state = menu;
				break;
			case exitt:
				window.close ();
				break;
		}

		window.clear (Color (40, 36, 62));
		if (state == admin || state == player){
			window.draw (lines); //рисую массив линий
			for (int j = 0; j < NumWall; j++)
				if (Arr [j] -> drawThis)
					window.draw (Arr [j] -> sprite);
		}
		if (state == menu || state == mode || state == backToMenu || state == exitt)
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> drawThis)
					button [i] -> draw (window);
		window.display ();
	}
	return 0;
}