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


enum StateList {menu, mode, admin, player, backToMenu, settings, exitt, reqPass, selectLVL, AdSelectLVL, AdSaveLVL, completeLVL, pause, startLVL}; //основное перечесление которое отвечает за состояние игры
enum StatePlayer {rectangle, triangle, circle};
enum CreateWall {rectangleW, triangleW, circleW, wall, finishW, startW};


class System{ //основной класс игры, в котором хранится все самое выжное
public:
	static Vector2i mousePosWin; //координаты мыши относ. окна
	static Vector2f posMouse; //координаты мыши относ. карты

	static RenderWindow *window; //окно, в котором запускается игра

	static Event event; //событие

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

	static double speed;

	int GLOBAL_W;
	int GLOBAL_H;
	int W_WIN;
	int H_WIN;
	int EDGE;
	int NUM_CELL_X; 
	int NUM_CELL_Y;
	int NUM_H_LINE;
	int NUM_V_LINE;
	int W_BUTTON;
	int H_BUTTON;
	int GLOB_IND_W; 
	int GLOB_IND_H;
public:
	System (){
		GLOBAL_W = 4000; //2240 //максимальное разрешение экрана в котором игра пойдет, ширина
		GLOBAL_H = 3000; //1280 //высота
		W_WIN = GetSystemMetrics (0); //GetSystemMetrics (0) // самое маленькое 1366 разрешение на котором пойдет игра, ширина
		H_WIN = GetSystemMetrics (1); //GetSystemMetrics(1) // самое маленькое 768, высота
		EDGE = 20; //размер одной клетки
		NUM_CELL_X = 64; //количество клеток уровня по ширине
		NUM_CELL_Y = 32; //количество клеток уровня по высоте
		NUM_H_LINE = (NUM_CELL_Y + 1); //количество горизонтальных прямых, которые создают поле
		NUM_V_LINE = (NUM_CELL_X + 1); //количество вертикальных прямых, которые создают поле
		W_BUTTON = 120 + (W_WIN - 1360) / 5; //ширина кнопки
		H_BUTTON = 30 + (H_WIN - 760) / 10; //высота кнопки

		while (1){
			if ((H_WIN - NUM_CELL_Y * EDGE) / 2 < H_BUTTON + 40 || (W_WIN - NUM_CELL_X * EDGE) / 2 < 60)
				break;
			EDGE++;
		}

		GLOB_IND_W = (GLOBAL_W - NUM_CELL_X * EDGE) / 2; //отступ по ширине, с которого начинается область которую видит игрок
		GLOB_IND_H = (GLOBAL_H - NUM_CELL_Y * EDGE) / 2; //отступ по высоте, с которого начинается область которую видит игрок
		speed = (double) EDGE / 13; // /13
	}
};

//инициализируем всё, нужно что б если переменная менялась где-то, то она менялась и во всех классах наследниках
Vector2i System::mousePosWin;
Vector2f System::posMouse;

RenderWindow* System::window;

Event System::event;

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

double System::speed;


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
		shape.setPosition (Vector2f ((float) x, (float) y));
		shape.setTexture (&texture);
		shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));	
		
	}

	virtual void draw () = 0;
};


class Wall : public Body{ //класс стены
public:
	bool drawThis; //рисовать ли стену
public:
	Wall (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){ //конструктор с именем
		drawThis = true; 
		shape.setPosition ((float) x * EDGE + GLOB_IND_W, (float) y * EDGE + GLOB_IND_H);
		if (name == "Wall") //стена обычная
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		if (name == "Finish") //куда надо игроку идти
			shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		if (name == "Start") //откуда игрок будет начинать, сделано для удобства создания карт, что б админ видел где игрок начинает и где заканчиват
			shape.setTextureRect (IntRect (0, hTexture * 3, wTexture, hTexture));
		if (name == "HelpWall") //вспомогательные "стены" которые показывают правильный путь
			shape.setTextureRect (IntRect (0, hTexture * 4, wTexture, hTexture));
		if (name == "Circle")
			shape.setTextureRect (IntRect (0, hTexture * 5, wTexture, hTexture));
		if (name == "Rectangle")
			shape.setTextureRect (IntRect (0, hTexture * 6, wTexture, hTexture));
		if (name == "Triangle")
			shape.setTextureRect (IntRect (0, hTexture * 7, wTexture, hTexture));
	}

	void draw (){ 
		window -> draw (shape);
	}
};

class Background : public Body{ //класс фонового изображения
public:
	Background (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){ }

	void changeCoord (int x2, int y2){ //функция изменения координат черного заднего фона (центр фона находится где центр игрока)
		x = x2 + EDGE / 2; y = y2 + EDGE / 2;
		int left, top;
		left = x - GLOB_IND_W;
		top = y - GLOB_IND_H;
		shape.setTextureRect (IntRect (1000 - left, 500 - top, NUM_CELL_X * EDGE, NUM_CELL_Y * EDGE));
		shape.setPosition ((float) GLOB_IND_W, (float) GLOB_IND_H);
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
	double xx, yy;
public:
	Player (Image &image, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, "Player", X, Y, W, H, WTexture, HTexture){ //конструктор без имени
	    tmpX = x; tmpY = y; xx = (double) x; yy = (double) y;
	    playerMove = false;
		statePl = rectangle; shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		currDir = 0;
	}

	void update (bool **CoordWall){
		//может быть нажата одновременно только одна клавиша
		if (currDir < NumMoves && !playerMove){
			if (Direction [currDir] == 4) tmpY = y - EDGE; //запоминаем координаты куда мы должы придти
			else if (Direction [currDir] == 1) tmpY = y + EDGE; //запоминаем координаты куда мы должы придти
			else if (Direction [currDir] == 2) tmpX = x - EDGE; //запоминаем координаты куда мы должы придти 
			else if (Direction [currDir] == 3) tmpX = x + EDGE; //запоминаем координаты куда мы должы придти
			currDir++; playerMove = true;
			xx = (double) x; yy = (double) y;
		}

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

		if (playerMove){ //проверяем, нет ли стены на том месте куда мы хотим перейти
			if (abs (xx - tmpX) < 0.3 && abs (yy - tmpY) < 0.3){ //если мы попали туда куда хотели, то игрок не движется
				playerMove = false; 
				if (x < tmpX)
					x = (int) xx + 1;
				if (x > tmpX)
					x = (int) xx;
				if (y < tmpY)
					y = (int) yy + 1;
				if (y > tmpY)
					y = (int) yy;
				xx = (double) x; yy = (double) y;
			}
			else{
				if (x < tmpX){
					xx += speed; //скорость равна двум пикселям
					x = (int) xx;
				}
				if (x > tmpX){
					xx -= speed;
					x = (int) xx;
				}
				if (y < tmpY){
					yy += speed;
					y = (int) yy;
				}
				if (y > tmpY){
					yy -= speed;
					y = (int) yy;
				}
			}
		}

		if (x == Finish.x && y == Finish.y) //есди мы достигли финиша, то будет показана кнопка, свидетельствующая об этом
			lvlComplete = true;
		else
			lvlComplete = false; 
		shape.setPosition ((float) x, (float) y); //устанавливаем позицию игрока
	}

	void changeCoord (int x2, int y2){ //функция нужна для перемещения игрока в нужную координату (нужно при открытии уровня игркоом)
		x = x2; y = y2; shape.setPosition ((float) x, (float) y);
		tmpX = x; tmpY = y; xx = (double) x; yy = (double) y;
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

	void draw (){ 
		window -> draw (shape);
	}
};


class BodyButton : public Body{ //тело кнопок
public:
	mcText *text; //текст который выводится на кнопке
	bool drawThis, buttPressed, buttClick; //рисовать ли кнопку, нажата ли кнопка и кликнули ли по кнопке. Клик- это нажать и отпустить кнопку когда курсор мыши на кнопке
	Font font; //шрифт
	StateList state; //каждая кнопка кроме имени имеет группу к которой она относится
	int value; //значение кнопки
	float xText, yText; //координаты текста
public:
	BodyButton (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    Body (image, Name, X, Y, W, H, WTexture, HTexture){
	    font = Font; drawThis = false; buttClick = false; 
		buttPressed = false; state = State;
		text = new mcText (&font); //создаем текст который будет отображаться на кнопке
		text -> changeSize (30); //размер текста
		if (name != "Pause" && name != "Leave?" && name != "StartLVL")
			text -> add (Text);
		else
			text -> add (Text, Color::White);
		float tmp = (float) Text.getSize (); //получаем длинну текста в символах
		tmp /= 2;
		xText = (float) x - tmp * 14;
		yText = (float) y - h / 2 - 5;
		text -> setPosition (xText, yText); //распологаем текст по кнопке
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	void draw (){ }

	virtual void checkCursor () = 0;

	virtual void updateText (char *Pass) = 0;
};

class Button : public BodyButton{
public:
	Button (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int Value, int WTexture, int HTexture) : 
		    BodyButton (image, Text, Name, Font, State, X, Y, W, H, WTexture, HTexture){
		value = Value;
		if (state == menu)
			drawThis = true;
		else
			drawThis = false;
		shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));

		//т.к. размерыьбукв разный, двигаем текст на кнопках, что б он находился ровно по кнопке
		if (name == "Mode")
			text -> setPosition (xText - 12, yText);
		if (name == "Go!")
			text -> setPosition (xText - 6, yText);
		if (name == "QuitNo")
			text -> setPosition (xText - 5, yText);
		if (name == "BackToMenuSet")
			text -> setPosition (xText - 7, yText);
		if (name == "BackToMenu")
			text -> setPosition (xText - 7, yText);
		if (name == "AdminMode")
			text -> setPosition (xText - 10, yText);
		if (name == "PlayerMode")
			text -> setPosition (xText - 4, yText);
		if (name == "BackToMenuSel")
			text -> setPosition (xText - 7, yText);
		if (name == "BackToMenuPl")
			text -> setPosition (xText - 7, yText);
		if (name == "HelpPl")
			text -> setPosition (xText - 3, yText);
		if (name == "BackToMenuAd")
			text -> setPosition (xText - 7, yText);
		if (name == "SaveAd")
			text -> setPosition (xText - 6, yText);
		if (name == "OpenAd")
			text -> setPosition (xText - 11, yText);
	}

	void draw (){
		window -> draw (shape);
		text -> draw (window);
	}

	void checkCursor (){ //функция проверки на нажатие кнопки или наведением курсора на кнопку
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
		text -> clear ();
		text -> changeSize (30); //размер текста
		text -> add (Pass);
		float tmp = (float) strlen (Pass); //получаем длинну текста в символах
		tmp /= 2;
		xText = (float) x - tmp * 14;
		yText = (float) y - h / 2 - 5;
		text -> setPosition (xText, yText); //распологаем текст по кнопке
		if (name == "Edit"){ //числа имееют другую ширину чем буквы, поэтому сдвигаем немного
			xText -= 5;
			text -> setPosition (xText, yText); 
		}
	}
};

class Static : public BodyButton{
public:
	Static (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    BodyButton (image, Text, Name, Font, State, X, Y, W, H, WTexture, HTexture){ 
		if (name == "RequestPass")
			text -> setPosition (xText - 5, yText);
		if (name == "SelectStatic")
			text -> setPosition (xText - 8, yText);
		if (name == "Quit game?")
			text -> setPosition (xText - 10, yText);
		if (name == "VolMusic")
			text -> setPosition (xText - 8, yText);
		if (name == "VolSound")
			text -> setPosition (xText - 10, yText);
		if (name == "Pause")
			text -> setPosition (xText - 9, yText);
		if (name == "Leave?")
			text -> setPosition (xText - 6, yText);
		if (name == "StartLVL")
			text -> setPosition (xText - 10, yText);
	}

	void draw (){
		text -> draw (window);
	}

	void checkCursor (){ }

	void updateText (char *Pass){
		text -> clear ();
		text -> changeSize (30); //размер текста
		char tmpC [40];
		strcpy (tmpC, "Time: ");
		strcat (tmpC, Pass);
		text -> add (tmpC, Color::Red);
		float tmp = (float) strlen (Pass); //получаем длинну текста в символах
		tmp /= 2;
		xText = (float) x - tmp * 14;
		yText = (float) y - h / 2 - 5;
		text -> setPosition (xText, yText); //распологаем текст по кнопке
	}
};

class HorizontScrollBar : public BodyButton{
public:
	int leftBorder, rightBorder; //левая и правая граница по которой может перемещаться кнопка (не вся доступная область, т.к. кнопка имеет ширину)
public:
	HorizontScrollBar (Image &image, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int tmpBordL, int tmpBordR, int WTexture, int HTexture) : 
		    BodyButton (image, "", Name, Font, State, X, Y, W, H, WTexture, HTexture){ 
        leftBorder = tmpBordL; rightBorder = tmpBordR;
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
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture)); //если наведен курсор на мышку, то кнопка меняет текстуру
		else
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
	}

	void updateText (char *Pass){ }
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
        pictureT.loadFromImage (Ipicture);
		wPicture = WPicture; hPicture = HPicture;
		picture.setSize (Vector2f ((float) w, (float) h));
		picture.setPosition (Vector2f ((float) x, (float) y));
		picture.setTexture (&pictureT);
		picture.setTextureRect (IntRect (0, 0, wPicture, hPicture));	
		picture.setOrigin ((float) w / 2, (float) h / 2);

		if (name == "Circle") typeWall = circleW;
		if (name == "Rectangle") typeWall = rectangleW;
		if (name == "Triangle") typeWall = triangleW;
		if (name == "Start") typeWall = startW;
		if (name == "Wall") typeWall = wall;
		if (name == "Finish") typeWall = finishW;
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
			shape.setTextureRect (IntRect (0, 30, wTexture, hTexture)); //если наведен курсор на мышку, то кнопка меняет текстуру
		}
		else{
			buttPressed = false; //если курсор не на мыши то кнопка обычного вида
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
		}		

		if (typeWall == whichWall)
			picture.setTextureRect (IntRect (0, hPicture, wPicture, hPicture));
		else
			picture.setTextureRect (IntRect (0, 0, wPicture, hPicture));
	}

	void updateText (char *Pass){ }
};


class Game : public System{ //вся механика и инициализация игры в этом классе
public:
	StateList state; //состояние игры
	int CurrentLVL; //текущий уровень
	bool PassEnter; //введен ли пароль игроком от админ мода
	char Pass [30]; //пароль
	int PassedLVL; //сколько пройдено уровней
	bool escapeReleased; //флаг равен 1 если ескейп отпустили (ну его нажали, а потом отпустили)

	Image wallImage; //загрузка спрайта стен
	int NumWall; //количество стен
	Wall *ArrWall [10000]; //массив стен
	bool **CoordWall; //координаты стен
	int indexFinish; //индекс финиша (что б долго не искать)

	VertexArray lines; //линии которые в админ моде рисуются, что б легче было создавтаь уровни
	VertexArray playerLines;

	Background *plBackground; //фоновое изображение, черное, которые закрывает лабаринт

	Player *pl; //игрок
	mcText *timePlText;
	float timePl;
	float AllTime;
	Coordinate fn, sizeMap, st;

	int NumButton; //количество кнопок
	BodyButton *button [100]; //массив кнопок

	char fileNameAd [50]; //имя файла открытого админом
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

		if (state == admin || state == player || state == AdSelectLVL || state == AdSaveLVL || state == pause || state == startLVL){
			if (state == player || state == pause || state == startLVL)
				window -> draw (playerLines);
			else
				window -> draw (lines); //рисую массив линий
			
			for (int j = 0; j < NumWall; j++) //рисую стены
				if (ArrWall [j] -> drawThis)
					ArrWall [j] -> draw ();
		}
		
		if (state == player || state == pause || state == startLVL){ //фон
			plBackground -> draw ();
		}

		if (state == admin || state == player || state == pause || state == startLVL) //рисуем финиш поверх черного фона
			if (indexFinish != -1)
				ArrWall [indexFinish] -> draw ();

		if (state == pause || state == startLVL) //именно здесь рисуем игрока, что б он был не поверх текста на кнопок и их самих
			pl -> draw ();

		for (int i = 0; i < NumButton; i++) //рисую кнопки
			if (button [i] -> drawThis)
				button [i] -> draw ();

		if (state == player)
			pl -> draw (); //рисую игрока

		window -> display ();
	}


	void initializeButton (){
		Font font;
		font.loadFromFile ("Resources/Fonts/modeka.otf");

		Image buttonImage; //загрузка спрайта стен
		buttonImage.loadFromFile ("Resources/Textures/button.png");

		StateList tmpS;

		NumButton = 0;

		tmpS = menu;
		button [NumButton++] = new Button (buttonImage, "Go!", "Go!", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Mode", "Mode", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 50, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Setting", "Settings", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 100, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Exit", "Exit", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 150, W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = exitt;
		button [NumButton++] = new Static (buttonImage, "Quit game?", "Quit game?", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 100, W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new Button (buttonImage, "No!", "QuitNo", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 150, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Yes.", "QuitYes", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 200, W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = settings;
		button [NumButton++] = new Static (buttonImage, "Sound", "VolSound", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 50, W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new HorizontScrollBar (buttonImage, "SoundSlider", font, tmpS, GLOBAL_W / 2 - 120 / 2 + 10, GLOB_IND_H + EDGE * 7 + 50, 20, H_BUTTON, GLOBAL_W / 2 - W_BUTTON / 2 + 10, GLOBAL_W / 2 - W_BUTTON / 2 + W_BUTTON - 10, 20, 30);
		button [NumButton++] = new Static (buttonImage, "Music", "VolMusic", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 100, W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new HorizontScrollBar (buttonImage, "MusicSlider", font, tmpS, GLOBAL_W / 2 - 120 / 2 + 10, GLOB_IND_H + EDGE * 7 + 100, 20, H_BUTTON, GLOBAL_W / 2 - W_BUTTON / 2 + 10, GLOBAL_W / 2 - W_BUTTON / 2 + W_BUTTON - 10, 20, 30);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuSet", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 150, W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = mode;
		button [NumButton++] = new Button (buttonImage, "Player", "PlayerMode", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Admin", "AdminMode", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 50, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenu", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 100, W_BUTTON, H_BUTTON, 0, 120, 30);

		Image pictureImage; //загрузка спрайта стен
		tmpS = admin;
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuAd", font, tmpS, GLOBAL_W / 2 - W_WIN / 4, GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Open", "OpenAd", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Save", "SaveAd", font, tmpS, GLOBAL_W / 2 + W_WIN / 4, GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/circle.png");
		button [NumButton++] = new PictureButton (buttonImage, "Circle", font, tmpS, GLOBAL_W / 2 - 18 - W_BUTTON / 2, GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4, 30, 30, EDGE, EDGE, pictureImage, 30, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/rectangle.png"); 
		button [NumButton++] = new PictureButton (buttonImage, "Rectangle", font, tmpS, GLOBAL_W / 2 - 84 - W_BUTTON / 2, GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4, 30, 30, EDGE, EDGE, pictureImage, 30, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/triangle.png"); 
		button [NumButton++] = new PictureButton (buttonImage, "Triangle", font, tmpS, GLOBAL_W / 2 - 51 - W_BUTTON / 2, GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4, 30, 30, EDGE, EDGE, pictureImage, 30, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/wall.png"); 
		button [NumButton++] = new PictureButton (buttonImage, "Wall", font, tmpS, GLOBAL_W / 2 + W_BUTTON / 2 + 18, GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4, 30, 30, EDGE, EDGE, pictureImage, 30, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/start.png"); 
		button [NumButton++] = new PictureButton (buttonImage, "Start", font, tmpS, GLOBAL_W / 2 + 51 + W_BUTTON / 2, GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4, 30, 30, EDGE, EDGE, pictureImage, 30, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/finish.png"); 
		button [NumButton++] = new PictureButton (buttonImage, "Finish", font, tmpS, GLOBAL_W / 2 + 84 + W_BUTTON / 2, GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4, 30, 30, EDGE, EDGE, pictureImage, 30, 30);

		tmpS = player;
		button [NumButton++] = new Button (buttonImage, "Pause", "BackToMenuPl", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
		//button [NumButton++] = new Button (buttonImage, "Help", "HelpPl", font, tmpS, GLOBAL_W / 2 + W_WIN / 6, GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Static (buttonImage, "", "TimePlayer", font, tmpS, GLOBAL_W / 2 + EDGE * NUM_CELL_X / 2 - W_BUTTON / 2, GLOBAL_H / 2 - EDGE * NUM_CELL_Y / 2 - 30, W_BUTTON, H_BUTTON, 120, 30);

		tmpS = pause;
		button [NumButton++] = new Static (buttonImage, "Pause", "Pause", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 50, W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new Static (buttonImage, "Leave?", "Leave?", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 100, W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new Button (buttonImage, "No!", "LeaveNo", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 150, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Yes.", "LeaveYes", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 200, W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = reqPass;
		button [NumButton++] = new EditButton (buttonImage, "", "Edit", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 50, W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new Static (buttonImage, "Enter 4 characters", "RequestPass", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7, W_BUTTON, H_BUTTON, 120, 30);

		tmpS = selectLVL;
		button [NumButton++] = new Button (buttonImage, "1", "SelectLVL", font, tmpS, GLOBAL_W / 2 - 3 * (W_BUTTON) / 8, GLOB_IND_H + EDGE * 7, (W_BUTTON - 4) / 4, H_BUTTON, 1, 29, 30);
		button [NumButton++] = new Button (buttonImage, "2", "SelectLVL", font, tmpS, GLOBAL_W / 2 - (W_BUTTON) / 8, GLOB_IND_H + EDGE * 7, (W_BUTTON - 4) / 4, H_BUTTON, 2, 29, 30);
		button [NumButton++] = new Button (buttonImage, "3", "SelectLVL", font, tmpS, GLOBAL_W / 2 + (W_BUTTON) / 8, GLOB_IND_H + EDGE * 7, (W_BUTTON - 4) / 4, H_BUTTON, 3, 29, 30);
		button [NumButton++] = new Button (buttonImage, "4", "SelectLVL", font, tmpS, GLOBAL_W / 2 + 3 * (W_BUTTON) / 8, GLOB_IND_H + EDGE * 7, (W_BUTTON - 4) / 4, H_BUTTON, 4, 29, 30);
		button [NumButton++] = new Static (buttonImage, "Select LVL", "SelectStatic", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 - 50, W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuSel", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 50, W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = AdSelectLVL;
		button [NumButton++] = new EditButton (buttonImage, "", "EditLVL", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 120, 30);

		tmpS = AdSaveLVL;
		button [NumButton++] = new EditButton (buttonImage, "", "AdSaveLVL", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 120, 30);

		tmpS = startLVL;
		button [NumButton++] = new Static (buttonImage, "Press Escape to leave", "StartLVL", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 50, W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new Static (buttonImage, "Press Enter to continue", "StartLVL", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 100, W_BUTTON, H_BUTTON, 120, 30);

		tmpS = completeLVL;
		button [NumButton++] = new Button (buttonImage, "End lvl", "lvlComplete", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
	}

	void initializeLine (){
		lines = VertexArray (Lines, (NUM_H_LINE + NUM_V_LINE + 2) * 2); //массив линий
		int i = 0; //i-счетчик линий занесенных в массив
		for (; i < NUM_V_LINE * 2; i += 2){ //создание вертикальных линий
			lines [i].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H + NUM_CELL_Y * EDGE);
		}
		for (int k = 0; i < (NUM_H_LINE + NUM_V_LINE) * 2; i += 2, k += 2){ //создание горизонтальных линий
			lines [i].position = Vector2f ((float) GLOB_IND_W, (float) EDGE * k / 2 + GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + NUM_CELL_X * EDGE, (float) EDGE * k / 2 + GLOB_IND_H);
		}

		playerLines = VertexArray (Lines, 8);
		playerLines [0].position = Vector2f ((float) GLOB_IND_W - 1, (float) GLOB_IND_H - 1);
		playerLines [1].position = Vector2f ((float) GLOB_IND_W + NUM_CELL_X * EDGE + 1, (float) GLOB_IND_H - 1);

		playerLines [2].position = Vector2f ((float) GLOB_IND_W - 1, (float) GLOB_IND_H - 1);
		playerLines [3].position = Vector2f ((float) GLOB_IND_W - 1, (float) GLOB_IND_H + NUM_CELL_Y * EDGE + 1);

		playerLines [4].position = Vector2f ((float) GLOB_IND_W + NUM_CELL_X * EDGE + 1, (float) GLOB_IND_H - 1);
		playerLines [5].position = Vector2f ((float) GLOB_IND_W + NUM_CELL_X * EDGE + 1, (float) GLOB_IND_H + NUM_CELL_Y * EDGE + 1);

		playerLines [6].position = Vector2f ((float) GLOB_IND_W - 1, (float) GLOB_IND_H + NUM_CELL_Y * EDGE + 1);
		playerLines [7].position = Vector2f ((float) GLOB_IND_W + NUM_CELL_X * EDGE + 1, (float) GLOB_IND_H + NUM_CELL_Y * EDGE + 1);
	}

	void initialize (){
		Start.x = GLOB_IND_H; Start.y = GLOB_IND_H + NUM_CELL_Y * EDGE; //инициализируем стартовую точку
		Finish.x = GLOB_IND_W + NUM_CELL_X * EDGE; Finish.y = GLOB_IND_H; //инициализируем финиш

		state = menu; //инициализируем состояние

		Image playerImage; //зарузка спрайта игрока
		playerImage.loadFromFile ("Resources/Textures/player.png");
		pl  = new Player (playerImage, Start.x, Start.y, EDGE, EDGE, 20, 20); //создание объекта игрок

		timer = 0;

		Image backgroundImage; //черный фон
		backgroundImage.loadFromFile ("Resources/Textures/BackGround/background.png");
		plBackground = new Background (backgroundImage, "PlayerBackground", 0, 0, NUM_CELL_X * EDGE, NUM_CELL_Y * EDGE, 0, 0); //не важно какие последние 2 параметра

		readInfo ();

		strcpy (fileNameAd, "");

		backgroundMusic.openFromFile ("Resources/Music/DJVI_-_Dry_Out.ogg"); //музыка
		backgroundMusic.play (); 
		backgroundMusic.setLoop (true);
		backgroundMusic.setVolume (volumBackMusic);

		buffer.loadFromFile ("Resources/Sounds/button-30.wav"); //звук
		sndClickButt.setBuffer (buffer);
		sndClickButt.setVolume (volSndClickButt);

		escapeReleased = false;
		whichWall = wall;


		Font font;
		font.loadFromFile ("Resources/Fonts/modeka.otf");
		timePlText = new mcText (&font); //создаем текст который будет отображаться на кнопке
		timePlText -> changeSize (30); //размер текста
		timePlText -> add ("", Color::Red);
		timePlText -> setPosition ((float) GLOBAL_W / 2 + EDGE * NUM_CELL_X / 2 - 50, (float) GLOBAL_H / 2 - EDGE * NUM_CELL_Y / 2 - 30); //распологаем текст по кнопке
		timePl = 0;

		sizeMap.x = NUM_CELL_X;
		sizeMap.y = NUM_CELL_Y;
	}

	void initializeWall (){
		wallImage.loadFromFile ("Resources/Textures/wall.png");
	
		NumWall = 0; //количество стен
		CoordWall = new bool* [NUM_CELL_X];
		for (int i = 0; i < NUM_CELL_X; i++){
			CoordWall [i] = new bool [NUM_CELL_Y];
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		}
	}

	Game (){ //конструктор в котором инициализируем основные параметры
		AdOrPlMode = "PlayerMode"; //строка хранящая имя текущего мода игры (игрок или админ)
		lvlComplete = false; //показывает завершен ли первый уровень
		CurrentLVL = 1;
		PassEnter = false;
		strcpy (Pass, "");
		state = menu;
		timer = 0;
		indexFinish = -1;

		initialize (); //вызываем остальные инициализации
		initializeButton ();
		initializeLine ();
		initializeWall ();
	}


	void createWalls (){ //создание стен в админ моде
		int tmpX, tmpY;
		int tmp; 
		bool wallDeleted = false;
		bool circleDeleted = false;
		bool rectangleDeleted = false;
		bool triangleDeleted = false;
		if (Mouse::isButtonPressed (Mouse::Left))
			if ((posMouse.x >= GLOB_IND_W) && (posMouse.x <= GLOB_IND_W + NUM_CELL_X * EDGE) && (posMouse.y >= GLOB_IND_H) && (posMouse.y <= GLOB_IND_H + NUM_CELL_Y * EDGE))
				if (timer > 400){
					timer = 0;	
					tmpX = (int) posMouse.x; tmpX -= GLOB_IND_W; tmp = tmpX % EDGE; tmpX -= tmp; tmpX /= EDGE;
					tmpY = (int) posMouse.y; tmpY -= GLOB_IND_H; tmp = tmpY % EDGE; tmpY -= tmp; tmpY /= EDGE;
					for (int i = 0; i < NumWall; i++){
						if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY && ArrWall [i] -> name != "Start" && ArrWall [i] -> name != "Finish"){
							for (int j = i; j < NumWall - 1; j++)
								ArrWall [j] =  ArrWall [j + 1];
							NumWall--;
							CoordWall [tmpX][tmpY] = false;
							if (ArrWall [i] -> name == "Wall") wallDeleted = true;
							else if (ArrWall [i] -> name == "Circle") circleDeleted = true;
							else if (ArrWall [i] -> name == "Rectangle") rectangleDeleted = true;
							else if (ArrWall [i] -> name == "Triangle") triangleDeleted = true;
							break;
						}
					}
					switch (whichWall){
					case startW:{
						bool tmpB = true;
						for (int i = 0; i < NumWall; i++)
							if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY && ArrWall [i] -> name == "Finish")
								tmpB = false;
						if (tmpB){
							for (int i = 0; i < NumWall; i++){
								if (ArrWall [i] -> name == "Start"){
									CoordWall [ArrWall [i] -> x][ArrWall [i] -> y] = false;
									for (int j = i; j < NumWall - 1; j++)
										ArrWall [j] =  ArrWall [j + 1];
									NumWall--;
									break;
								}
							}
							ArrWall [NumWall++] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE, 20, 20);
							Start.x = tmpX; Start.y = tmpY;
						}
						break;
								}
					case finishW:{
						bool tmpB = true;
						for (int i = 0; i < NumWall; i++)
							if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY && ArrWall [i] -> name == "Start")
								tmpB = false;
						if (tmpB){
							for (int i = 0; i < NumWall; i++){
								if (ArrWall [i] -> name == "Finish"){
									ArrWall [i] -> drawThis = false;
									CoordWall [ArrWall [i] -> x][ArrWall [i] -> y] = false;
									for (int j = i; j < NumWall - 1; j++)
										ArrWall [j] =  ArrWall [j + 1];
									NumWall--;
									break;
								}
							}
							ArrWall [NumWall++] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE, 20, 20);
							Finish.x = tmpX; Finish.y = tmpY;
						}
						break;
								 }
					case wall:{
						bool tmp2 = false;
						if (!wallDeleted){
							for (int i = 0; i < NumWall; i++)
								if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY && (ArrWall [i] -> name == "Start" || ArrWall [i] -> name == "Finish"))
									tmp2 = true;
							if (!tmp2){
									ArrWall [NumWall++] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE, 20, 20);
									CoordWall [tmpX][tmpY] = true;
								}
						}
						wallDeleted = false;
						break;
						}
					case rectangleW:{
						bool tmp2 = false;
						if (!rectangleDeleted){
							for (int i = 0; i < NumWall; i++)
								if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY && (ArrWall [i] -> name == "Start" || ArrWall [i] -> name == "Finish"))
									tmp2 = true;
							if (!tmp2){
								ArrWall [NumWall++] = new Wall (wallImage, "Rectangle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							}
						}
						rectangleDeleted = false;
						break;
									}
					case triangleW:{
						bool tmp2 = false;
						if (!triangleDeleted){
							for (int i = 0; i < NumWall; i++)
								if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY && (ArrWall [i] -> name == "Start" || ArrWall [i] -> name == "Finish"))
									tmp2 = true;
							if (!tmp2){
								ArrWall [NumWall++] = new Wall (wallImage, "Triangle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							}
						}
						triangleDeleted = false;
						break;
								   }
					case circleW:{
						bool tmp2 = false;
						if (!circleDeleted){
							for (int i = 0; i < NumWall; i++)
								if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY && (ArrWall [i] -> name == "Start" || ArrWall [i] -> name == "Finish"))
									tmp2 = true;
							if (!tmp2){
								ArrWall [NumWall++] = new Wall (wallImage, "Circle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							}
						}
						circleDeleted = false;
						break;
								 }
					}
				}	
	}

	void saveFile (char *tmpC){ //сохранение уровня админом
		ofstream outF (tmpC);
		int tmp = 0;
		for (int i = 0; i < NumWall; i++){
			if (ArrWall [i] -> drawThis)
				tmp++;
		}
		outF << tmp << endl;
		for (int i = 0; i < NumWall; i++){
			if (ArrWall [i] -> drawThis){
				outF << ArrWall [i] -> x << " " << ArrWall [i] -> y;
				if (ArrWall [i] -> name == "Wall")            outF << " Wall" << endl;
				else if (ArrWall [i] -> name == "Start")      outF << " Start" << endl;
				else if (ArrWall [i] -> name == "Finish")     outF << " Finish" << endl;
				else if (ArrWall [i] -> name == "Rectangle")  outF << " Rectangle" << endl;
				else if (ArrWall [i] -> name == "Circle")     outF << " Circle" << endl;
				else if (ArrWall [i] -> name == "Triangle")   outF << " Triangle" << endl;
			}
		}
	}

	void openFileAd (char *tmpName){
		bool indexFinishUpdate = false;
		char tmpC [50];
		strcpy (tmpC, tmpName);
		int tmpX, tmpY;
		if (NumWall != 0){
			for (int i = 0; i < NumWall; i++)
				ArrWall [i] -> ~Wall ();
		}
		for (int i = 0; i < NUM_CELL_X; i++)
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		ifstream inF (tmpC);

		inF >> NumWall;
		for (int i = 0; i < NumWall; i++){
			inF >> tmpX >> tmpY >> tmpC;
			if (strcmp (tmpC, "Wall") == 0){
				ArrWall [i] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE, 20, 20);
				CoordWall [tmpX][tmpY] = true;
			}
			else if (strcmp (tmpC, "Rectangle") == 0)
				ArrWall [i] = new Wall (wallImage, "Rectangle", tmpX, tmpY, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Circle") == 0)
				ArrWall [i] = new Wall (wallImage, "Circle", tmpX, tmpY, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Triangle") == 0)
				ArrWall [i] = new Wall (wallImage, "Triangle", tmpX, tmpY, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Start") == 0){
				Start.x = tmpX * EDGE + GLOB_IND_W; Start.y = tmpY * EDGE + GLOB_IND_H;
				ArrWall [i] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE, 20, 20);
			}
			else if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX * EDGE + GLOB_IND_W; Finish.y = tmpY * EDGE + GLOB_IND_H; indexFinish = i; indexFinishUpdate = true;
				ArrWall [i] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE, 20, 20);
			}
		}
		if (!indexFinishUpdate)
			indexFinish = -1;
	}

	void openSpecificFile (char *nameFile){ //открытие уровня когда игрок заходит
		bool indexFinishUpdate = false;
		int tmpX, tmpY;
		char tmpC [40];
		if (NumWall != 0){
			for (int i = 0; i < NumWall; i++)
				ArrWall [i] -> ~Wall ();
		}
		for (int i = 0; i < NUM_CELL_X; i++)
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		NumAnsw = 0;
		ifstream inF (nameFile);
		inF >> NumWall;
		for (int i = 0; i < NumWall; i++){
			inF >> tmpX >> tmpY >> tmpC;
			if (strcmp (tmpC, "Wall") == 0){
				ArrWall [i] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE, 20, 20);
				CoordWall [tmpX][tmpY] = true;
			}
			else if (strcmp (tmpC, "Rectangle") == 0)
				ArrWall [i] = new Wall (wallImage, "Rectangle", tmpX, tmpY, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Circle") == 0)
				ArrWall [i] = new Wall (wallImage, "Circle", tmpX, tmpY, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Triangle") == 0)
				ArrWall [i] = new Wall (wallImage, "Triangle", tmpX, tmpY, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Start") == 0){
				Start.x = tmpX * EDGE + GLOB_IND_W; Start.y = tmpY * EDGE + GLOB_IND_H;
				ArrWall [i] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE, 20, 20);
				ArrWall [i] -> drawThis = false;
			}
			else if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX * EDGE + GLOB_IND_W; Finish.y = tmpY * EDGE + GLOB_IND_H; indexFinish = i; indexFinishUpdate = true;
				ArrWall [i] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE, 20, 20);
			}
		}
		if (!indexFinishUpdate)
			indexFinish = -1;
	}

	void inputKeyboard (char *tmpC, bool fictiv){ //ввод с клавиатуры
		if (event.type == Event::KeyPressed){
			if ((strlen (tmpC) < 9 && !fictiv) || (strlen (tmpC) < 4 && fictiv))
				if (Keyboard::isKeyPressed (Keyboard::A))                     strcat (tmpC, "a");
				else if (Keyboard::isKeyPressed (Keyboard::B))                strcat (tmpC, "b");
				else if (Keyboard::isKeyPressed (Keyboard::C))                strcat (tmpC, "c");
				else if (Keyboard::isKeyPressed (Keyboard::D))                strcat (tmpC, "d");
				else if (Keyboard::isKeyPressed (Keyboard::E))                strcat (tmpC, "e");
				else if (Keyboard::isKeyPressed (Keyboard::F))                strcat (tmpC, "f");
				else if (Keyboard::isKeyPressed (Keyboard::G))                strcat (tmpC, "g");
				else if (Keyboard::isKeyPressed (Keyboard::H))                strcat (tmpC, "h");
				else if (Keyboard::isKeyPressed (Keyboard::I))                strcat (tmpC, "i");
				else if (Keyboard::isKeyPressed (Keyboard::J))                strcat (tmpC, "j");
				else if (Keyboard::isKeyPressed (Keyboard::K))                strcat (tmpC, "k");
				else if (Keyboard::isKeyPressed (Keyboard::L))                strcat (tmpC, "l");
				else if (Keyboard::isKeyPressed (Keyboard::M))                strcat (tmpC, "m");
				else if (Keyboard::isKeyPressed (Keyboard::N))                strcat (tmpC, "n");
				else if (Keyboard::isKeyPressed (Keyboard::O))                strcat (tmpC, "o");
				else if (Keyboard::isKeyPressed (Keyboard::P))                strcat (tmpC, "p");
				else if (Keyboard::isKeyPressed (Keyboard::Q))                strcat (tmpC, "q");
				else if (Keyboard::isKeyPressed (Keyboard::R))                strcat (tmpC, "r");
				else if (Keyboard::isKeyPressed (Keyboard::S))                strcat (tmpC, "s");
				else if (Keyboard::isKeyPressed (Keyboard::T))                strcat (tmpC, "t");
				else if (Keyboard::isKeyPressed (Keyboard::U))                strcat (tmpC, "u");
				else if (Keyboard::isKeyPressed (Keyboard::V))                strcat (tmpC, "v");
				else if (Keyboard::isKeyPressed (Keyboard::W))                strcat (tmpC, "w");
				else if (Keyboard::isKeyPressed (Keyboard::X))                strcat (tmpC, "x");
				else if (Keyboard::isKeyPressed (Keyboard::Y))                strcat (tmpC, "y");
				else if (Keyboard::isKeyPressed (Keyboard::Z))                strcat (tmpC, "z");
				else if (Keyboard::isKeyPressed (Keyboard::Period))           strcat (tmpC, ".");
				else if (Keyboard::isKeyPressed (Keyboard::Num0))             strcat (tmpC, "0");
				else if (Keyboard::isKeyPressed (Keyboard::Num1))             strcat (tmpC, "1");
				else if (Keyboard::isKeyPressed (Keyboard::Num2))             strcat (tmpC, "2");
				else if (Keyboard::isKeyPressed (Keyboard::Num3))             strcat (tmpC, "3");
				else if (Keyboard::isKeyPressed (Keyboard::Num4))             strcat (tmpC, "4");
				else if (Keyboard::isKeyPressed (Keyboard::Num5))             strcat (tmpC, "5");
				else if (Keyboard::isKeyPressed (Keyboard::Num6))             strcat (tmpC, "6");
				else if (Keyboard::isKeyPressed (Keyboard::Num7))             strcat (tmpC, "7");
				else if (Keyboard::isKeyPressed (Keyboard::Num8))             strcat (tmpC, "8");
				else if (Keyboard::isKeyPressed (Keyboard::Num9))             strcat (tmpC, "9");

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
		st.x = (pl -> x - GLOB_IND_W) / EDGE;
		st.y = (pl -> y - GLOB_IND_H) / EDGE;
		fn.x = (Finish.x - GLOB_IND_W) / EDGE;
		fn.y = (Finish.y - GLOB_IND_H) / EDGE;
		pl -> currDir = 0;
		outputSearch (CoordWall, fn, st, sizeMap);
	}


	void StateMenu (){
		button [NumButton - 1] -> drawThis = false; 
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> buttClick && button [i] -> name == "Mode"){
					sndClickButt.play (); 
					state = mode; 
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == mode)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
				if (button [i] -> buttClick && button [i] -> name == "Go!"){
					sndClickButt.play (); 
					if (AdOrPlMode == "AdminMode"){
						NumWall = 0;
						ArrWall [NumWall++] = new Wall (wallImage, "Start", 0, 0, EDGE, EDGE, 20, 20);
						ArrWall [NumWall++] = new Wall (wallImage, "Finish", 1, 0, EDGE, EDGE, 20, 20);
						whichWall = wall;
						state = admin; 
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == admin)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
					if (AdOrPlMode == "PlayerMode"){
						sndClickButt.play (); 
						writeInfo ();
						state = selectLVL;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == selectLVL)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
				}
				if (button [i] -> buttClick && button [i] -> name == "Settings"){
					sndClickButt.play (); 
					state = settings;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == settings)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
				if ((button [i] -> buttClick && button [i] -> name == "Exit") || escapeReleased){
					writeInfo ();
					sndClickButt.play (); 
					state = exitt;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == exitt)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
			}
	}
	void StateMode (){
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if ((button [i] -> buttClick && button [i] -> name == "BackToMenu") || escapeReleased){
					sndClickButt.play (); 
					state = menu;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == menu)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
				if (button [i] -> buttClick && button [i] -> name == "AdminMode")
					sndClickButt.play (); 
			}
			if (AdOrPlMode == "AdminMode" && !PassEnter){
				state = reqPass;
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> state == reqPass)
						button [i] -> drawThis = true;
					else
						button [i] -> drawThis = false;
				
			}
	}
	void StateAdmin (){
		createWalls ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> buttClick && button [i] -> name == "SaveAd"){
					sndClickButt.play (); 
					state = AdSaveLVL;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == AdSaveLVL)
							button [i] -> drawThis = true;
				}
				if (button [i] -> buttClick && button [i] -> name == "OpenAd"){
					sndClickButt.play (); 
					state = AdSelectLVL;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == AdSelectLVL)
							button [i] -> drawThis = true;
				}
				if ((button [i] -> buttClick && button [i] -> name == "BackToMenuAd") || escapeReleased){
					sndClickButt.play (); 
					state = menu;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == menu)
								button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
			}

			if (event.type == Event::KeyPressed){
				if (Keyboard::isKeyPressed (Keyboard::Num1))       whichWall = rectangleW;
				else if (Keyboard::isKeyPressed (Keyboard::Num2))  whichWall = triangleW;
				else if (Keyboard::isKeyPressed (Keyboard::Num3))  whichWall = circleW;
				else if (Keyboard::isKeyPressed (Keyboard::Num4))  whichWall = wall;
				else if (Keyboard::isKeyPressed (Keyboard::Num5))  whichWall = startW;
				else if (Keyboard::isKeyPressed (Keyboard::Num6))  whichWall = finishW;
			}
	}
	void StatePlayer (){
		pl -> update (CoordWall);
		plBackground -> changeCoord (pl -> x, pl -> y);

		for (int i = 0; i < NumWall; i++){
			if ((ArrWall [i] -> x * EDGE + GLOB_IND_W == pl -> x) && (ArrWall [i] -> y * EDGE + GLOB_IND_H == pl -> y)){
				if (ArrWall [i] -> name == "Rectangle" && pl -> statePl != rectangle){
					pl -> changeCoord (Start.x, Start.y);
					plBackground -> changeCoord (pl -> x, pl -> y);
					state = startLVL;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == startLVL)
							button [i] -> drawThis = true;
						else
							if (button [i] -> name == "BackToMenuPl")
								button [i] -> drawThis = false;
					createWay ();
					break;
				}
				else if (ArrWall [i] -> name == "Circle" && pl -> statePl != circle){
					pl -> changeCoord (Start.x, Start.y);
					plBackground -> changeCoord (pl -> x, pl -> y);
					state = startLVL;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == startLVL)
							button [i] -> drawThis = true;
						else
							if (button [i] -> name == "BackToMenuPl")
								button [i] -> drawThis = false;
					createWay ();
					break;
				}
				else if (ArrWall [i] -> name == "Triangle" && pl -> statePl != triangle){
					pl -> changeCoord (Start.x, Start.y);
					plBackground -> changeCoord (pl -> x, pl -> y);
					state = startLVL;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == startLVL)
							button [i] -> drawThis = true;
						else
							if (button [i] -> name == "BackToMenuPl")
								button [i] -> drawThis = false;
					createWay ();
					break;
				}
			}
		}

		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				if (button [i] -> name == "TimePlayer"){
					char tmpC [30];
					char *tmpC2;
					tmpC2 = _itoa ((int) timePl / 1250, tmpC, 10);
					button [i] -> updateText (tmpC2);
					timePl += time;
				}
				button [i] -> checkCursor ();
				if ((button [i] -> buttClick && button [i] -> name == "BackToMenuPl") || escapeReleased){
					timePl -= time;
					escapeReleased = false;
					sndClickButt.play (); 
					state = pause;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == pause)
							button [i] -> drawThis = true;
						else
							if (button [i] -> name == "BackToMenuPl")
								button [i] -> drawThis = false;
				}
				if ((button [i] -> buttClick && button [i] -> name == "lvlComplete") || (lvlComplete && Keyboard::isKeyPressed (Keyboard::Return))){
					AllTime += timePl / 1250;
					timePl = 0;
					sndClickButt.play (); 
					if (CurrentLVL < 4){
						timer = 0;
						if (PassedLVL < 4)
							PassedLVL++;
						writeInfo ();
						CurrentLVL++;
						char tmpC [30], *tmpC2;
						tmpC2 = _itoa (CurrentLVL, tmpC, 10);
						char nameFile [30] = "Resources/LVLs/lvl";
						strcat (nameFile, tmpC2);
						strcat (nameFile, ".txt");
						openSpecificFile (nameFile);
						pl -> changeCoord (Start.x, Start.y);
						plBackground -> changeCoord (pl -> x, pl -> y);
						createWay ();
					}
				}
			}

			if (lvlComplete){
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> name == "lvlComplete")
						button [i] -> drawThis = true;
			}
			else{
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> name == "lvlComplete")
						button [i] -> drawThis = false;
			}
	}
	void StateSettings (){
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if ((button [i] -> buttClick && button [i] -> name == "BackToMenuSet") || escapeReleased){
					sndClickButt.play (); 
					writeInfo ();
					state = menu;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == menu)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
				if (button [i] -> buttClick && button [i] -> name == "MusicSlider")
					sndClickButt.play (); 
				if (button [i] -> buttClick && button [i] -> name == "SoundSlider")
					sndClickButt.play (); 
			}
	}
	void StateExitt (){
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if ((button [i] -> buttClick && button [i] -> name == "QuitNo") || escapeReleased){
					sndClickButt.play (); 
					state = menu;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == menu)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
				if ((button [i] -> buttClick && button [i] -> name == "QuitYes") || Keyboard::isKeyPressed (Keyboard::Return)){
					sndClickButt.play (); 
					window -> close ();
				}
			}
	}
	void StateReqPass (){
		inputKeyboard (Pass, 1);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> name == "Edit")
					button [i] -> updateText (Pass);
				if ((button [i] -> buttClick && button [i] -> name == "Edit") || (event.type == Event::KeyPressed && Keyboard::isKeyPressed (Keyboard::Return))){
					if (!PassEnter){
						sndClickButt.play (); 
						if (strcmp (Pass, "4329") == 0){
							writeInfo ();
							state = mode; PassEnter = true;
							AdOrPlMode = "AdminMode"; strcpy (Pass, "");
							for (int i = 0; i < NumButton; i++)
								if (button [i] -> state == mode)
									button [i] -> drawThis = true;
								else
									button [i] -> drawThis = false;
						}
						else{
							state = mode; PassEnter = false;
							AdOrPlMode = "PlayerMode"; strcpy (Pass, "");
							for (int i = 0; i < NumButton; i++)
								if (button [i] -> state == mode)
									button [i] -> drawThis = true;
								else
									button [i] -> drawThis = false;
						}
					}
				}
			}
	}
	void StateSelectLVL (){
		char tmpC2 [30];
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> buttClick && button [i] -> name == "SelectLVL"){
					sndClickButt.play (); 
					if (button [i] -> value <= PassedLVL + 1){
						CurrentLVL = button [i] -> value;
						_itoa (button [i] -> value, tmpC2, 10);
						state = player;
						char nameFile [30] = "Resources/LVLs/lvl";
						strcat (nameFile, tmpC2);
						strcat (nameFile, ".txt");
						openSpecificFile (nameFile);
						pl -> changeCoord (Start.x, Start.y);
						plBackground -> changeCoord (Start.x, Start.y);

						createWay ();

						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == player)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
				}
				if ((button [i] -> buttClick && button [i] -> name == "BackToMenuSel") || escapeReleased){
					sndClickButt.play (); 
					state = menu;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == menu)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
			}
	}
	void StateAdSelectLVL (){
		inputKeyboard (fileNameAd, 0);
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> state == AdSelectLVL){
					button [i] -> checkCursor ();
					if (button [i] -> name == "EditLVL")
						button [i] -> updateText (fileNameAd);
					if ((button [i] -> buttClick && button [i] -> name == "EditLVL") || (event.type == Event::KeyPressed && Keyboard::isKeyPressed (Keyboard::Return))){
						sndClickButt.play (); 
						state = admin;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == admin)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
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
								openFileAd (tmpC);
								break;
							}
						}
						
					}
				}
	}
	void StateAdSaveLVL (){
		inputKeyboard (fileNameAd, 0);
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> state == AdSaveLVL){
					button [i] -> checkCursor ();
					if (button [i] -> name == "AdSaveLVL")
						button [i] -> updateText (fileNameAd);
					if ((button [i] -> buttClick && button [i] -> name == "AdSaveLVL") || (event.type == Event::KeyPressed && Keyboard::isKeyPressed (Keyboard::Return))){
						sndClickButt.play (); 
						state = admin;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == admin)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
						int tmpI; 
						char tmpC2 [100][30]; 
						bool edit = true;
						ifstream inF ("Resources/LVLs/listLVLs.txt");
						inF >> tmpI;
						for (int i = 0; i < tmpI; i++){
							inF >> tmpC2 [i];
							if (strcmp (tmpC2 [i], fileNameAd) == 0){
								edit = false;
								break;
							}
						}
						inF.close ();
						if (edit){
							ofstream outF ("Resources/LVLs/listLVLs.txt");
							outF << ++tmpI << endl;
							for (int i = 0; i < tmpI - 1; i++){
								outF << tmpC2 [i] << endl;
							}
							outF << fileNameAd << endl;
							char tmpC [100] = "Resources/LVLs/";
							strcat (tmpC, fileNameAd);
							strcat (tmpC, ".txt");
							saveFile (tmpC);
						}
						if (!edit){
							ofstream outF ("Resources/LVLs/listLVLs.txt");
							outF << tmpI << endl;
							for (int i = 0; i < tmpI; i++){
								outF << tmpC2 [i] << endl;
							}
							char tmpC [100] = "Resources/LVLs/";
							strcat (tmpC, fileNameAd);
							strcat (tmpC, ".txt");
							saveFile (tmpC);
						}
					}
				}
	}
	void StatePause (){
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> state == pause){
				button [i] -> checkCursor ();
				if ((button [i] -> buttClick && button [i] -> name == "LeaveYes") || Keyboard::isKeyPressed (Keyboard::Return)){
					AllTime += timePl / 1250;
					timePl = 0;
					sndClickButt.play (); 
					timer = 0;
					NumAnsw = 0;
					writeInfo ();
					state = selectLVL;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == selectLVL)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
					lvlComplete = false;
				}
				if ((button [i] -> buttClick && button [i] -> name == "LeaveNo") || escapeReleased){
					sndClickButt.play (); 
					state = player;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == player)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
			}
	}
	void StateStartLVL (){
		pl -> changeFigure ();
		if (Keyboard::isKeyPressed (Keyboard::Return)){
			sndClickButt.play (); 
			state = player;
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> state == player)
					button [i] -> drawThis = true;
				else
					button [i] -> drawThis = false;
		}
		if (escapeReleased){
			AllTime += timePl / 1250;
			timePl = 0;
			sndClickButt.play (); 
			timer = 0;
			NumAnsw = 0;
			writeInfo ();
			state = selectLVL;
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> state == selectLVL)
					button [i] -> drawThis = true;
				else
					button [i] -> drawThis = false;
			lvlComplete = false;
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
		}
	}
};

int main (){
	System system;
	view.reset (FloatRect (0, 0, (float) system.W_WIN, (float) system.H_WIN)); //создание камеры
	setCoordinateForView ((float) system.GLOBAL_W / 2, (float) system.GLOBAL_H / 2); //двигаем камеру

	
	Game game;
	system.window = new RenderWindow (VideoMode (system.W_WIN, system.H_WIN), "LABYRINTH PRO", Style::Fullscreen, ContextSettings (0, 0, 1)); //создание окна
	bool isUpdate = false;

	while (system.window -> isOpen ()){
		system.time = (float) system.clock.getElapsedTime ().asMicroseconds(); //время каждый раз обновляется
		system.clock.restart ();
		system.time = system.time / 800;
		system.timer += system.time;

		system.mousePosWin = Mouse::getPosition (system.window [0]); //координаты мыши относ. окна
		system.posMouse = system.window -> mapPixelToCoords (system.mousePosWin); //координаты мыши относ. карты


		while (system.window -> pollEvent (system.event)){ //заходит в цикл когда что-то происходит (игрок двигает мышкой или нажжимает на клавиши)
			if (system.event.type == Event::Closed) //закрыть игру можно и ескейпом
				system.window -> close (); 
			isUpdate = true;
			game.update ();
			if (system.event.type == Event::KeyReleased && system.event.key.code == 36) //если нажади ескейп
				game.escapeReleased = true;
			else
				game.escapeReleased = false;
		}		
		if (!isUpdate){ //обновляем игру если не зашли в предыдущий while
			game.update (); game.escapeReleased = false;
		}
		if (game.state == player) //обновляем игрока всегда
			game.StatePlayer ();
		isUpdate = false;
		game.draw ();
	}
	return 0;
}