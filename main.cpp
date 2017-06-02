#include <SFML/Graphics.hpp> //основная библиотека для работы с графикой и изображением
#include <SFML/Audio.hpp> //для работы со звуком и музыкой
#include <iostream> //для сяутов и синов
#include <fstream> //для работы с файлами (записывается туда уровни инфо об игроке и т.д.)
#include "forMcText.h" //взял библиотеку по ссылке kychka-pc.ru/wiki/svobodnaya-baza-znanij-sfml/test/sistemnyj-modul-system-module/potoki-threads/sfsound-sfmusic
#include "algorithm.h" //написанная мной функция для нахождения минимального пути в лабиринте (поиск в ширину) или волновой поиск
#include <Windows.h> //нужна что б узнать разрешение экрана игрока
#include "view.h" //управление камерой
using namespace std;
using namespace sf;


enum StateList {menu, mode, admin, player, settings, exitt, reqPass, selectLVL, AdSelectLVL, AdSaveLVL, completeLVL, pause, startLVL, myLVLs, newGame, allState}; //основное перечесление которое отвечает за состояние игры
enum StatePlayer {rectangle, triangle, circle};
enum CreateWall {rectangleW, triangleW, circleW, wall, finishW, startW, saveW};


class System{ //основной класс игры, в котором хранится все самое выжное
public:
	static Vector2i mousePosWin; //координаты мыши относ. окна
	static Vector2f posMouse; //координаты мыши относ. карты

	static RenderWindow *window; //окно, в котором запускается игра

	static Event event; //событие

	static int FPS;
	static Clock clock; //время
	static float timer; //таймер
	static float time; //время

	static Music backgroundMusic; //фоновая музыка
	static float volumBackMusic; //громокость фоновой музыки

	static SoundBuffer buffer; //буфер для звука нажатия на кнопки
	static Sound sndClickButt; //звук нажатия на кнопку
	static float volSndClickButt; //громкость звука

	static CreateWall whichWall;

	static String AdOrPlMode; //строка хранящая имя текущего мода игры (игрок или админ)
	static Coordinate Start, Finish; //координаты начала (откуда игрок стартует) и конца (куда должен придти)
	static bool lvlComplete; //показывает завершен уровень

	static float speed; //скорость с которой движется игрок по уровню

	static bool PassEnter; //введен ли пароль игроком от админ мода
	static int PassedLVL; //сколько пройдено уровней

	int GLOBAL_W;
	int GLOBAL_H;
	int W_WIN;
	int H_WIN;
	int EDGE;
	int NUM_CELL_X; 
	int NUM_CELL_Y;
	int W_BUTTON;
	int H_BUTTON;
	int GLOB_IND_W; 
	int GLOB_IND_H;
	int SIZE_TEXT;
	int NUM_SQUARE;
	int SQUARE;
public:
	System (){
		GLOBAL_W = 4000; //2240 //максимальное разрешение экрана в котором игра пойдет, ширина
		GLOBAL_H = 3000; //1280 //высота
		W_WIN = GetSystemMetrics (0); //GetSystemMetrics (0) // самое маленькое 1366 разрешение на котором пойдет игра, ширина
		H_WIN = GetSystemMetrics (1); //GetSystemMetrics(1) // самое маленькое 768, высота
		EDGE = 10; //размер одной клетки
		NUM_CELL_X = 64; //количество клеток уровня по ширине
		NUM_CELL_Y = 32; //количество клеток уровня по высоте
		W_BUTTON = W_WIN / 10; //ширина кнопки
		H_BUTTON = H_WIN / 20; //высота кнопки
		SIZE_TEXT = (int) 30 * H_BUTTON / 44;

		while (1){
			if ((H_WIN - NUM_CELL_Y * EDGE) / 2 < H_BUTTON + 40 || (W_WIN - NUM_CELL_X * EDGE) / 2 < 60)
				break;
			EDGE++;
		}
		NUM_SQUARE = 8; 
		SQUARE = EDGE * NUM_CELL_Y / NUM_SQUARE;

		GLOB_IND_W = (GLOBAL_W - NUM_CELL_X * EDGE) / 2; //отступ по ширине, с которого начинается область которую видит игрок
		GLOB_IND_H = (GLOBAL_H - NUM_CELL_Y * EDGE) / 2; //отступ по высоте, с которого начинается область которую видит игрок
		speed = (float) EDGE / 20; // EDGE /13
	}
};

//инициализируем всё, нужно что б если переменная менялась где-то, то она менялась и во всех классах наследниках
Vector2i System::mousePosWin;
Vector2f System::posMouse;

RenderWindow* System::window;

Event System::event;

int System::FPS;
Clock System::clock;
float System::timer;
float System::time;

Music System::backgroundMusic;
float System::volumBackMusic;

SoundBuffer System::buffer;
Sound System::sndClickButt;
float System::volSndClickButt;

CreateWall System::whichWall;

String System::AdOrPlMode;
Coordinate System::Start;
Coordinate System::Finish;
bool System::lvlComplete;

float System::speed;

bool System::PassEnter;
int System::PassedLVL;


class Body : public System{ //класс который служит основой для всех других классов с графикой
public:
	int x, y; //координаты
	int w, h; //ширина, высота
	Texture texture; //текстура
	String name; //имя
	RectangleShape shape; //текстура
	int wTexture, hTexture; //что бы масштабировать текстуру, нужно знать какой именно кусок тайла масштабировать
public:
	Body (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture){
		x = X; y = Y; w = W; h = H; name = Name;
		wTexture = WTexture; hTexture = HTexture;
		texture.loadFromImage (image); 
		shape.setSize (Vector2f ((float) w, (float) h));
		shape.setPosition ((float) x, (float) y);
		shape.setTexture (&texture);
		shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));	
		
	}

	virtual void draw () = 0;
};


class Wall : public Body{ //класс стены
public:
	Wall (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){ //конструктор с именем
		shape.setPosition ((float) x * EDGE + GLOB_IND_W, (float) y * EDGE + GLOB_IND_H);
		if (name == "Save")            shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
		else if (name == "Wall")       shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		else if (name == "Finish")     shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		else if (name == "Start")      shape.setTextureRect (IntRect (0, hTexture * 3, wTexture, hTexture));
		else if (name == "Circle")     shape.setTextureRect (IntRect (0, hTexture * 4, wTexture, hTexture));
		else if (name == "Rectangle")  shape.setTextureRect (IntRect (0, hTexture * 5, wTexture, hTexture));
		else if (name == "Triangle")   shape.setTextureRect (IntRect (0, hTexture * 6, wTexture, hTexture));
	}

	Wall& operator= (const Wall& tmpW){
		if (this != &tmpW){
			x = tmpW.x; y = tmpW.y;
			w = tmpW.w; h = tmpW.h;  
			name = tmpW.name; 
			wTexture = tmpW.wTexture; hTexture = tmpW.hTexture;
			shape.setPosition ((float) x * EDGE + GLOB_IND_W, (float) y * EDGE + GLOB_IND_H);
			if (name == "Save")            shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
			else if (name == "Wall")       shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
			else if (name == "Finish")     shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
			else if (name == "Start")      shape.setTextureRect (IntRect (0, hTexture * 3, wTexture, hTexture));
			else if (name == "Circle")     shape.setTextureRect (IntRect (0, hTexture * 4, wTexture, hTexture));
			else if (name == "Rectangle")  shape.setTextureRect (IntRect (0, hTexture * 5, wTexture, hTexture));
			else if (name == "Triangle")   shape.setTextureRect (IntRect (0, hTexture * 6, wTexture, hTexture));
		}
		return *this;
	}

	void draw (){ 
		window -> draw (shape);
	}

	void changeCoord (int tmpX, int tmpY){
		shape.setPosition ((float) tmpX, (float) tmpY);
	}
};

class Background : public Body{ //класс фонового изображения
public:
	Background (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){ }

	void changeCoord (int x2, int y2){ //функция изменения координат черного заднего фона (центр фона находится где центр игрока)
		shape.setOrigin ((float) w / 2, (float) h / 2);
		shape.setPosition ((float) x2, (float) y2);
	}

	void draw (){ 
		window -> draw (shape);
	}
};

class Player : public Body{ //класс игрока
public:
	int tmpX, tmpY; //переменные которые хранят место куда мы хотим попасть, нажав клавишу
	bool playerMove; //движется ли игрок
	StatePlayer statePl;
	int currDir;
	float xx, yy;
public:
	Player (Image &image, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, "Player", X, Y, W, H, WTexture, HTexture){ //конструктор без имени
	    xx = (float) x; yy = (float) y; 
		tmpX = x; tmpY = y; 
		playerMove = false;
		currDir = 0; statePl = rectangle; 
		shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		shape.setPosition ((float) GLOBAL_W / 2 - SQUARE / 2, (float) GLOBAL_H / 2 - SQUARE / 2);
	}

	void changeFigure (){
		if (Keyboard::isKeyPressed (Keyboard::R) || Keyboard::isKeyPressed (Keyboard::Num1)){
			statePl = rectangle;
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		}
		else if (Keyboard::isKeyPressed (Keyboard::T) || Keyboard::isKeyPressed (Keyboard::Num2)){
			statePl = triangle;
			shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		}
		else if (Keyboard::isKeyPressed (Keyboard::C) || Keyboard::isKeyPressed (Keyboard::Num3)){
			statePl = circle;
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
		}
	}

	void update (bool **CoordWall){
		//может быть нажата одновременно только одна клавиша
		if (currDir < NumMoves && !playerMove){
			if (Direction [currDir] == 4)        tmpY = y - EDGE; //запоминаем координаты куда мы должы придти
			else if (Direction [currDir] == 1)   tmpY = y + EDGE; //запоминаем координаты куда мы должы придти
			else if (Direction [currDir] == 2)   tmpX = x - EDGE; //запоминаем координаты куда мы должы придти 
			else if (Direction [currDir] == 3)   tmpX = x + EDGE; //запоминаем координаты куда мы должы придти
			currDir++; playerMove = true;
			xx = (float) x; yy = (float) y;
		}

		changeFigure ();

		if (playerMove){ //проверяем, нет ли стены на том месте куда мы хотим перейти
			//cout << speed << " -speed" << endl;
			//cout << xx << " " << yy << " -xx & yy" << endl;
			//cout << tmpX << " " << tmpY << " -tmpX & tmpY" << endl;
			//cout << xx - tmpX << " " << yy - tmpY << " difference" << endl;
			//cout << endl;
			if (abs (xx - tmpX) < 1 && abs (yy - tmpY) < 1){ //если мы попали туда куда хотели, то игрок не движется
				playerMove = false; 
				if (x < tmpX)        x = (int) xx + 1;
				else if (x > tmpX)   x = (int) xx;
				else if (y < tmpY)   y = (int) yy + 1;
				else if (y > tmpY)   y = (int) yy;
				xx = (float) x; yy = (float) y;
			}
			else{
				if (x < tmpX)        xx += speed;  
				else if (x > tmpX)   xx -= speed; 
				else if (y < tmpY)   yy += speed; 
				else if (y > tmpY)   yy -= speed;  
				x = (int) xx; y = (int) yy;
			}
		}

		if (x == Finish.x && y == Finish.y) //есди мы достигли финиша, то будет показана кнопка, свидетельствующая об этом
			lvlComplete = true;
		else
			lvlComplete = false; 
	}

	void changeCoord (int x2, int y2){ //функция нужна для перемещения игрока в нужную координату (нужно при открытии уровня игркоом)
		x = x2; y = y2; 
		xx = (float) x; yy = (float) y;
		tmpX = x; tmpY = y; 
	}

	void draw (){ 
		window -> draw (shape);
	}
};


class BodyButton : public Body{ //тело кнопок
public:
	mcText *text; //текст который выводится на кнопке
	String buttText;
	bool drawThis, buttPressed, buttClick; //рисовать ли кнопку, нажата ли кнопка и кликнули ли по кнопке. Клик- это нажать и отпустить кнопку когда курсор мыши на кнопке
	Font font; //шрифт
	StateList state; //каждая кнопка кроме имени имеет группу к которой она относится
	int value; //значение кнопки

	bool changeForm;
	float reducePrecent;
	float enlargePrecent;
public:
	BodyButton (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    Body (image, Name, X, Y, W, H, WTexture, HTexture){
	    font = Font; state = State; buttText = Text;
		drawThis = false; buttClick = false; buttPressed = false; 
		changeForm = false; reducePrecent = 100; enlargePrecent = 1;

		text = new mcText (&font); //создаем текст который будет отображаться на кнопке
		text -> changeSize (SIZE_TEXT); //размер текста
		text -> add (buttText);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	void draw (){ }

	virtual void checkCursor () = 0;

	virtual void updateText (char *Pass) = 0;

	virtual void reduceButton (){
		if (changeForm){
			shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
			shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);
			text -> clear ();
			text -> changeSize (SIZE_TEXT * (int) reducePrecent / 100); //размер текста
			if (name != "Pause" && name != "Leave?" && name != "StartLVL")
				text -> add (buttText);
			else
				text -> add (buttText, Color (193, 180, 180));
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * reducePrecent / 100 / 3); //распологаем текст по кнопке

			reducePrecent -= 3;
			if (reducePrecent < 3){
				reducePrecent = 100;
				changeForm = false;
			}
		}
	}

	virtual void enlargeButton (){
		if (changeForm){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);
			text -> clear ();
			text -> changeSize (SIZE_TEXT * (int) enlargePrecent / 100); //размер текста
			if (name != "Pause" && name != "Leave?" && name != "StartLVL")
				text -> add (buttText);
			else
				text -> add (buttText, Color (193, 180, 180));
			if (name == "Edit")
				text -> clear ();
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * enlargePrecent / 100 / 3); //распологаем текст по кнопке

			enlargePrecent += 3;
			if (enlargePrecent > 100){
				enlargePrecent = 1;
				changeForm = false;
			}
		}
	}

	virtual void clearButton (){
		shape.setSize (Vector2f (1, 1));
		text -> clear ();
	}
};

class Button : public BodyButton{
public:
	bool buttLocked;
public:
	Button (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int Value, int WTexture, int HTexture) : 
		    BodyButton (image, Text, Name, Font, State, X, Y, W, H, WTexture, HTexture){
		value = Value; buttLocked = false;

		if (state == menu)
			drawThis = true;
		else
			drawThis = false;
		shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
		if (name == "SelectLVL")
			shape.setTextureRect (IntRect (0, 120, wTexture, hTexture));
	}

	void draw (){
		window -> draw (shape);
		text -> draw (window);
	}

	void checkCursor (){ //функция проверки на нажатие кнопки или наведением курсора на кнопку
		if (name == "SelectLVL")
			if (value > PassedLVL + 1)  buttLocked = true;
			else                        buttLocked = false;
		if (name == "My lvls")
			if (!PassEnter)             buttLocked = true;
			else                        buttLocked = false;

		buttClick = false;
		if (!buttLocked){
			if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //если курсор мыши находится на кнопке
				if (Mouse::isButtonPressed (Mouse::Left)) //и если нажали на нее
					buttPressed = true;
				else{
					if (buttPressed) //если же курсор на кнопке и кнопка была нажата, а сейчас не нажата-значит мы кликнули по ней
						buttClick = true; 
					buttPressed = false;
				}
				shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture)); //если наведен курсор на мышку, то кнопка меняет текстуру
				if (name == "SelectLVL")
					shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture));
			}
			else{
				buttPressed = false; //если курсор не на мыши то кнопка обычного вида
				shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
				if (name == "SelectLVL")
					shape.setTextureRect (IntRect (0, 4 * hTexture, wTexture, hTexture));
			}
		}
		else{
			shape.setTextureRect (IntRect (0, hTexture * 3, wTexture, hTexture));
			if (name == "SelectLVL")
				shape.setTextureRect (IntRect (3 * wTexture, 4 * hTexture, wTexture, hTexture));
		}

		if (buttClick && (name == "AdminMode" || name == "PlayerMode")) //если мы в state = mode, можем выбрать режим игры, админ (для редактирования и создания карт) или игрок (играть)
			AdOrPlMode = name; //переменная хранящая текущий режим игры

		if (name == AdOrPlMode) //и если имя переменной которая хранит имя режима совпала с кнопкой, то кнопка выбрана (подсвечивается золотистым)
			shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
	}

	void updateText (char *Pass){ }
};

class EditButton : public BodyButton{
public:
	EditButton (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    BodyButton (image, Text, Name, Font, State, X, Y, W, H, WTexture, HTexture){ }

	void draw (){
		window -> draw (shape);
		text -> draw (window);
	}

	void checkCursor (){ //функция проверки на нажатие кнопки или навдением курсора на кнопку
		buttClick = false;
		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //если курсор мыши находится на кнопке
			if (Mouse::isButtonPressed (Mouse::Left)) //и если нажали на нее
				buttPressed = true;
			else{
				if (buttPressed) //если же курсор на кнопке и кнопка была нажата, а сейчас не нажата-значит мы кликнули по ней
					buttClick = true; 
				buttPressed = false;
			}
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture)); //если наведен курсор на мышку, то кнопка меняет текстуру
		}
		else{
			buttPressed = false; //если курсор не на мыши то кнопка обычного вида
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
		}
	}

	void updateText (char *Pass){ //функция обновления текста
		buttText = Pass;
		text -> clear ();
		text -> changeSize (SIZE_TEXT); //размер текста
		text -> add (buttText);
		float tmp = (float) buttText.getSize (); //получаем длинну текста в символах
		tmp /= 2;
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
	}
};

class Static : public BodyButton{
public:
	Static (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    BodyButton (image, Text, Name, Font, State, X, Y, W, H, WTexture, HTexture){ 
		
		text -> clear ();
		text -> changeSize (SIZE_TEXT); //размер текста
		if (name == "TimePlayer")
			text -> add (buttText, Color (211, 25, 12));
		else if (name == "Pause" || name == "Leave?" || name == "StartLVL")
			text -> add (buttText, Color (193, 180, 180));
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
	}

	void draw (){
		text -> draw (window);
	}

	void checkCursor (){ }

	void updateText (char *Pass){
		text -> clear ();
		text -> changeSize (SIZE_TEXT); //размер текста
		char tmpC [40];
		if (name == "TimePlayer"){
			strcpy (tmpC, "Time: ");
			strcat (tmpC, Pass);
			text -> add (tmpC, Color (211, 25, 12));
		}
		else if (name == "FPS"){
			strcpy (tmpC, "FPS: ");
			strcat (tmpC, Pass);
			text -> add (tmpC);
		}
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
	}
};

class HorizontScrollBar : public BodyButton{
public:
	int leftBorder, rightBorder; //левая и правая граница по которой может перемещаться кнопка (не вся доступная область, т.к. кнопка имеет ширину)
	RectangleShape backgroundd;
	int wBground;
	int hBground;
	int x2, y2;
public:
	HorizontScrollBar (Image &image, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int tmpBordL, int tmpBordR, int WTexture, int HTexture, int WBground, int HBground) : 
		    BodyButton (image, "", Name, Font, State, X, Y, W, H, WTexture, HTexture){ 
        leftBorder = tmpBordL; rightBorder = tmpBordR;
		wBground = WBground; hBground = HBground;
		x2 = x; y2 = y;

		backgroundd.setSize (Vector2f ((float) W_BUTTON, (float) H_BUTTON));
		backgroundd.setOrigin ((float) W_BUTTON / 2, (float) H_BUTTON / 2);
		backgroundd.setPosition ((float) x2, (float) y2);
		backgroundd.setTexture (&texture);
		backgroundd.setTextureRect (IntRect (0, 5 * hBground, wBground, hBground));	

		shape.setTextureRect (IntRect (0, 4 * hTexture, wTexture, hTexture));
		if (name == "MusicSlider"){ //считываем с файла данные об настройках игрока, и устанавливаем кнопку в нужное пложение
			shape.setPosition ((float) leftBorder + volumBackMusic * (rightBorder - leftBorder) / 100, (float) y);
			x = leftBorder + (int) volumBackMusic * (rightBorder - leftBorder) / 100;
		}
		if (name == "SoundSlider"){
			shape.setPosition ((float) leftBorder + volSndClickButt * (rightBorder - leftBorder) / 100, (float) y);
			x = leftBorder + (int) volSndClickButt * (rightBorder - leftBorder) / 100;
		}
	}

	void draw (){
		window -> draw (backgroundd);
		window -> draw (shape);
	}

	void checkCursor (){ 
		if (posMouse.x >= leftBorder && posMouse.x <= rightBorder) //если мышка находится в доступном для кнопки месте
			if (posMouse.y >= y - h / 2 && posMouse.y <= y + h / 2){
				if (event.type == Event::MouseButtonPressed && !buttPressed){
					shape.setPosition ((float) posMouse.x, (float) y);
					x = (int) posMouse.x; buttPressed = true;
					if (name == "MusicSlider"){ //меняем значение грмоксоти музыки
						volumBackMusic = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						backgroundMusic.setVolume (volumBackMusic); //значение громкости устанавливаем в процентом соотношении, считая от левой границы
					}
					if (name == "SoundSlider"){ //меняем значение громкости звуков
						volSndClickButt = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						sndClickButt.setVolume (volSndClickButt); //значение громкости устанавливаем в процентом соотношении, считая от левой границы
					}
				}
				if (buttPressed){ //если кнопка зажата
					shape.setPosition ((float) posMouse.x, (float) y);
					x = (int) posMouse.x; 
					if (name == "MusicSlider"){ //меняем значение грмоксоти музыки
						volumBackMusic = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						backgroundMusic.setVolume (volumBackMusic); //значение громкости устанавливаем в процентом соотношении, считая от левой границы
					}
					if (name == "SoundSlider"){ //меняем значение громкости звуков
						volSndClickButt = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						sndClickButt.setVolume (volSndClickButt); //значение громкости устанавливаем в процентом соотношении, считая от левой границы
					}
				}

				if (event.type == Event::MouseButtonPressed) //если мышка зажата, то устанавливаем соотв. флаг
					buttClick = true;
				else
					buttClick = false;
		}

		if (event.type == Event::MouseButtonReleased) //если отпустили мышку
			buttPressed = false;

		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)) //если курсор мыши находится на кнопке
			shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture)); //если наведен курсор на мышку, то кнопка меняет текстуру
		else
			shape.setTextureRect (IntRect (0, 4 * hTexture, wTexture, hTexture));
	}

	void updateText (char *Pass){ }

	void reduceButton (){
		if (changeForm){
			shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
			shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);

			backgroundd.setSize (Vector2f ((float) W_BUTTON * reducePrecent / 100, (float) H_BUTTON * reducePrecent / 100));
			backgroundd.setOrigin ((float) W_BUTTON * reducePrecent / 100 / 2, (float) H_BUTTON * reducePrecent / 100 / 2);
			backgroundd.setPosition ((float) x2, (float) y2);

			reducePrecent -= 3;
			if (reducePrecent < 3){
				reducePrecent = 100;
				changeForm = false;
			}
		}
	}

	void enlargeButton (){
		if (changeForm){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);

			backgroundd.setSize (Vector2f ((float) W_BUTTON * enlargePrecent / 100, (float) H_BUTTON * enlargePrecent / 100));
			backgroundd.setOrigin ((float) W_BUTTON * enlargePrecent / 100 / 2, (float) H_BUTTON * enlargePrecent / 100 / 2);
			backgroundd.setPosition ((float) x2, (float) y2);

			enlargePrecent += 3;
			if (enlargePrecent > 100){
				enlargePrecent = 1;
				changeForm = false;
			}
		}
	}

	void clearButton (){
		shape.setSize (Vector2f (1, 1));
		backgroundd.setSize (Vector2f (1, 1));
	}
};

class PictureButton : public BodyButton{
public:
	Texture pictureT;
	RectangleShape picture;
	int wPicture;
	int hPicture;
	CreateWall typeWall;
public:
	PictureButton (Image &image, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int WTexture, int HTexture, Image &Ipicture, int WPicture, int HPicture) : 
		    BodyButton (image, "", Name, Font, State, X, Y, W, H, WTexture, HTexture){
		
		shape.setTextureRect (IntRect (0, 120, wTexture, hTexture));
        pictureT.loadFromImage (Ipicture);
		wPicture = WPicture; hPicture = HPicture;
		picture.setSize (Vector2f ((float) w, (float) h));
		picture.setPosition ((float) x, (float) y);
		picture.setTexture (&pictureT);
			
		picture.setOrigin ((float) w / 2, (float) h / 2);

		if (name == "Rectangle")       { typeWall = rectangleW; picture.setTextureRect (IntRect (0, 0 * hPicture, wPicture, hPicture)); }
		else if (name == "Triangle")   { typeWall = triangleW;  picture.setTextureRect (IntRect (0, 1 * hPicture, wPicture, hPicture)); }
		else if (name == "Circle")     { typeWall = circleW;    picture.setTextureRect (IntRect (0, 2 * hPicture, wPicture, hPicture)); }
		else if (name == "Wall")       { typeWall = wall;       picture.setTextureRect (IntRect (0, 3 * hPicture, wPicture, hPicture)); }
		else if (name == "Start")      { typeWall = startW;     picture.setTextureRect (IntRect (0, 4 * hPicture, wPicture, hPicture)); }
		else if (name == "Finish")     { typeWall = finishW;    picture.setTextureRect (IntRect (0, 5 * hPicture, wPicture, hPicture)); }
		else if (name == "Save")       { typeWall = saveW;      picture.setTextureRect (IntRect (0, 6 * hPicture, wPicture, hPicture)); }
	}

	void draw (){
		window -> draw (shape);
		window -> draw (picture);
	}

	void checkCursor (){ //функция проверки на нажатие кнопки или наведением курсора на кнопку
		buttClick = false;
		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //если курсор мыши находится на кнопке
			if (Mouse::isButtonPressed (Mouse::Left)) //и если нажали на нее
				buttPressed = true;
			else{
				if (buttPressed){ //если же курсор на кнопке и кнопка была нажата, а сейчас не нажата-значит мы кликнули по ней
					buttClick = true; whichWall = typeWall;
				}
				buttPressed = false;
			}
			shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture));
		}
		else{
			buttPressed = false; //если курсор не на мыши то кнопка обычного вида
			shape.setTextureRect (IntRect (0, 4 * hTexture, wTexture, hTexture));
		}		

		if (typeWall == whichWall)
			shape.setTextureRect (IntRect (2 * wTexture, 4 * hTexture, wTexture, hTexture));
		else
			if (name == "Rectangle")       picture.setTextureRect (IntRect (0, 0 * hPicture, wPicture, hPicture));
			else if (name == "Triangle")   picture.setTextureRect (IntRect (0, 1 * hPicture, wPicture, hPicture));
			else if (name == "Circle")     picture.setTextureRect (IntRect (0, 2 * hPicture, wPicture, hPicture));
			else if (name == "Wall")       picture.setTextureRect (IntRect (0, 3 * hPicture, wPicture, hPicture));
			else if (name == "Start")      picture.setTextureRect (IntRect (0, 4 * hPicture, wPicture, hPicture));
			else if (name == "Finish")     picture.setTextureRect (IntRect (0, 5 * hPicture, wPicture, hPicture));
			else if (name == "Save")       picture.setTextureRect (IntRect (0, 6 * hPicture, wPicture, hPicture));
	}

	void updateText (char *Pass){ }

	void reduceButton (){
		if (changeForm){
			shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
			shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);

			picture.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
			picture.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
			picture.setPosition ((float) x, (float) y);

			reducePrecent -= 3;
			if (reducePrecent < 3){
				reducePrecent = 100;
				changeForm = false;
			}
		}
	}

	void enlargeButton (){
		if (changeForm){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);

			picture.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			picture.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			picture.setPosition ((float) x, (float) y);

			enlargePrecent += 3;
			if (enlargePrecent > 100){
				enlargePrecent = 1;
				changeForm = false;
			}
		}
	}

	void clearButton (){
		shape.setSize (Vector2f (1, 1));
		picture.setSize (Vector2f (1, 1));
	}
};


class Game : public System{ //вся механика и инициализация игры в этом классе
public:
	StateList state; //состояние игры
	int CurrentLVL; //текущий уровень
	char Pass [30]; //пароль
	bool escapeReleased; //флаг равен 1 если ескейп отпустили (ну его нажали, а потом отпустили)
	bool enterReleased; //флаг равен 1 если Enter отпустили (ну его нажали, а потом отпустили)
	bool anyKeyReleased; //флаг равен 1 если Enter отпустили (ну его нажали, а потом отпустили)
	bool playerLVL; //игрок играет в свой созданный уровень?
	bool changeStates;
	StateList whichStateWas;
	StateList whichStateWill;
	bool secondPhase;

	Image wallImage; //загрузка спрайта стен
	Image wallImagePL;
	int NumWall; //количество стен
	Wall *ArrWall [10000]; //массив стен
	bool **CoordWall; //координаты стен
	int indexFinish; //индекс финиша (что б долго не искать)
	int indexStart; //индекс старта (что б долго не искать)
	Wall *BorderWall [500];
	int NumBorderWall;

	VertexArray lines; //линии которые в админ моде рисуются, что б легче было создавтаь уровни
	VertexArray playerLines;

	Background *plBackground; //фоновое изображение, черное, которые закрывает лабаринт

	Player *pl; //игрок

	mcText *timePlText;
	float timePl;
	float AllTime;

	int indexFPSBut;
	int indexTimePlBut;
	int NumButton; //количество кнопок
	BodyButton *button [100]; //массив кнопок

	char myLVLname [50];  //имя файла открытого игроком, и уровень при этом созданный игроком
	char fileNameAd [50]; //имя файла открытого админом
	char fileNamePl [70]; //имя файла открытого игроком
public:
	void readInfo (){ //считать информацию об игроке
		ifstream inF ("Resources/Info/Player.txt");
		inF >> PassedLVL >> volumBackMusic >> PassEnter >> volSndClickButt >> AllTime;
	}

	void writeInfo (){ //записать информациюю об игроке
		ofstream outF ("Resources/Info/Player.txt");
		outF << PassedLVL << " " << volumBackMusic << " " << PassEnter << " " << volSndClickButt  << " " << AllTime << endl;
	}

	void draw (){ //главная и единственная функция рисования
		window -> setView (view); //обновляем камеру
		window -> clear (Color (40, 36, 62));

		if (state == admin || state == AdSelectLVL || state == AdSaveLVL){
			window -> draw (lines);
			for (int i = 0; i < NumWall; i++) //рисую стены
					ArrWall [i] -> draw ();
		}

		if (state == player || state == pause || state == startLVL){
			window -> draw (playerLines);
			int tmpX, tmpY;
			float tmpX2, tmpY2;
			tmpX = (int) pl -> x; tmpX -= GLOB_IND_W;
			tmpY = (int) pl -> y; tmpY -= GLOB_IND_H;
			for (int i = 0; i < NumWall; i++){ //рисую стены			
				if (abs (ArrWall [i] -> x * EDGE - tmpX) < EDGE * (NUM_SQUARE + 1) / 2 && abs (ArrWall [i] -> y * EDGE - tmpY) < EDGE * (NUM_SQUARE + 1) / 2){
					tmpX2 = (float) ArrWall [i] -> x * EDGE - tmpX;
					tmpY2 = (float) ArrWall [i] -> y * EDGE - tmpY;
					tmpX2 = tmpX2 * ((float) SQUARE / (float) EDGE);
					tmpY2 = tmpY2 * ((float) SQUARE / (float) EDGE);
					ArrWall [i] -> changeCoord (GLOBAL_W / 2 + (int) tmpX2 - SQUARE / 2, GLOBAL_H / 2 + (int) tmpY2 - SQUARE / 2);
					ArrWall [i] -> draw ();
				}
			}
			for (int i = 0; i < NumBorderWall; i++)
				if (abs (BorderWall [i] -> x * EDGE - tmpX) < EDGE * (NUM_SQUARE + 1) / 2 && abs (BorderWall [i] -> y * EDGE - tmpY) < EDGE * (NUM_SQUARE + 1) / 2){
					tmpX2 = (float) BorderWall [i] -> x * EDGE - tmpX;
					tmpY2 = (float) BorderWall [i] -> y * EDGE - tmpY;
					tmpX2 = tmpX2 * ((float) SQUARE / (float) EDGE);
					tmpY2 = tmpY2 * ((float) SQUARE / (float) EDGE);
					BorderWall [i] -> changeCoord (GLOBAL_W / 2 + (int) tmpX2 - SQUARE / 2, GLOBAL_H / 2 + (int) tmpY2 - SQUARE / 2);
					BorderWall [i] -> draw ();
				}
			
			plBackground -> draw ();
			pl -> draw (); 
			button [indexTimePlBut] -> draw (); //рисую кнопку где отображается время (не хотелось захламлять код лишними if)
		}

		for (int i = 0; i < NumButton; i++) //рисую кнопки
			if (button [i] -> drawThis || button [i] -> state == allState)
				button [i] -> draw ();

		window -> display ();
	}

	void initializeButton (){
		Font font;
		font.loadFromFile ("Resources/Fonts/modeka.otf");

		Image buttonImage; //загрузка спрайта стен
		buttonImage.loadFromFile ("Resources/Textures/button.png");

		StateList tmpS;
		int tmpI;

		NumButton = 0;

		tmpS = allState;
		indexFPSBut = NumButton;
		button [NumButton++] = new Static (buttonImage, "FPS: 0", "FPS", font, tmpS, GLOBAL_W / 2 - W_WIN / 2 + W_BUTTON / 2, GLOBAL_H / 2 + H_WIN / 2 - H_BUTTON / 2, W_BUTTON, H_BUTTON, 120, 30);

		tmpS = completeLVL;
		button [NumButton++] = new Button (buttonImage, "End lvl", "lvlComplete", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = menu;
		button [NumButton++] = new Button (buttonImage, "Go!",     "Go!", font, tmpS,      GLOBAL_W / 2, GLOBAL_H / 2 - 4 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Mode",    "Mode", font, tmpS,     GLOBAL_W / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Setting", "Settings", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Exit",    "Exit", font, tmpS,     GLOBAL_W / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = exitt;
		button [NumButton++] = new Static (buttonImage, "Quit game?", "Quit game?", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON,    120, 30);
		button [NumButton++] = new Button (buttonImage, "No!",        "QuitNo", font, tmpS,     GLOBAL_W / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Yes.",       "QuitYes", font, tmpS,    GLOBAL_W / 2, GLOBAL_H / 2,                      W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = newGame;
		button [NumButton++] = new Static (buttonImage, "Start new game?", "NewGame?", font, tmpS,      GLOBAL_W / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), W_BUTTON, H_BUTTON,    120, 30);
		button [NumButton++] = new Button (buttonImage, "Yes!",            "New Game", font, tmpS,      GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "No.",             "Continue Game", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = settings;
		button [NumButton++] = new Static (buttonImage, "Sound:",  "VolSound", font, tmpS,      GLOBAL_W / 2 - W_BUTTON / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), W_BUTTON, H_BUTTON,     120, 30);
		button [NumButton++] = new Static (buttonImage, "Music:",  "VolMusic", font, tmpS,      GLOBAL_W / 2 - W_BUTTON / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON,     120, 30);
		button [NumButton++] = new Button (buttonImage, "Back",    "BackToMenuSet", font, tmpS, GLOBAL_W / 2,                GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0,  120, 30);
		button [NumButton++] = new HorizontScrollBar (buttonImage, "SoundSlider", font, tmpS,   GLOBAL_W / 2 + W_BUTTON / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), 20, H_BUTTON, GLOBAL_W / 2 + 10, GLOBAL_W / 2 + W_BUTTON - 10, 30, 30, 120, 30);
		button [NumButton++] = new HorizontScrollBar (buttonImage, "MusicSlider", font, tmpS,   GLOBAL_W / 2 + W_BUTTON / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), 20, H_BUTTON, GLOBAL_W / 2 + 10, GLOBAL_W / 2 + W_BUTTON - 10, 30, 30, 120, 30);
		

		tmpS = mode;
		button [NumButton++] = new Button (buttonImage, "Player",   "PlayerMode", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 4 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Admin",    "AdminMode", font, tmpS,  GLOBAL_W / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Back",     "BackToMenu", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);

		Image pictureImage; //загрузка спрайта стен
		tmpS = admin;
		tmpI = GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new Button (buttonImage, "Back",   "BackToMenuAd", font, tmpS, GLOBAL_W / 2 - W_WIN / 4, tmpI, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Open",   "OpenAd", font, tmpS,       GLOBAL_W / 2,             tmpI, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Save",   "SaveAd", font, tmpS,       GLOBAL_W / 2 + W_WIN / 4, tmpI, W_BUTTON, H_BUTTON, 0, 120, 30);
		pictureImage.loadFromFile ("Resources/Textures/button2.png"); 
		tmpI = GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new PictureButton (buttonImage, "Rectangle", font, tmpS, GLOBAL_W / 2 - 9 - H_BUTTON * 3, tmpI, H_BUTTON, H_BUTTON, 30, 30, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Triangle", font, tmpS,  GLOBAL_W / 2 - 6 - H_BUTTON * 2, tmpI, H_BUTTON, H_BUTTON, 30, 30, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Circle", font, tmpS,    GLOBAL_W / 2 - 3 - H_BUTTON,     tmpI, H_BUTTON, H_BUTTON, 30, 30, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Wall", font, tmpS,      GLOBAL_W / 2,                    tmpI, H_BUTTON, H_BUTTON, 30, 30, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Start", font, tmpS,     GLOBAL_W / 2 + 3 + H_BUTTON,     tmpI, H_BUTTON, H_BUTTON, 30, 30, pictureImage, 30, 30); 
		button [NumButton++] = new PictureButton (buttonImage, "Finish", font, tmpS,    GLOBAL_W / 2 + 6 + H_BUTTON * 2, tmpI, H_BUTTON, H_BUTTON, 30, 30, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Save", font, tmpS,      GLOBAL_W / 2 + 9 + H_BUTTON * 3, tmpI, H_BUTTON, H_BUTTON, 30, 30, pictureImage, 30, 30);

		tmpS = player;
		tmpI = NUM_SQUARE * SQUARE / 2 + (W_WIN - NUM_SQUARE * SQUARE) / 4;
		button [NumButton++] = new Button (buttonImage, "Pause", "BackToMenuPl", font, tmpS, GLOBAL_W / 2 + tmpI, GLOBAL_H / 2 - 0 * (H_BUTTON + 6),              W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Static (buttonImage, "Time: 0", "TimePlayer", font, tmpS, GLOBAL_W / 2 - tmpI, GLOBAL_H / 2 - 7 * (H_BUTTON + 6), W_BUTTON, H_BUTTON,    120, 30);
		indexTimePlBut = NumButton - 1;

		tmpS = pause;
		tmpI = GLOBAL_W / 2 + NUM_SQUARE * SQUARE / 2 + (W_WIN - NUM_SQUARE * SQUARE) / 4;
		button [NumButton++] = new Static (buttonImage, "Pause",   "Pause", font, tmpS,    tmpI, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON,    120, 30);
		button [NumButton++] = new Static (buttonImage, "Leave?",  "Leave?", font, tmpS,   tmpI, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON,    120, 30);
		button [NumButton++] = new Button (buttonImage, "No!",     "LeaveNo", font, tmpS,  tmpI, GLOBAL_H / 2 - 0 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Yes.",    "LeaveYes", font, tmpS, tmpI, GLOBAL_H / 2 + 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = reqPass;
		button [NumButton++] = new EditButton (buttonImage, "",               "Edit", font, tmpS,        GLOBAL_W / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new Static (buttonImage, "Enter 4 characters", "RequestPass", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 120, 30);

		tmpS = myLVLs;
		button [NumButton++] = new EditButton (buttonImage, "",                "InputMyLVL", font, tmpS,      GLOBAL_W / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), W_BUTTON, H_BUTTON,    120, 30);
		button [NumButton++] = new Static (buttonImage, "Enter name your LVL", "StaticMyLVL", font, tmpS,     GLOBAL_W / 2, GLOBAL_H / 2 - 4 * (H_BUTTON + 6), W_BUTTON, H_BUTTON,    120, 30);
		button [NumButton++] = new Button (buttonImage, "Back",                "BackToMenuMyLVL", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = selectLVL;
		tmpI = GLOBAL_H / 2 - 4 * (H_BUTTON + 6);
		button [NumButton++] = new Button (buttonImage, "1",          "SelectLVL", font, tmpS,     GLOBAL_W / 2 - 3 * (W_BUTTON) / 8, tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 1, 30,  30);
		button [NumButton++] = new Button (buttonImage, "2",          "SelectLVL", font, tmpS,     GLOBAL_W / 2 - (W_BUTTON) / 8,     tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 2, 30,  30);
		button [NumButton++] = new Button (buttonImage, "3",          "SelectLVL", font, tmpS,     GLOBAL_W / 2 + (W_BUTTON) / 8,     tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 3, 30,  30);
		button [NumButton++] = new Button (buttonImage, "4",          "SelectLVL", font, tmpS,     GLOBAL_W / 2 + 3 * (W_BUTTON) / 8, tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 4, 30,  30);
		button [NumButton++] = new Static (buttonImage, "Select LVL", "SelectStatic", font, tmpS,  GLOBAL_W / 2, GLOBAL_H / 2 - 5 * (H_BUTTON + 6), W_BUTTON,           H_BUTTON,    120, 30);
		button [NumButton++] = new Button (buttonImage, "My lvls",    "My lvls", font, tmpS,       GLOBAL_W / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), W_BUTTON,           H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Back",       "BackToMenuSel", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON,           H_BUTTON, 0, 120, 30);

		tmpS = AdSelectLVL;
		tmpI = (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new EditButton (buttonImage, "", "EditLVL", font, tmpS,        GLOBAL_W / 2, GLOBAL_H / 2 - tmpI, W_BUTTON, H_BUTTON,    120, 30);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToAdminSel", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + tmpI, W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = AdSaveLVL;
		tmpI = (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new EditButton (buttonImage, "", "AdSaveLVL", font, tmpS,       GLOBAL_W / 2, GLOBAL_H / 2 - tmpI, W_BUTTON, H_BUTTON,    120, 30);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToAdminSave", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + tmpI, W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = startLVL;
		tmpI = GLOBAL_W / 2 + NUM_SQUARE * SQUARE / 2 + (W_WIN - NUM_SQUARE * SQUARE) / 4;
		button [NumButton++] = new Static (buttonImage, "Press Escape to leave", "StartLVL", font, tmpS, tmpI, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new Static (buttonImage, "Press any key to start", "StartLVL", font, tmpS,  tmpI, GLOBAL_H / 2 - 0 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 120, 30);
	}

	void initializeLine (){
		
		lines = VertexArray (Lines, (NUM_CELL_Y + NUM_CELL_X + 4) * 2); //массив линий
		int i = 0; //i-счетчик линий занесенных в массив
		for (int k = 0; k < (NUM_CELL_Y + NUM_CELL_X + 2) * 2; k++)
			lines [k].color = Color (128, 128, 128); //30, 30, 30//79,75,5
		for (; i < (NUM_CELL_X + 1) * 2; i += 2){ //создание вертикальных линий
			lines [i].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H + NUM_CELL_Y * EDGE);
		}
		for (int k = 0; i < (NUM_CELL_Y + NUM_CELL_X + 2) * 2; i += 2, k += 2){ //создание горизонтальных линий
			lines [i].position = Vector2f ((float) GLOB_IND_W, (float) EDGE * k / 2 + GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + NUM_CELL_X * EDGE, (float) EDGE * k / 2 + GLOB_IND_H);
		}

		playerLines = VertexArray (Lines, 8);
		for (int i = 0; i < 8; i++)
			playerLines [i].color = Color (30, 30, 30);
		playerLines [0].position = Vector2f ((float) GLOBAL_W / 2 - NUM_SQUARE * SQUARE / 2, (float) GLOBAL_H / 2 - NUM_SQUARE * SQUARE / 2); 
		playerLines [1].position = Vector2f ((float) GLOBAL_W / 2 + NUM_SQUARE * SQUARE / 2, (float) GLOBAL_H / 2 - NUM_SQUARE * SQUARE / 2);

		playerLines [2].position = Vector2f ((float) GLOBAL_W / 2 - NUM_SQUARE * SQUARE / 2, (float) GLOBAL_H / 2 - NUM_SQUARE * SQUARE / 2);
		playerLines [3].position = Vector2f ((float) GLOBAL_W / 2 - NUM_SQUARE * SQUARE / 2, (float) GLOBAL_H / 2 + NUM_SQUARE * SQUARE / 2);

		playerLines [4].position = Vector2f ((float) GLOBAL_W / 2 + NUM_SQUARE * SQUARE / 2, (float) GLOBAL_H / 2 - NUM_SQUARE * SQUARE / 2);
		playerLines [5].position = Vector2f ((float) GLOBAL_W / 2 + NUM_SQUARE * SQUARE / 2, (float) GLOBAL_H / 2 + NUM_SQUARE * SQUARE / 2);

		playerLines [6].position = Vector2f ((float) GLOBAL_W / 2 - NUM_SQUARE * SQUARE / 2, (float) GLOBAL_H / 2 + NUM_SQUARE * SQUARE / 2);
		playerLines [7].position = Vector2f ((float) GLOBAL_W / 2 + NUM_SQUARE * SQUARE / 2, (float) GLOBAL_H / 2 + NUM_SQUARE * SQUARE / 2);
	}

	void initialize (){
		Start.x = GLOB_IND_H; Start.y = GLOB_IND_H + NUM_CELL_Y * EDGE; //инициализируем стартовую точку
		Finish.x = GLOB_IND_W + NUM_CELL_X * EDGE; Finish.y = GLOB_IND_H; //инициализируем финиш

		Image playerImage; //зарузка спрайта игрока
		playerImage.loadFromFile ("Resources/Textures/player.png");
		pl  = new Player (playerImage, Start.x, Start.y, SQUARE, SQUARE, 20, 20); //создание объекта игрок

		Image backgroundImage; //черный фон
		backgroundImage.loadFromFile ("Resources/Textures/background.png");
		plBackground = new Background (backgroundImage, "PlayerBackground", 0, 0, 1000 * (NUM_SQUARE) * SQUARE / 500, 1000 * (NUM_SQUARE) * SQUARE / 500, 1000, 1000); //не важно какие последние 2 параметра
		plBackground -> changeCoord (GLOBAL_W / 2, GLOBAL_H / 2);

		backgroundMusic.openFromFile ("Resources/Music/DJVI_-_Dry_Out.ogg"); //музыка
		backgroundMusic.play (); 
		backgroundMusic.setLoop (true);
		backgroundMusic.setVolume (volumBackMusic);

		buffer.loadFromFile ("Resources/Sounds/sound.ogg"); //звук
		sndClickButt.setBuffer (buffer);
		sndClickButt.setVolume (volSndClickButt);

		changeStates = false; secondPhase = false;
	}

	void initializeWall (){
		wallImage.loadFromFile ("Resources/Textures/wall.png");
		wallImagePL.loadFromFile ("Resources/Textures/wall2.png");
	
		NumWall = 0; //количество стен
		CoordWall = new bool* [NUM_CELL_X];
		for (int i = 0; i < NUM_CELL_X; i++){
			CoordWall [i] = new bool [NUM_CELL_Y];
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		}
		NumBorderWall = 0;
		for (int i = -1; i < 65; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", i, -1, SQUARE, SQUARE, 20, 20);
		for (int i = -1; i < 65; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", i, 32, SQUARE, SQUARE, 20, 20);
		for (int i = 0; i < 32; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", -1, i, SQUARE, SQUARE, 20, 20);
		for (int i = 0; i < 32; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", 64, i, SQUARE, SQUARE, 20, 20);

	}

	Game (){ //конструктор в котором инициализируем основные параметры
		AdOrPlMode = "PlayerMode"; //строка хранящая имя текущего мода игры (игрок или админ)
		strcpy (Pass, "");
		strcpy (fileNameAd, "");
		strcpy (myLVLname, "");
		lvlComplete = false; //показывает завершен ли первый уровень
		playerLVL = false;
		escapeReleased = false;
		PassEnter = false;
		state = menu;
		whichWall = wall;
		CurrentLVL = 1;
		timer = 0;
		indexFinish = -1;
		NumWall = 0;

		readInfo ();
		initialize (); //вызываем остальные инициализации
		initializeButton ();
		initializeLine ();
		initializeWall ();
	}


	void createWalls (){ //создание стен в админ моде
		int tmpX, tmpY;
		int tmp; 
		bool deleted = false;
		bool wallDeleted = false;
		bool circleDeleted = false;
		bool rectangleDeleted = false;
		bool triangleDeleted = false;
		bool saveDeleted = false;
		if (Mouse::isButtonPressed (Mouse::Left))
			if ((posMouse.x >= GLOB_IND_W) && (posMouse.x <= GLOB_IND_W + NUM_CELL_X * EDGE) && (posMouse.y >= GLOB_IND_H) && (posMouse.y <= GLOB_IND_H + NUM_CELL_Y * EDGE))
				if (timer > 0.5){
					timer = 0;	
					tmpX = (int) posMouse.x; tmpX -= GLOB_IND_W; tmp = tmpX % EDGE; tmpX -= tmp; tmpX /= EDGE;
					tmpY = (int) posMouse.y; tmpY -= GLOB_IND_H; tmp = tmpY % EDGE; tmpY -= tmp; tmpY /= EDGE;
					for (int i = 0; i < NumWall; i++){
						if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY)
							if (ArrWall [i] -> name != "Start" && ArrWall [i] -> name != "Finish"){
								deleted = true;
								if (ArrWall [i] -> name == "Wall")             wallDeleted = true;
								else if (ArrWall [i] -> name == "Circle")      circleDeleted = true;
								else if (ArrWall [i] -> name == "Rectangle")   rectangleDeleted = true;
								else if (ArrWall [i] -> name == "Triangle")    triangleDeleted = true;
								else if (ArrWall [i] -> name == "Save")        saveDeleted = true;
								if (i != NumWall - 1)
									*ArrWall [i] = *ArrWall [NumWall - 1];
								if (ArrWall [NumWall - 1] -> name == "Start")
									indexStart = i;
								else if (ArrWall [NumWall - 1] -> name == "Finish")
									indexFinish = i;
								CoordWall [tmpX][tmpY] = false;
								delete ArrWall [NumWall - 1];
								NumWall--;
								break;
							}
					}

					if (whichWall == startW){
						if (tmpX != ArrWall [indexFinish] -> x || tmpY != ArrWall [indexFinish] -> y){
							*ArrWall [indexStart] = *ArrWall [NumWall - 1];

							if (ArrWall [NumWall - 1] -> name == "Finish")
								indexFinish = indexStart;

							delete ArrWall [NumWall - 1];
							CoordWall [tmpX][tmpY] = false;
							NumWall--;

							indexStart = NumWall;
							ArrWall [NumWall++] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE, 20, 20);
							Start.x = tmpX; Start.y = tmpY;
						}
					}
					else if (whichWall == finishW){
						if (tmpX != ArrWall [indexStart] -> x || tmpY != ArrWall [indexStart] -> y){
							*ArrWall [indexFinish] = *ArrWall [NumWall - 1];

							if (ArrWall [NumWall - 1] -> name == "Start")
								indexStart = indexFinish;

							delete ArrWall [NumWall - 1];
							CoordWall [tmpX][tmpY] = false;
							NumWall--;

							indexFinish = NumWall;
							ArrWall [NumWall++] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE, 20, 20);
							Finish.x = tmpX; Finish.y = tmpY;
						}
					}
					else{ 
						if ((tmpX == ArrWall [indexStart] -> x && tmpY == ArrWall [indexStart] -> y) || (tmpX == ArrWall [indexFinish] -> x && tmpY == ArrWall [indexFinish] -> y))
							tmp = 0;
						else {
							if ((whichWall == wall) && !wallDeleted){
								ArrWall [NumWall++] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE, 20, 20);
								CoordWall [tmpX][tmpY] = true;
							}
							else if ((whichWall == rectangleW) && !rectangleDeleted)   ArrWall [NumWall++] = new Wall (wallImage, "Rectangle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							else if ((whichWall == triangleW) && !triangleDeleted)     ArrWall [NumWall++] = new Wall (wallImage, "Triangle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							else if ((whichWall == circleW) && !circleDeleted)         ArrWall [NumWall++] = new Wall (wallImage, "Circle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							else if ((whichWall == saveW) && !saveDeleted)             ArrWall [NumWall++] = new Wall (wallImage, "Save",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
						}
					}
				}	
	}

	void saveLVL (char *tmpC){ //сохранение уровня админом
		ofstream outF (tmpC);
		int tmp = 0;
		outF << NumWall << endl;
		outF << (Start.x - GLOB_IND_W) / EDGE << " " << (Start.y - GLOB_IND_H) / EDGE << endl;
		outF << ArrWall [indexStart] -> x << " " << ArrWall [indexStart] -> y << " Start" << endl;
		outF << ArrWall [indexFinish] -> x << " " << ArrWall [indexFinish] -> y << " Finish" << endl;
		for (int i = 0; i < NumWall; i++){
			if (i != indexStart && i != indexFinish){
				outF << ArrWall [i] -> x << " " << ArrWall [i] -> y;
				if (ArrWall [i] -> name == "Wall")             outF << " Wall" << endl;
				else if (ArrWall [i] -> name == "Rectangle")   outF << " Rectangle" << endl;
				else if (ArrWall [i] -> name == "Circle")      outF << " Circle" << endl;
				else if (ArrWall [i] -> name == "Triangle")    outF << " Triangle" << endl;
				else if (ArrWall [i] -> name == "Save")        outF << " Save" << endl;
			}
		}
	}

	void openLVL_PL (char *tmpName){
		char tmpC [50];
		strcpy (tmpC, tmpName);
		int tmpX, tmpY;

		for (int i = 0; i < NumWall; i++)
			ArrWall [i] -> ~Wall ();
		for (int i = 0; i < NUM_CELL_X; i++)
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		NumAnsw = 0;

		ifstream inF (tmpC);
		inF >> NumWall; 
		inF >> Start.x >> Start.y;
		Start.x = Start.x * EDGE + GLOB_IND_W;
		Start.y = Start.y * EDGE + GLOB_IND_H;

		for (int i = 0; i < NumWall; i++){
			inF >> tmpX >> tmpY >> tmpC;
			ArrWall [i] = new Wall (wallImagePL, tmpC, tmpX, tmpY, SQUARE, SQUARE, 20, 20);

			if (strcmp (tmpC, "Wall") == 0)
				CoordWall [tmpX][tmpY] = true;
			else if (strcmp (tmpC, "Start") == 0)
				indexStart = i;
			else if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX * EDGE + GLOB_IND_W; 
				Finish.y = tmpY * EDGE + GLOB_IND_H; indexFinish = i;
			}
		}
	}

	void openLVL_AD (char *tmpName){
		char tmpC [50];
		strcpy (tmpC, tmpName);
		int tmpX, tmpY;

		for (int i = 0; i < NumWall; i++)
			ArrWall [i] -> ~Wall ();
		for (int i = 0; i < NUM_CELL_X; i++)
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		NumAnsw = 0;

		ifstream inF (tmpC);
		inF >> NumWall; 
		inF >> Start.x >> Start.y;
		Start.x = Start.x * EDGE + GLOB_IND_W;
		Start.y = Start.y * EDGE + GLOB_IND_H;

		for (int i = 0; i < NumWall; i++){
			inF >> tmpX >> tmpY >> tmpC;
			ArrWall [i] = new Wall (wallImage, tmpC, tmpX, tmpY, EDGE, EDGE, 20, 20);

			if (strcmp (tmpC, "Wall") == 0)
				CoordWall [tmpX][tmpY] = true;
			else if (strcmp (tmpC, "Start") == 0)
				indexStart = i;
			else if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX * EDGE + GLOB_IND_W; 
				Finish.y = tmpY * EDGE + GLOB_IND_H; indexFinish = i;
			}
		}
	}

	void inputKeyboard (char *tmpC, bool fictiv){ //ввод с клавиатуры
		if (event.type == Event::KeyPressed){
			if ((strlen (tmpC) < 9 && !fictiv) || (strlen (tmpC) < 4 && fictiv)){
				if (Keyboard::isKeyPressed (Keyboard::A))             strcat (tmpC, "a");
				else if (Keyboard::isKeyPressed (Keyboard::B))        strcat (tmpC, "b");
				else if (Keyboard::isKeyPressed (Keyboard::C))        strcat (tmpC, "c");
				else if (Keyboard::isKeyPressed (Keyboard::D))        strcat (tmpC, "d");
				else if (Keyboard::isKeyPressed (Keyboard::E))        strcat (tmpC, "e");
				else if (Keyboard::isKeyPressed (Keyboard::F))        strcat (tmpC, "f");
				else if (Keyboard::isKeyPressed (Keyboard::G))        strcat (tmpC, "g");
				else if (Keyboard::isKeyPressed (Keyboard::H))        strcat (tmpC, "h");
				else if (Keyboard::isKeyPressed (Keyboard::I))        strcat (tmpC, "i");
				else if (Keyboard::isKeyPressed (Keyboard::J))        strcat (tmpC, "j");
				else if (Keyboard::isKeyPressed (Keyboard::K))        strcat (tmpC, "k");
				else if (Keyboard::isKeyPressed (Keyboard::L))        strcat (tmpC, "l");
				else if (Keyboard::isKeyPressed (Keyboard::M))        strcat (tmpC, "m");
				else if (Keyboard::isKeyPressed (Keyboard::N))        strcat (tmpC, "n");
				else if (Keyboard::isKeyPressed (Keyboard::O))        strcat (tmpC, "o");
				else if (Keyboard::isKeyPressed (Keyboard::P))        strcat (tmpC, "p");
				else if (Keyboard::isKeyPressed (Keyboard::Q))        strcat (tmpC, "q");
				else if (Keyboard::isKeyPressed (Keyboard::R))        strcat (tmpC, "r");
				else if (Keyboard::isKeyPressed (Keyboard::S))        strcat (tmpC, "s");
				else if (Keyboard::isKeyPressed (Keyboard::T))        strcat (tmpC, "t");
				else if (Keyboard::isKeyPressed (Keyboard::U))        strcat (tmpC, "u");
				else if (Keyboard::isKeyPressed (Keyboard::V))        strcat (tmpC, "v");
				else if (Keyboard::isKeyPressed (Keyboard::W))        strcat (tmpC, "w");
				else if (Keyboard::isKeyPressed (Keyboard::X))        strcat (tmpC, "x");
				else if (Keyboard::isKeyPressed (Keyboard::Y))        strcat (tmpC, "y");
				else if (Keyboard::isKeyPressed (Keyboard::Z))        strcat (tmpC, "z");
				else if (Keyboard::isKeyPressed (Keyboard::Period))   strcat (tmpC, ".");
				else if (Keyboard::isKeyPressed (Keyboard::Num0))     strcat (tmpC, "0");
				else if (Keyboard::isKeyPressed (Keyboard::Num1))     strcat (tmpC, "1");
				else if (Keyboard::isKeyPressed (Keyboard::Num2))     strcat (tmpC, "2");
				else if (Keyboard::isKeyPressed (Keyboard::Num3))     strcat (tmpC, "3");
				else if (Keyboard::isKeyPressed (Keyboard::Num4))     strcat (tmpC, "4");
				else if (Keyboard::isKeyPressed (Keyboard::Num5))     strcat (tmpC, "5");
				else if (Keyboard::isKeyPressed (Keyboard::Num6))     strcat (tmpC, "6");
				else if (Keyboard::isKeyPressed (Keyboard::Num7))     strcat (tmpC, "7");
				else if (Keyboard::isKeyPressed (Keyboard::Num8))     strcat (tmpC, "8");
				else if (Keyboard::isKeyPressed (Keyboard::Num9))     strcat (tmpC, "9");
			}
			if (Keyboard::isKeyPressed (Keyboard::BackSpace)){       
				int i = strlen (tmpC);
				if (i > 0){
					char tmpC2 [50];
					strcpy (tmpC2, tmpC);
					strncpy (tmpC, tmpC2, i - 1);
					tmpC [i - 1] = '\0';
				}
			}
		}
			
	}

	void createWay (){
		Coordinate fn, sizeMap, st;
		st.x = (pl -> x - GLOB_IND_W) / EDGE;
		st.y = (pl -> y - GLOB_IND_H) / EDGE;
		fn.x = (Finish.x - GLOB_IND_W) / EDGE;
		fn.y = (Finish.y - GLOB_IND_H) / EDGE;
		pl -> currDir = 0;
		sizeMap.x = NUM_CELL_X;
		sizeMap.y = NUM_CELL_Y;
		outputSearch (CoordWall, fn, st, sizeMap);
	}

	void changeState2 (){
		if (!secondPhase){
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> state == whichStateWas){
				if (button [i] -> changeForm == false){
					secondPhase = true; state = whichStateWill;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == whichStateWill){
							button [i] -> drawThis  = true;
							button [i] -> clearButton ();
						}
						else
							button [i] -> drawThis  = false;
					break;
				}
				button [i] -> reduceButton ();
			}
		}
		else{
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> state == whichStateWill){
					button [i] -> enlargeButton (); 
					if (button [i] -> changeForm == false){
						changeStates = false; secondPhase = false;
						state = whichStateWill;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == state)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
				}
		}
	}

	void changeState (StateList tmpS){
		sndClickButt.play ();
		changeStates = true; whichStateWas = state; whichStateWill = tmpS;
		for (int i = 0; i < NumButton; i++){
			if (button [i] -> state == whichStateWas )
				button [i] -> changeForm = true;
			else if (button [i] -> state == whichStateWill )
				button [i] -> changeForm = true;
		}
	}


	void StateMenu (){
		if (changeStates)
			changeState2 ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> buttClick && button [i] -> name == "Mode" && !changeStates){
					changeState (mode);
					break;
				}
				else if (((button [i] -> buttClick && button [i] -> name == "Go!") || enterReleased) && !changeStates){
					if (AdOrPlMode == "AdminMode"){
						NumWall = 0;
						ArrWall [NumWall++] = new Wall (wallImage, "Start", 0, 0, EDGE, EDGE, 20, 20);
						ArrWall [NumWall++] = new Wall (wallImage, "Finish", 1, 0, EDGE, EDGE, 20, 20);
						indexStart = 0;
						indexFinish = 1;
						whichWall = wall;
						changeState (admin);
					}
					if (AdOrPlMode == "PlayerMode"){  
						writeInfo ();
						changeState (selectLVL);
					}
					break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "Settings" && !changeStates){
					changeState (settings); break;
				}
				else if (((button [i] -> buttClick && button [i] -> name == "Exit") || escapeReleased) && !changeStates){
					writeInfo ();
					changeState (exitt); break;
				}
			}
	}
	void StateMode (){
		if (changeStates)
			changeState2 ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "BackToMenu") || escapeReleased) && !changeStates){
					changeState (menu); break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "AdminMode" && !PassEnter && !changeStates){
					changeState (reqPass); break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "AdminMode" && PassEnter && !changeStates){
					 sndClickButt.play (); break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "PlayerMode" && !changeStates){
					sndClickButt.play (); break;
				}
			}
	}
	void StateAdmin (){
		timer += time;
		if (changeStates)
			changeState2 ();
		createWalls ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> buttClick && button [i] -> name == "SaveAd" && !changeStates){
					changeState (AdSaveLVL); break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "OpenAd" && !changeStates){
					changeState (AdSelectLVL); break;
				}
				else if (((button [i] -> buttClick && button [i] -> name == "BackToMenuAd") || escapeReleased) && !changeStates){
					changeState (menu); timer = 0; break;
				}
			}

			if (event.type == Event::KeyPressed){
				if (Keyboard::isKeyPressed (Keyboard::Num1))        whichWall = rectangleW;
				else if (Keyboard::isKeyPressed (Keyboard::Num2))   whichWall = triangleW;
				else if (Keyboard::isKeyPressed (Keyboard::Num3))   whichWall = circleW;
				else if (Keyboard::isKeyPressed (Keyboard::Num4))   whichWall = wall;
				else if (Keyboard::isKeyPressed (Keyboard::Num5))   whichWall = startW;
				else if (Keyboard::isKeyPressed (Keyboard::Num6))   whichWall = finishW;
				else if (Keyboard::isKeyPressed (Keyboard::Num7))   whichWall = saveW;
			}
	}
	void StatePlayer (){
		if (changeStates)
			changeState2 ();

		if (!changeStates)
			pl -> update (CoordWall);

		for (int i = 0; i < NumWall; i++)
			if ((ArrWall [i] -> x * EDGE + GLOB_IND_W == pl -> x) && (ArrWall [i] -> y * EDGE + GLOB_IND_H == pl -> y)){
				if ((ArrWall [i] -> name == "Rectangle" && pl -> statePl != rectangle) || (ArrWall [i] -> name == "Circle" && pl -> statePl != circle) || (ArrWall [i] -> name == "Triangle" && pl -> statePl != triangle)){
					pl -> changeCoord (Start.x, Start.y);
					changeState (startLVL);
					createWay ();
					break;
				}
				else if (ArrWall [i] -> name == "Save"){
					Start.x = ArrWall [i] -> x * EDGE + GLOB_IND_W;
					Start.y = ArrWall [i] -> y * EDGE + GLOB_IND_H;
					createWay ();
				}
			}

		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				if (button [i] -> name == "TimePlayer"){
					char tmpC [30];
					_itoa ((int) timePl, tmpC, 10);
					button [i] -> updateText (tmpC);
					timePl += time;
				}
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "BackToMenuPl") || escapeReleased) && !changeStates){
					timePl -= time;
					escapeReleased = false;
					changeState (pause);
				}
				else if (((button [i] -> buttClick && button [i] -> name == "lvlComplete") || (lvlComplete && enterReleased)) && !changeStates){
					AllTime += timePl;
					timePl = 0;
					sndClickButt.play (); 
					if (!playerLVL){
						if (CurrentLVL < 4){
							if (PassedLVL < 4)
								PassedLVL++;
							writeInfo ();
							CurrentLVL++; saveLVL (fileNamePl);
							char tmpC [30], *tmpC2;
							tmpC2 = _itoa (CurrentLVL, tmpC, 10);
							char nameFile [30] = "Resources/LVLs/lvl";
							strcat (nameFile, tmpC2);
							strcat (nameFile, ".txt");
							openLVL_PL (nameFile); strcpy (fileNamePl, nameFile);
							pl -> changeCoord (Start.x, Start.y);
							createWay ();
							enterReleased = false;
							changeState (startLVL);
						}
					}
					else{
						playerLVL = false;
						NumAnsw = 0;
						changeState (selectLVL);
						lvlComplete = false;
					}
				}
			}

			if (lvlComplete){
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> name == "lvlComplete"){
						button [i] -> drawThis = true;
						break;
					}
			}
			else{
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> name == "lvlComplete"){
						button [i] -> drawThis = false;
						break;
					}
			}
	}
	void StateSettings (){
		if (changeStates)
			changeState2 ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "BackToMenuSet") || escapeReleased) && !changeStates){
					writeInfo ();
					changeState (menu); break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "MusicSlider" && !changeStates){
					sndClickButt.play (); break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "SoundSlider" && !changeStates){
					sndClickButt.play (); break;
				}
			}
	}
	void StateExitt (){
		if (changeStates)
			changeState2 ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "QuitNo") || escapeReleased) && !changeStates){
					changeState (menu); break;
				}
				else if (((button [i] -> buttClick && button [i] -> name == "QuitYes") || enterReleased) && !changeStates){
					sndClickButt.play (); 
					window -> close (); break;
				}
			}
	}
	void StateReqPass (){
		if (changeStates)
			changeState2 ();
		inputKeyboard (Pass, 1);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> name == "Edit" && !changeStates)
					button [i] -> updateText (Pass);
				if (((button [i] -> buttClick && button [i] -> name == "Edit") || enterReleased) && !changeStates)
					if (!PassEnter){ 
						if (strcmp (Pass, "4329") == 0){
							PassEnter = true;
							writeInfo ();
							AdOrPlMode = "AdminMode"; strcpy (Pass, "");
							changeState (mode);
						}
						else{
							PassEnter = false;
							AdOrPlMode = "PlayerMode"; strcpy (Pass, "");
							changeState (mode);
						}
					}
			}
	}
	void StateSelectLVL (){
		if (changeStates)
			changeState2 ();
		char tmpC2 [30];
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> buttClick && button [i] -> name == "SelectLVL" && !changeStates){
					if (button [i] -> value <= PassedLVL + 1){
						CurrentLVL = button [i] -> value;
						_itoa (button [i] -> value, tmpC2, 10);
						
						char nameFile [30] = "Resources/LVLs/lvl";
						strcat (nameFile, tmpC2);
						strcat (nameFile, ".txt");
						strcpy (fileNamePl, nameFile);
						if (button [i] -> value <= PassedLVL)
							changeState (newGame);
						else{
							openLVL_PL (fileNamePl);
							pl -> changeCoord (Start.x, Start.y);
							pl -> statePl = rectangle;
							//plBackground -> changeCoord (Start.x, Start.y);
							playerLVL = false;
							createWay ();
							changeState (startLVL);
						}
					}
				}
				else if (((button [i] -> buttClick && button [i] -> name == "BackToMenuSel") || escapeReleased) && !changeStates){
					changeState (menu); break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "My lvls" && !changeStates){
					readInfo ();
					changeState (myLVLs); break;
				}
			}
	}
	void StateAdSelectLVL (){
		if (changeStates)
			changeState2 ();
		inputKeyboard (fileNameAd, 0);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> state == AdSelectLVL){
				button [i] -> checkCursor ();
				if (button [i] -> name == "EditLVL" && !changeStates)
					button [i] -> updateText (fileNameAd);
				if (((button [i] -> buttClick && button [i] -> name == "EditLVL") || enterReleased) && !changeStates){
					changeState (admin);
					char tmpC [100] = "Resources/LVLs/";
					ifstream inF ("Resources/LVLs/listLVLs.txt");
					char tmpC2 [30];
					int tmpI;
					inF >> tmpI;
					for (int i = 0; i < tmpI; i++){
						inF >> tmpC2;
						if (strcmp (tmpC2, fileNameAd) == 0){
							strcat (tmpC, fileNameAd);
							strcat (tmpC, ".txt");
							openLVL_AD (tmpC);
							break;
						}
					}
				}
				else if (((button [i] -> buttClick && button [i] -> name == "BackToAdminSel") || escapeReleased) && !changeStates)
					changeState (admin);
			}
	}
	void StateAdSaveLVL (){
		if (changeStates)
			changeState2 ();
		inputKeyboard (fileNameAd, 0);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> state == AdSaveLVL){
				button [i] -> checkCursor ();
				if (button [i] -> name == "AdSaveLVL" && !changeStates)
					button [i] -> updateText (fileNameAd);
				if (((button [i] -> buttClick && button [i] -> name == "AdSaveLVL") || enterReleased) && !changeStates){
					changeState (admin);

					int tmpI; 
					char tmpC2 [100][30]; 
					bool edit = true;
					ifstream inF ("Resources/LVLs/listLVLs.txt");
					inF >> tmpI;
					for (int i = 0; i < tmpI; i++){
						inF >> tmpC2 [i];
						if (strcmp (tmpC2 [i], fileNameAd) == 0)
							edit = false;
					}
					inF.close ();
					if (edit){
						ofstream outF ("Resources/LVLs/listLVLs.txt");
						outF << ++tmpI << endl;
						for (int i = 0; i < tmpI - 1; i++)
							outF << tmpC2 [i] << endl;
						outF << fileNameAd << endl;
						
					}
					if (!edit){
						ofstream outF ("Resources/LVLs/listLVLs.txt");
						outF << tmpI << endl;
						for (int i = 0; i < tmpI; i++)
							outF << tmpC2 [i] << endl;
					}
					char tmpC [100] = "Resources/LVLs/";
					strcat (tmpC, fileNameAd);
					strcat (tmpC, ".txt");
					Start.x = ArrWall [indexStart] -> x;
					Start.y = ArrWall [indexStart] -> y;
					Start.x = Start.x * EDGE + GLOB_IND_W;
					Start.y = Start.y * EDGE + GLOB_IND_H;
					saveLVL (tmpC);
				}
				else if (((button [i] -> buttClick && button [i] -> name == "BackToAdminSave") || escapeReleased) && !changeStates)
					changeState (admin);
			}
	}
	void StatePause (){
		if (changeStates)
			changeState2 ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> state == pause){
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "LeaveYes") || enterReleased) && !changeStates){
					AllTime += timePl;
					timePl = 0;
					NumAnsw = 0;
					writeInfo ();
					saveLVL (fileNamePl);
					changeState (selectLVL);
					lvlComplete = false;
					for (int k = 0; k < NumButton; k++)
						if (button [k] -> name == "TimePlayer"){
							button [k] -> updateText ("0");
							break;
						}
					break;
				}
				if (((button [i] -> buttClick && button [i] -> name == "LeaveNo") || escapeReleased) && !changeStates){
					changeState (player); break;
				}
			}
	}
	void StateStartLVL (){
		if (changeStates)
			changeState2 ();
		pl -> changeFigure ();
		if (anyKeyReleased && !changeStates && !escapeReleased)
			changeState (player);
		else if (escapeReleased && !changeStates){
			AllTime += timePl; timePl = 0;
			NumAnsw = 0; writeInfo ();
			saveLVL (fileNamePl); 
			for (int k = 0; k < NumButton; k++)
				if (button [k] -> name == "TimePlayer"){
					button [k] -> updateText ("0");
					break;
				}
			changeState (selectLVL);
			lvlComplete = false;
		}
	}
	void StateMyLVLs (){
		if (changeStates)
			changeState2 ();
		inputKeyboard (myLVLname, 0);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> name == "InputMyLVL" && !changeStates)
						button [i] -> updateText (myLVLname);
				if (((button [i] -> buttClick && button [i] -> name == "InputMyLVL") || enterReleased) && !changeStates){
					sndClickButt.play (); 
					char tmpC [100] = "Resources/LVLs/";
					bool findLVL = false;
					ifstream inF ("Resources/LVLs/listLVLs.txt");
					char tmpC2 [30];
					int tmpI;
					inF >> tmpI;
					for (int j = 0; j < tmpI; j++){
						inF >> tmpC2;
						if (strcmp (tmpC2, myLVLname) == 0){
							if (strstr (myLVLname, "lvl") == NULL || strcspn (myLVLname, "1234") == NULL){
								strcat (tmpC, myLVLname);
								strcat (tmpC, ".txt");
								openLVL_PL (tmpC);
								strcpy (fileNamePl, tmpC);
								pl -> changeCoord (Start.x, Start.y);
								pl -> statePl = rectangle;
								//plBackground -> changeCoord (Start.x, Start.y);

								playerLVL = true;
								createWay ();
								findLVL = true;
								changeState (startLVL);
							}
							break;
						}
					}

					if (!findLVL)
						for (int k = 0; k < NumButton; k++)
							if (button [k] -> name == "InputMyLVL")
								strcpy (myLVLname, "");
				}
				else if (((button [i] -> buttClick && button [i] -> name == "BackToMenuMyLVL") || escapeReleased) && !changeStates)
					changeState (selectLVL);
			}
	}
	void StateNewGame (){
		if (changeStates)
			changeState2 ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "Continue Game") || escapeReleased) && !changeStates){
					openLVL_PL (fileNamePl);
					pl -> changeCoord (Start.x, Start.y);
					pl -> statePl = rectangle;
					//plBackground -> changeCoord (Start.x, Start.y);
					playerLVL = false;
					createWay ();
					changeState (startLVL);
				}
				else if (((button [i] -> buttClick && button [i] -> name == "New Game") || enterReleased) && !changeStates){
					openLVL_PL (fileNamePl);
					Start.x = ArrWall [indexStart] -> x;
					Start.y = ArrWall [indexStart] -> y;
					Start.x = Start.x * EDGE + GLOB_IND_W;
					Start.y = Start.y * EDGE + GLOB_IND_H;
					pl -> changeCoord (Start.x, Start.y);
					pl -> statePl = rectangle;
					//plBackground -> changeCoord (Start.x, Start.y);
					playerLVL = false;
					createWay ();
					changeState (startLVL);
				}
			}
	}

	void update (){
		switch (state){
		case menu:
			StateMenu ();
			break;
		case mode:
			StateMode ();
			break;
		case admin:
			StateAdmin ();
			break;
		case player:
			//StatePlayer ();
			break;
		case settings:
			StateSettings ();
			break;
		case exitt:
			StateExitt ();
			break;
		case reqPass:
			StateReqPass ();
			break;
		case selectLVL:
			StateSelectLVL ();
			break;
		case AdSelectLVL:
			StateAdSelectLVL ();
			break;
		case AdSaveLVL:
			StateAdSaveLVL ();
			break;
		case pause:
			StatePause ();
			break;
		case startLVL:
			StateStartLVL ();
			break;
		case myLVLs:
			StateMyLVLs ();
			break;
		case newGame:
			StateNewGame ();
			break;
		}
	}
};

int main (){
	System system;
	view.reset (FloatRect (0, 0, (float) system.W_WIN, (float) system.H_WIN)); //создание камеры
	setCoordinateForView ((float) system.GLOBAL_W / 2, (float) system.GLOBAL_H / 2); //двигаем камеру

	
	Game game;
	//system.window = new RenderWindow (VideoMode (system.W_WIN, system.H_WIN), "LABYRINTH PRO", Style::Fullscreen, ContextSettings (0, 0, 1)); //создание окна
	system.window = new RenderWindow (VideoMode (system.W_WIN, system.H_WIN), "LABYRINTH PRO"); //создание окна
	bool isUpdate = false;

	while (system.window -> isOpen ()){
		system.time = system.clock.getElapsedTime().asSeconds(); //время каждый раз обновляется
		system.clock.restart ().asSeconds ();
		system.FPS = (int) (1.0 / system.time);
		char tmpC [30];
		_itoa (system.FPS, tmpC, 10);
		game.button [game.indexFPSBut] -> updateText (tmpC);

		system.mousePosWin = Mouse::getPosition (system.window [0]); //координаты мыши относ. окна
		system.posMouse = system.window -> mapPixelToCoords (system.mousePosWin); //координаты мыши относ. карты


		while (system.window -> pollEvent (system.event)){ //заходит в цикл когда что-то происходит (игрок двигает мышкой или нажимает на клавиши)
			if (system.event.type == Event::Closed) //окно закрывается когда нажали крестик
				system.window -> close (); 
			isUpdate = true;
			game.update ();

			if (system.event.type == Event::KeyReleased && system.event.key.code == 36) game.escapeReleased = true; //флаг, о том нажат ли ескейп
			else game.escapeReleased = false;
			if (system.event.type == Event::KeyReleased && system.event.key.code == 58) game.enterReleased = true; //флаг, о том нажат ли ентер
			else game.enterReleased = false; 
			if (system.event.type == Event::KeyReleased) game.anyKeyReleased = true; //флаг, о том нажата ли какая-либо кнопка
			else game.anyKeyReleased = false; 
		}		
		if (!isUpdate){ //обновляем игру если не зашли в предыдущий while
			game.update ();
			game.escapeReleased = false;
			game.enterReleased = false;
			game.anyKeyReleased = false;
		}
		if (game.state == player) //обновляем игрока всегда
			game.StatePlayer ();
		isUpdate = false;
		game.draw ();
	}
	return 0;
}