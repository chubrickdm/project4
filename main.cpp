#include <SFML/Graphics.hpp> //основная библиотека для работы с графикой и изображением
#include <SFML/Audio.hpp> //для работы со звуком и музыкой
#include <iostream> //для сяутов и синов
#include <fstream> //для работы с файлами (записывается туда уровни инфо об игроке и т.д.)
#include "forMcText.h" //взял библиотеку по ссылке kychka-pc.ru/wiki/svobodnaya-baza-znanij-sfml/test/sistemnyj-modul-system-module/potoki-threads/sfsound-sfmusic
#include "searchWay.h" //написанная мной функция для нахождения минимального пути в лабиринте (поиск в ширину) или волновой поиск
#include "view.h" //управление камерой
#include "system.h"
#include "body.h"
#include "general classes.h"
#include "quickSort.h"
#include "binSearch.h"
using namespace std;
using namespace sf;


//инициализируем всё, нужно что б если переменная менялась где-то, то она менялась и во всех классах наследниках
bool          System::F_changeStates; //флаг, который показывает, меняется ли состояние в данный момент
Vector2i      System::mousePosWin; //системные переменные
Vector2f      System::posMouse;
RenderWindow* System::window;
Event         System::event;

float         System::timer; //переменные связанные со временем
float         System::time;
int           System::FPS;
Clock         System::clock;

bool          System::F_musicOff; //переменные связанные с музыкой и звуками
bool          System::F_soundOff; 
float         System::volumeSound;
float         System::volumeMusic;
Music         System::backgroundMusic;
SoundBuffer   System::bufferClickButt;
Sound         System::sndClickButt;
SoundBuffer   System::bufferTeleport;
Sound         System::sndTeleport;

bool          System::F_lvlComplete; //внтуригровая логика
bool          System::F_showMessage;
float         System::speedChangeSt;
float         System::speed;
int           System::keyCodePressed;
int           System::key [3];
int           System::whatButChange;
int           System::PassedLVL;
CreateWall    System::whichWall;
String        System::AdOrPlMode;
Coordinate    System::Start;
Coordinate    System::Finish;


class Button : public BodyButton{
private:
	bool F_locked; //флаг, который показывает заблокирована ли кнопка
public:
	Button (Image &image, String Text, String Name, Font &Font, SubtypeState &Subtype, int X, int Y, int W, int H, int Value, int WTexture, int HTexture) : 
		    BodyButton (image, Text, Name, Font, Subtype, X, Y, W, H, WTexture, HTexture){
		value = Value; F_locked = false;

		if (subtype == menu)
			F_draw = true;
		else
			F_draw = false;
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
			if (value > PassedLVL + 1)  F_locked = true;
			else                        F_locked = false;
		if (name == "My lvls")
			if (PassedLVL != 8)         F_locked = true;
			else                        F_locked = false;

		F_click = false;
		if (!F_locked){
			if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //если курсор мыши находится на кнопке
				if (Mouse::isButtonPressed (Mouse::Left)) //и если нажали на нее
					F_pressed = true;
				else{
					if (F_pressed) //если же курсор на кнопке и кнопка была нажата, а сейчас не нажата-значит мы кликнули по ней
						F_click = true; 
					F_pressed = false;
				}
				shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture)); //если наведен курсор на мышку, то кнопка меняет текстуру
				if (name == "SelectLVL")
					shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture));
			}
			else{ //если курсор не на кнопке
				F_pressed = false; //если курсор не на мыши то кнопка обычного вида
				shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
				if (name == "SelectLVL")
					shape.setTextureRect (IntRect (0, 4 * hTexture, wTexture, hTexture));
			}
		}
		else{ //если кнопка заблокирована
			shape.setTextureRect (IntRect (0, hTexture * 3, wTexture, hTexture));
			if (name == "SelectLVL")
				shape.setTextureRect (IntRect (3 * wTexture, 4 * hTexture, wTexture, hTexture));
		}

		if (name == AdOrPlMode) //если имя переменной которая хранит имя режима совпала с кнопкой, то кнопка выбрана (подсвечивается золотистым)
			shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
	}

	void updateText (char *Pass){ }
};

class EditButton : public BodyButton{
public:
	EditButton (Image &image, String Text, String Name, Font &Font, SubtypeState &Subtype, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    BodyButton (image, Text, Name, Font, Subtype, X, Y, W, H, WTexture, HTexture){
		if (name == "ChangeKey")
			shape.setTextureRect (IntRect (0, 120, wTexture, hTexture));
		if (Text == "1")      value = 1;
		else if (Text == "2") value = 2;
		else if (Text == "3") value = 3;
	}

	void draw (){
		window -> draw (shape);
		text -> draw (window);
	}

	void checkCursor (){ //функция проверки на нажатие кнопки или навдением курсора на кнопку
		F_click = false;
		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //если курсор мыши находится на кнопке
			if (Mouse::isButtonPressed (Mouse::Left)) //и если нажали на нее
				F_pressed = true;
			else{
				if (F_pressed) //если же курсор на кнопке и кнопка была нажата, а сейчас не нажата-значит мы кликнули по ней
					F_click = true; 
				F_pressed = false;
			}
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture)); //если наведен курсор на мышку, то кнопка меняет текстуру
			if (name == "ChangeKey")
				shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture));
		}
		else{
			F_pressed = false; //если курсор не на мыши то кнопка обычного вида
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
			if (name == "ChangeKey")
				shape.setTextureRect (IntRect (0, 4 * hTexture, wTexture, hTexture));
		}
		
		if (name == "ChangeKey" && whatButChange == value)
			shape.setTextureRect (IntRect (2 * wTexture, 4 * hTexture, wTexture, hTexture));
	}

	void updateText (char *Pass){ //функция обновления текста
		buttText = Pass;
		text -> clear ();
		text -> changeSize (SIZE_TEXT); //размер текста
		text -> add (buttText);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
	}
};

class Static : public BodyButton{
public:
	Static (String Text, String Name, Font &Font, SubtypeState &Subtype, int X, int Y, Color ccolor) : //перегрузка для задания цвета текста
		    BodyButton (Text, Name, Font, Subtype, X, Y){ 
		text -> clear ();
		text -> changeSize (SIZE_TEXT); //размер текста
		color = ccolor;
		text -> add (buttText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке

		if (subtype == wholeType)
			type = player;
	}

	Static (String Text, String Name, Font &Font, SubtypeState &Subtype, int X, int Y) : //по умолчанию цвет текста будет черный
		    BodyButton (Text, Name, Font, Subtype, X, Y){ 
		color = Color (0, 0, 0);
		if (subtype == wholeType)
			type = player;
	}

	void draw (){
		text -> draw (window);
	}

	void EFF_reduce (){ //уменьшение кнопки
		if (F_transformation){
			text -> clear ();
			text -> changeSize (SIZE_TEXT * (int) reducePrecent / 100); //размер текста
			text -> add (buttText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * reducePrecent / 100 / 3); //распологаем текст по кнопке

			reducePrecent -= speedChangeSt * time; //когда прекратится изменение формы
			if (reducePrecent < speedChangeSt * time){
				reducePrecent = 100;
				F_transformation = false;
			}
		}
	}

	void EFF_enlarge (){ //увелечение кнопки
		if (F_transformation){
			text -> clear ();
			text -> changeSize (SIZE_TEXT * (int) enlargePrecent / 100); //размер текста
			text -> add (buttText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * enlargePrecent / 100 / 3); //распологаем текст по кнопке

			enlargePrecent += speedChangeSt * time; //когда прекратится изменение формы
			if (enlargePrecent > 100 - speedChangeSt * time){
				enlargePrecent = 1;
				F_transformation = false;

				text -> clear ();
				text -> changeSize (SIZE_TEXT); //размер текста
				text -> add (buttText, color);
				text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
			}
		}
	}

	void checkCursor (){ }

	void updateText (char *Pass){
		text -> clear ();
		text -> changeSize (SIZE_TEXT); //размер текста
		char tmpC [40];

		if (name == "TimePlayer")
			strcpy (tmpC, "Time: ");
		else if (name == "FPS")
			strcpy (tmpC, "FPS: ");
		else if (name == "DeathPlayer")
			strcpy (tmpC, "Death: ");
		else if (name == "ControlRec")
			strcpy (tmpC, "Rectangle: ");
		else if (name == "ControlTri")
			strcpy (tmpC, "Triangle: ");
		else if (name == "ControlCir")
			strcpy (tmpC, "Circle: ");

		strcat (tmpC, Pass);
		text -> add (tmpC, color);
		buttText = tmpC;
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
	}
};

class HorizontScrollBar : public BodyButton{
private:
	int leftBorder, rightBorder; //левая и правая граница по которой может перемещаться кнопка (не вся доступная область, т.к. кнопка имеет ширину)
	RectangleShape backgroundd;
	int wBground;
	int hBground;
	int x2, y2;
public:
	HorizontScrollBar (Image &image, String Name, SubtypeState &Subtype, int X, int Y, int W, int H, int tmpBordL, int tmpBordR, int WTexture, int HTexture, int WBground, int HBground) : 
		    BodyButton (image, Name, Subtype, X, Y, W, H, WTexture, HTexture){ 
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
			shape.setPosition ((float) leftBorder + volumeMusic * (rightBorder - leftBorder) / 100, (float) y);
			x = leftBorder + (int) volumeMusic * (rightBorder - leftBorder) / 100;
		}
		if (name == "SoundSlider"){
			shape.setPosition ((float) leftBorder + volumeSound * (rightBorder - leftBorder) / 100, (float) y);
			x = leftBorder + (int) volumeSound * (rightBorder - leftBorder) / 100;
		}
	}

	void draw (){
		window -> draw (backgroundd);
		window -> draw (shape);
	}

	void checkCursor (){ 
		if (posMouse.x >= leftBorder && posMouse.x <= rightBorder) //если мышка находится в доступном для кнопки месте
			if (posMouse.y >= y - h / 2 && posMouse.y <= y + h / 2){
				if (event.type == Event::MouseButtonPressed && !F_pressed){
					shape.setPosition ((float) posMouse.x, (float) y);
					x = (int) posMouse.x; F_pressed = true;
					if (name == "MusicSlider"){ //меняем значение грмоксоти музыки
						volumeMusic = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						if (!F_musicOff) changeMusicVolume (volumeMusic); //значение громкости устанавливаем в процентом соотношении, считая от левой границы
						else             changeMusicVolume (0);
					}
					if (name == "SoundSlider"){ //меняем значение громкости звуков
						volumeSound = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						if (!F_soundOff) changeSoundVolume (volumeSound);
						else             changeSoundVolume (0);
					}
				}
				if (F_pressed){ //если кнопка зажата
					shape.setPosition ((float) posMouse.x, (float) y);
					x = (int) posMouse.x; 
					if (name == "MusicSlider"){ //меняем значение грмоксоти музыки
						volumeMusic = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						if (!F_musicOff) changeMusicVolume (volumeMusic); //значение громкости устанавливаем в процентом соотношении, считая от левой границы
						else             changeMusicVolume (0);
					}
					if (name == "SoundSlider"){ //меняем значение громкости звуков
						volumeSound = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						if (!F_soundOff) changeSoundVolume (volumeSound);
						else             changeSoundVolume (0);
					}
				}

				if (event.type == Event::MouseButtonPressed) //если мышка зажата, то устанавливаем соотв. флаг
					F_click = true;
				else
					F_click = false;
		}

		if (event.type == Event::MouseButtonReleased) //если отпустили мышку
			F_pressed = false;

		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)) //если курсор мыши находится на кнопке
			shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture)); //если наведен курсор на мышку, то кнопка меняет текстуру
		else
			shape.setTextureRect (IntRect (0, 4 * hTexture, wTexture, hTexture));
	}

	void updateText (char *Pass){ }

	void EFF_reduce (){
		if (F_transformation){
			shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
			shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);

			backgroundd.setSize (Vector2f ((float) W_BUTTON * reducePrecent / 100, (float) H_BUTTON * reducePrecent / 100));
			backgroundd.setOrigin ((float) W_BUTTON * reducePrecent / 100 / 2, (float) H_BUTTON * reducePrecent / 100 / 2);
			backgroundd.setPosition ((float) x2, (float) y2);

			reducePrecent -= speedChangeSt * time;
			if (reducePrecent < speedChangeSt * time){
				reducePrecent = 100;
				F_transformation = false;
			}
		}
	}

	void EFF_enlarge (){
		if (F_transformation){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);

			backgroundd.setSize (Vector2f ((float) W_BUTTON * enlargePrecent / 100, (float) H_BUTTON * enlargePrecent / 100));
			backgroundd.setOrigin ((float) W_BUTTON * enlargePrecent / 100 / 2, (float) H_BUTTON * enlargePrecent / 100 / 2);
			backgroundd.setPosition ((float) x2, (float) y2);

			enlargePrecent += speedChangeSt * time;
			if (enlargePrecent > 100 - speedChangeSt * time){
				enlargePrecent = 1;
				F_transformation = false;
			}
		}
	}

	void clear (){
		shape.setSize (Vector2f (1, 1));
		backgroundd.setSize (Vector2f (1, 1));
	}
};

class PictureButton : public BodyButton{
private:
	Texture texturePicture;
	RectangleShape picture;
	int wPicture;
	int hPicture;
	CreateWall typeWall;
public:
	PictureButton (Image &image, String Name, SubtypeState &Subtype, int X, int Y, int W, int H, int WTexture, int HTexture, Image &Ipicture, int WPicture, int HPicture) : 
		    BodyButton (image, Name, Subtype, X, Y, W, H, WTexture, HTexture){
		
		shape.setTextureRect (IntRect (0, 120, wTexture, hTexture));
        texturePicture.loadFromImage (Ipicture);
		wPicture = WPicture; hPicture = HPicture;
		picture.setSize (Vector2f ((float) w, (float) h));
		picture.setPosition ((float) x, (float) y);
		picture.setTexture (&texturePicture);
			
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
		F_click = false;
		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //если курсор мыши находится на кнопке
			if (Mouse::isButtonPressed (Mouse::Left)) //и если нажали на нее
				F_pressed = true;
			else{
				if (F_pressed){ //если же курсор на кнопке и кнопка была нажата, а сейчас не нажата-значит мы кликнули по ней
					F_click = true; whichWall = typeWall;
				}
				F_pressed = false;
			}
			shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture));
		}
		else{
			F_pressed = false; //если курсор не на мыши то кнопка обычного вида
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

	void EFF_reduce (){
		if (F_transformation){
			shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
			shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);

			picture.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
			picture.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
			picture.setPosition ((float) x, (float) y);

			reducePrecent -= speedChangeSt * time;
			if (reducePrecent < speedChangeSt * time){
				reducePrecent = 100;
				F_transformation = false;
			}
		}
	}

	void EFF_enlarge (){
		if (F_transformation){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);

			picture.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			picture.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			picture.setPosition ((float) x, (float) y);

			enlargePrecent += speedChangeSt * time;
			if (enlargePrecent > 100 - speedChangeSt * time){
				enlargePrecent = 1;
				F_transformation = false;
			}
		}
	}

	void clear (){
		shape.setSize (Vector2f (1, 1));
		picture.setSize (Vector2f (1, 1));
	}
};

class CheckButton : public BodyButton{
private:
	bool F_switchOff;
public:
	CheckButton (Image &image, String Name, SubtypeState &Subtype, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    BodyButton (image, Name, Subtype, X, Y, W, H, WTexture, HTexture){
		F_switchOff = false;
		shape.setTextureRect (IntRect (wTexture * 2, 6 * hTexture, wTexture, hTexture));
		if (F_musicOff && name == "SwitchMusic")      F_switchOff = true;
		else if (F_soundOff && name == "SwitchSound") F_switchOff = true;
	}

	void draw (){
		window -> draw (shape);
	}

	void checkCursor (){ //функция проверки на нажатие кнопки или наведением курсора на кнопку
		F_click = false;
		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //если курсор мыши находится на кнопке
			if (Mouse::isButtonPressed (Mouse::Left)) //и если нажали на нее
				F_pressed = true;
			else{
				if (F_pressed){ //если же курсор на кнопке и кнопка была нажата, а сейчас не нажата-значит мы кликнули по ней
					F_click = true; changeBool (F_switchOff); 
					if (F_switchOff){
						if (name == "SwitchMusic")      F_musicOff = true;
						else if (name == "SwitchSound") F_soundOff = true;
					}
					else{
						if (name == "SwitchMusic")      F_musicOff = false;
						else if (name == "SwitchSound") F_soundOff = false;
					}
				}
				F_pressed = false;
			}
			if (!F_switchOff) shape.setTextureRect (IntRect (wTexture * 1, 6 * hTexture, wTexture, hTexture));
			else          shape.setTextureRect (IntRect (wTexture * 3, 6 * hTexture, wTexture, hTexture));
		}
		else{ //если курсор не на кнопке
			F_pressed = false; //если курсор не на мыши то кнопка обычного вида
			if (!F_switchOff) shape.setTextureRect (IntRect (wTexture * 0, 6 * hTexture, wTexture, hTexture));
			else          shape.setTextureRect (IntRect (wTexture * 2, 6 * hTexture, wTexture, hTexture));
		}
	}

	void EFF_reduce (){
		if (F_transformation){
			shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
			shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);

			reducePrecent -= speedChangeSt * time;
			if (reducePrecent < speedChangeSt * time){
				reducePrecent = 100;
				F_transformation = false;
			}
		}
	}

	void EFF_enlarge (){
		if (F_transformation){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);

			enlargePrecent += speedChangeSt * time;
			if (enlargePrecent > 100 - speedChangeSt * time){
				enlargePrecent = 1;
				F_transformation = false;
			}
		}
	}

	virtual void clear (){ //функция очищения кнопки, нужна что б начать кнопки увеличивать (вторая фаза)
		shape.setSize (Vector2f (1, 1));
	}

	void updateText (char *Pass){ }
};


class Message : public System{
private:
	float messageTimer;
	float reducePrecent; //процент уменьшения
	float enlargePrecent; //процент увелечения
	int y;
	int x;
	int phase;
	Font font; //шрифт
	Color color; //цвет текста кнопки
	mcText *text; //текст который выводится на кнопке
	String messageText; //текст который будет отображаться на кнопке
private:
	void EFF_reduce (){ //уменьшение кноif (F_transformation){
		text -> clear ();
		text -> changeSize (SIZE_TEXT * (int) reducePrecent / 100); //размер текста
		text -> add (messageText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * reducePrecent / 100 / 3); //распологаем текст по кнопке

		reducePrecent -= speedChangeSt * time; //когда прекратится изменение формы
		if (reducePrecent < speedChangeSt * time){
			reducePrecent = 100; phase = 0;
			F_showMessage = false;
		}
		
	}

	void EFF_enlarge (){ //увелечение кнопки
		text -> clear ();
		text -> changeSize (SIZE_TEXT * (int) enlargePrecent / 100); //размер текста
		text -> add (messageText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * enlargePrecent / 100 / 3); //распологаем текст по кнопке

		enlargePrecent += speedChangeSt * time; //когда прекратится изменение формы
		if (enlargePrecent > 100 - speedChangeSt * time){
			enlargePrecent = 1;
			phase = 1;

			text -> clear ();
			text -> changeSize (SIZE_TEXT); //размер текста
			text -> add (messageText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
		}
	}
public:
	void initialize (Font &tmpFont){
		font = tmpFont;
		text = new mcText (&font); //создаем текст который будет отображаться на кнопке
		text -> changeSize (SIZE_TEXT); //размер текста
		text -> add ("", color);
		reducePrecent = 100; enlargePrecent = 1; 
		phase = 0;
	}

	void setColor (Color &tmpColor){
		color = tmpColor;
	}

	void showMessage (int tmpX, int tmpY, String tmpText, Color tmpColor, float tmpTimer){
		if (!F_showMessage){
			x = tmpX; y = tmpY;
			F_showMessage = true;
			messageText = tmpText;
			messageTimer = tmpTimer;
			color = tmpColor;
			text -> clear ();
			text = new mcText (&font); //создаем текст который будет отображаться на кнопке
			text -> changeSize (SIZE_TEXT); //размер текста
			text -> add (messageText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - text -> h / 2); //распологаем текст по кнопке
		}
	}

	void showMessage (int tmpX, int tmpY, String tmpText, float tmpTimer){
		if (!F_showMessage){
			x = tmpX; y = tmpY;
			F_showMessage = true;
			messageText = tmpText;
			messageTimer = tmpTimer;
			text -> clear ();
			text = new mcText (&font); //создаем текст который будет отображаться на кнопке
			text -> changeSize (SIZE_TEXT); //размер текста
			text -> add (messageText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - text -> h / 2); //распологаем текст по кнопке
		}
	}

	void draw (){
		if (F_changeStates)
			phase = 2;

		if (phase == 0)
			EFF_enlarge ();
		else if (phase == 1){
			messageTimer -= time;
			if (messageTimer <= 0)
			phase = 2;
		}
		else if (phase == 2)
			EFF_reduce ();
		text -> draw (window);
		
	}
};


class Game : public System{ //вся механика и инициализация игры в этом классе
public:
	TypeState     type; //текущий тип
	SubtypeState  subtype; //текущий подтип
	SubtypeState whichStateWas; //какое состояние было, нужно для изменения состояний
	SubtypeState whichStateWill; //какое состояние должно стать, нужно для изменения состояний

	float lvlTime; //время проведенное игроком на уровне
	float AllTime; //общее время игрока проведенного в игре
	int CurrentLVL; //текущий уровень
	int NumLVLDeath; //количествэо смертей на уровне
	int NumWall; //количество стен
	int NumBorderWall; //количество стаен границы
	int NumButton; //количество кнопок
	int NumListLVL; //количество уровней в списке (что б потом знать сколько удалять последних кнопок)

	char Pass [30]; //пароль
	char playerLVLOpenByPlayer [50];  //имя файла открытого игроком, и уровень при этом созданный игроком
	char lvlOpenByAdmin [50]; //имя файла открытого админом
	char lvlOpenByPlayer [70]; //имя файла открытого игроком

	bool F_playerOnStart; //флаг, который показывает игрок находится ли на старте
	bool F_loadLVL; //флаг, который показывает загружен ли уровень
	bool F_reduceWholeType; //флаг, который показывает уменьшаются ли кнопки которые относятся к типо, а не подтипу (время, кол-во смертей, управление)
	bool F_enlargeWholeType; //флаг, который показывает увеличиваются ли кнопки которые относятся к типо, а не подтипу (время, кол-во смертей, управление)
	bool F_escapeReleased; //флаг, равен 1 если ескейп отпустили (ну его нажали, а потом отпустили)
	bool F_enterReleased; //флаг, равен 1 если Enter отпустили (ну его нажали, а потом отпустили)
	bool F_anyKeyReleased; //флаг, равен 1 если Enter отпустили (ну его нажали, а потом отпустили)
	bool F_isPlayerLVL; //флаг, который показывает игрок играет в свой созданный уровень или нет
	bool F_changeKey; //флаг, который показывает, вводится ли сейчас какая клавиша (когда меняем клавишу на которую меняется фигура)
	bool F_secPhaseChangeState; //флаг, который показывает, вторая ли сейчас фаза изменение состояний (увелечение)
	bool F_inSetingIntoPause; //флаг, который показывает, вошли ли мы в настройки через игрока (нужно что б когда выходили из настроек возвращались к игре)
	
	int indexFinish; //индекс финиша (что б долго не искать)
	int indexStart; //индекс старта (что б долго не искать)
	int indexDeathPlayerBut; //индекс кнопки на которой выводится количество смертей на уровне
	int indexFPSBut; //индекс кнопки на которой выводится значение фпс
	int indexTimePlBut; //индекс кнопки на которой выводится время игрока
	int indexControlBut [4]; //индексы кнопок, на котором изображено управление

	Image wallImage; //изображение стен
	Image wallImagePL; //изображение игрока
	VertexArray lines; //линии которые в админ моде рисуются, что б легче было создавтаь уровни
	RectangleShape cursor; //курсор
	Texture textureCursor; //текстура курсора 
	
	Player *pl; //игрок
	BodyButton *button [100]; //массив кнопок
	Wall *ArrWall [4000]; //массив стен
	bool **wallCoordinate; //координаты стен
	Wall *BorderWall [250]; //массив стен которые будут границей для игрока (нужно для красоты)
	Background *plBackground; //фоновое изображение, черное, которые закрывает лабаринт
	Background *logo; //логотип

	Message message;
public:
	void readInfo (){ //считать информацию об игроке
		ifstream inF ("Resources/Info/Player.txt");
		inF >> PassedLVL >> volumeMusic >> volumeSound >> AllTime >> key [0] >> key [1] >> key [2] >> F_musicOff >> F_soundOff; //последние 3-номера клавиш на которые меняется фигура игрока
	}

	void writeInfo (){ //записать информациюю об игроке
		ofstream outF ("Resources/Info/Player.txt");
		outF << PassedLVL << " " << volumeMusic << " " << volumeSound  << " " << AllTime;
		outF << " " << key [0] << " " << key [1] << " " << key [2] << " " << F_musicOff << " " << F_soundOff << endl; //последние 3-номера клавиш на которые меняется фигура игрока
	}

	void draw (){ //главная и единственная функция рисования
		window -> setView (view); //обновляем камеру
		window -> clear (Color (117, 152, 175)); 

		if (type == admin && subtype != listLVL){
			window -> draw (lines); //рисую вспомогательные линии для админа
			for (int i = 0; i < NumWall; i++) //рисую стены
					ArrWall [i] -> draw ();
		}

		if (type == player){
			int tmpX, tmpY;
			float tmpX2, tmpY2;
			tmpX = (int) pl -> x; tmpX -= GLOB_IND_W;
			tmpY = (int) pl -> y; tmpY -= GLOB_IND_H;
			for (int i = 0; i < NumWall; i++){ //рисую стены которые будут видны игроку, и сдвигаю их в центр		
				if (abs (ArrWall [i] -> x * EDGE - tmpX) < EDGE * (NUM_SQUARE + 1) / 2)
					if ( abs (ArrWall [i] -> y * EDGE - tmpY) < EDGE * (NUM_SQUARE + 1) / 2){
					tmpX2 = (float) ArrWall [i] -> x * EDGE - tmpX;
					tmpY2 = (float) ArrWall [i] -> y * EDGE - tmpY;
					tmpX2 = tmpX2 * ((float) SQUARE / (float) EDGE);
					tmpY2 = tmpY2 * ((float) SQUARE / (float) EDGE);
					ArrWall [i] -> changeLocation (GLOBAL_W / 2 + (int) tmpX2 - SQUARE / 2, GLOBAL_H / 2 + (int) tmpY2 - SQUARE / 2);
					ArrWall [i] -> draw ();
				}
			}
			for (int i = 0; i < NumBorderWall; i++) //рисую граничные стены которые будут видны игроку, и сдвигаю их в центр
				if (abs (BorderWall [i] -> x * EDGE - tmpX) < EDGE * (NUM_SQUARE + 1) / 2)
					if (abs (BorderWall [i] -> y * EDGE - tmpY) < EDGE * (NUM_SQUARE + 1) / 2){
					tmpX2 = (float) BorderWall [i] -> x * EDGE - tmpX;
					tmpY2 = (float) BorderWall [i] -> y * EDGE - tmpY;
					tmpX2 = tmpX2 * ((float) SQUARE / (float) EDGE);
					tmpY2 = tmpY2 * ((float) SQUARE / (float) EDGE);
					BorderWall [i] -> changeLocation (GLOBAL_W / 2 + (int) tmpX2 - SQUARE / 2, GLOBAL_H / 2 + (int) tmpY2 - SQUARE / 2);
					BorderWall [i] -> draw ();
				}
			
			plBackground -> draw (); //рисую фон, который закрывает обрубки стен которые дергаются
			pl -> draw (); 
			button [indexTimePlBut] -> draw (); //рисую кнопку где отображается время (не хотелось захламлять код лишними if)
			button [indexDeathPlayerBut] -> draw (); //рисую кнопку где отображается количество смертей на уровне
			for (int i = 0; i < 4; i++)
				button [indexControlBut [i]] -> draw ();
		}
		else if (type == menu || subtype == loadingLVL) //рисуем логотип в большинстве состояний
			logo -> draw ();

		for (int i = 0; i < NumButton; i++) //рисую кнопки
			if (button [i] -> F_draw)
				button [i] -> draw ();
		button [indexFPSBut] -> draw ();

		cursor.setPosition ((float) posMouse.x, (float) posMouse.y);	
		window -> draw (cursor);

		if (F_showMessage)
			message.draw ();

		window -> display ();
	}

	void initializeBackground (){
		Image backgroundImage;
		backgroundImage.loadFromFile ("Resources/Textures/background.png"); //фон который закрывает обрубки стен когда играет игрок
		plBackground = new Background (backgroundImage, "PlayerBackground", 0, 0, 1000 * (NUM_SQUARE) * SQUARE / 500, 1000 * (NUM_SQUARE) * SQUARE / 500, 1000, 1000); //не важно какие последние 2 параметра
		plBackground -> changeLocation (GLOBAL_W / 2, GLOBAL_H / 2);

		backgroundImage.loadFromFile ("Resources/Textures/logo2.png"); //логотип
		logo = new Background (backgroundImage, "Logo", 0, 0, W_BUTTON * 3, H_BUTTON * 3, 392, 91); 
		logo -> changeLocation (GLOBAL_W / 2, GLOBAL_H / 2 - 7 * (H_BUTTON + INTERVAL)); 
	}

	void initializeButtons (){
		Font font;
		font.loadFromFile ("Resources/Fonts/modeka.otf");

		Image buttonImage; //загрузка спрайта стен
		buttonImage.loadFromFile ("Resources/Textures/button.png");

		SubtypeState tmpS;
		int tmpI;

		NumButton = 0;

		tmpS = wholeType;
		indexControlBut [0] = NumButton + 0;
		indexControlBut [1] = NumButton + 1;
		indexControlBut [2] = NumButton + 2;
		indexControlBut [3] = NumButton + 3;
		indexTimePlBut = NumButton + 4;
		indexDeathPlayerBut = NumButton + 5;
		tmpI = NUM_SQUARE * SQUARE / 2 + (W_WIN - NUM_SQUARE * SQUARE) / 4;
		button [NumButton++] = new Static ("Rectangle: 1", "ControlRec",  font, tmpS, GLOBAL_W / 2 - tmpI, GLOBAL_H / 2 + 2 * (H_BUTTON + INTERVAL), Color (25, 36, 68));
		button [NumButton++] = new Static ("Triangle: 2",  "ControlTri",  font, tmpS, GLOBAL_W / 2 - tmpI, GLOBAL_H / 2 + 3 * (H_BUTTON + INTERVAL), Color (25, 36, 68));
		button [NumButton++] = new Static ("Circle: 3",    "ControlCir",  font, tmpS, GLOBAL_W / 2 - tmpI, GLOBAL_H / 2 + 4 * (H_BUTTON + INTERVAL), Color (25, 36, 68));
		button [NumButton++] = new Static ("Control",      "Control",     font, tmpS, GLOBAL_W / 2 - tmpI, GLOBAL_H / 2 + 1 * (H_BUTTON + INTERVAL), Color (6, 10, 25));
		button [NumButton++] = new Static ("Time: 0",      "TimePlayer",  font, tmpS, GLOBAL_W / 2 - tmpI, GLOBAL_H / 2 - 7 * (H_BUTTON + INTERVAL), Color (211, 25, 12));
		button [NumButton++] = new Static ("Death: 0",     "DeathPlayer", font, tmpS, GLOBAL_W / 2 - tmpI, GLOBAL_H / 2 - 6 * (H_BUTTON + INTERVAL), Color (211, 25, 12));

		tmpS = loadingLVL;
		button [NumButton++] = new Static ("Loading...", "LoadingLVL", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2, Color (6, 10, 25));

		tmpS = allState;
		indexFPSBut = NumButton;
		button [NumButton++] = new Static ("FPS: 0", "FPS", font, tmpS, GLOBAL_W / 2 - W_WIN / 2 + W_BUTTON / 2, GLOBAL_H / 2 + H_WIN / 2 - H_BUTTON / 2);


		tmpS = launcher;
		tmpI = H_BUTTON + INTERVAL;
		button [NumButton++] = new Button (buttonImage, "Go!",     "Go!",      font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 3 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Mode",    "Mode",     font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Setting", "Settings", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Exit",    "Exit",     font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 0 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = exitt;
		tmpI = H_BUTTON + INTERVAL;
		button [NumButton++] = new Static (             "Quit game?", "Quit game?", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * tmpI);
		button [NumButton++] = new Button (buttonImage, "No!",        "QuitNo",     font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 0 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Yes.",       "QuitYes",    font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + 1 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = settings;
		tmpI = H_BUTTON + INTERVAL;
		button [NumButton++] = new Button (buttonImage, "Controls", "ControlsSet",   font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Audio",    "AudioSet",      font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Back",     "BackToMenuSet", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 0 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = controlSeting;
		tmpI = H_BUTTON + INTERVAL;
		button [NumButton++] = new Static (                 "Rectangle:", "Rectangle",        font, tmpS, GLOBAL_W / 2 - W_BUTTON / 2, GLOBAL_H / 2 - 3 * tmpI);
		button [NumButton++] = new Static (                 "Triangle:",  "Triangle",         font, tmpS, GLOBAL_W / 2 - W_BUTTON / 2, GLOBAL_H / 2 - 2 * tmpI);
		button [NumButton++] = new Static (                 "Circle:",    "Circle",           font, tmpS, GLOBAL_W / 2 - W_BUTTON / 2, GLOBAL_H / 2 - 1 * tmpI);
		button [NumButton++] = new EditButton (buttonImage, "1",          "ChangeKey",        font, tmpS, GLOBAL_W / 2 + W_BUTTON / 4, GLOBAL_H / 2 - 3 * tmpI, W_BUTTON / 4, H_BUTTON, 47,  45);
		button [NumButton++] = new EditButton (buttonImage, "2",          "ChangeKey",        font, tmpS, GLOBAL_W / 2 + W_BUTTON / 4, GLOBAL_H / 2 - 2 * tmpI, W_BUTTON / 4, H_BUTTON, 47,  45);
		button [NumButton++] = new EditButton (buttonImage, "3",          "ChangeKey",        font, tmpS, GLOBAL_W / 2 + W_BUTTON / 4, GLOBAL_H / 2 - 1 * tmpI, W_BUTTON / 4, H_BUTTON, 47,  45);
		button [NumButton++] = new Button (buttonImage,     "Back",       "BackToControlSet", font, tmpS, GLOBAL_W / 2,                GLOBAL_H / 2 - 0 * tmpI, W_BUTTON,     H_BUTTON, 0, 188, 45);
		
		tmpS = audioSetting;
		tmpI = H_BUTTON + INTERVAL;
		button [NumButton++] = new Static (             "Sound:",  "VolSound",       font, tmpS, GLOBAL_W / 2 - W_BUTTON / 2 - tmpI, GLOBAL_H / 2 - 2 * tmpI);
		button [NumButton++] = new Static (             "Music:",  "VolMusic",       font, tmpS, GLOBAL_W / 2 - W_BUTTON / 2 - tmpI, GLOBAL_H / 2 - 1 * tmpI);
		button [NumButton++] = new Button (buttonImage, "Back",    "BackToSetAudio", font, tmpS, GLOBAL_W / 2,                       GLOBAL_H / 2 - 0 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		button [NumButton++] = new CheckButton (buttonImage,       "SwitchSound", tmpS, GLOBAL_W / 2 + W_BUTTON / 2 + 3 * tmpI / 2, GLOBAL_H / 2 - 2 * tmpI,     H_BUTTON, H_BUTTON, 47, 45);
		button [NumButton++] = new CheckButton (buttonImage,       "SwitchMusic", tmpS, GLOBAL_W / 2 + W_BUTTON / 2 + 3 * tmpI / 2, GLOBAL_H / 2 - 1 * tmpI,     H_BUTTON, H_BUTTON, 47, 45);
		button [NumButton++] = new HorizontScrollBar (buttonImage, "SoundSlider", tmpS, GLOBAL_W / 2 + W_BUTTON / 2 - tmpI,         GLOBAL_H / 2 - 2 * tmpI, 20, H_BUTTON - 4, GLOBAL_W / 2 - tmpI + 13, GLOBAL_W / 2 + W_BUTTON - tmpI - 13, 47, 45, 188, 45);
		button [NumButton++] = new HorizontScrollBar (buttonImage, "MusicSlider", tmpS, GLOBAL_W / 2 + W_BUTTON / 2 - tmpI,         GLOBAL_H / 2 - 1 * tmpI, 20, H_BUTTON - 4, GLOBAL_W / 2 - tmpI + 13, GLOBAL_W / 2 + W_BUTTON - tmpI - 13, 47, 45, 188, 45);
		
		tmpS = mode;
		tmpI = H_BUTTON + INTERVAL;
		button [NumButton++] = new Button (buttonImage, "Player", "PlayerMode", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 3 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Admin",  "AdminMode",  font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Back",   "BackToMenu", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = selectLVL;
		tmpI = GLOBAL_H / 2 - 3 * (H_BUTTON + INTERVAL);
		button [NumButton++] = new Button (buttonImage, "1",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 - 3 * (W_BUTTON) / 8, tmpI,                (W_BUTTON - 4) / 4, H_BUTTON, 1, 47,  45);
		button [NumButton++] = new Button (buttonImage, "2",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 - (W_BUTTON) / 8 - 1, tmpI,                (W_BUTTON - 4) / 4, H_BUTTON, 2, 47,  45);
		button [NumButton++] = new Button (buttonImage, "3",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 + (W_BUTTON) / 8 + 1, tmpI,                (W_BUTTON - 4) / 4, H_BUTTON, 3, 47,  45);
		button [NumButton++] = new Button (buttonImage, "4",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 + 3 * (W_BUTTON) / 8, tmpI,                (W_BUTTON - 4) / 4, H_BUTTON, 4, 47,  45);
		tmpI = GLOBAL_H / 2 - 2 * (H_BUTTON + INTERVAL);
		button [NumButton++] = new Button (buttonImage, "5",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 - 3 * (W_BUTTON) / 8, tmpI,                (W_BUTTON - 4) / 4, H_BUTTON, 5, 47,  45);
		button [NumButton++] = new Button (buttonImage, "6",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 - (W_BUTTON) / 8 - 1, tmpI,                (W_BUTTON - 4) / 4, H_BUTTON, 6, 47,  45);
		button [NumButton++] = new Button (buttonImage, "7",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 + (W_BUTTON) / 8 + 1, tmpI,                (W_BUTTON - 4) / 4, H_BUTTON, 7, 47,  45);
		button [NumButton++] = new Button (buttonImage, "8",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 + 3 * (W_BUTTON) / 8, tmpI,                (W_BUTTON - 4) / 4, H_BUTTON, 8, 47,  45);
		button [NumButton++] = new Button (buttonImage, "My lvls",    "My lvls",       font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + INTERVAL), W_BUTTON,           H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Back",       "BackToMenuSel", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 0 * (H_BUTTON + INTERVAL), W_BUTTON,           H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Static (             "Select LVL", "SelectStatic",  font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 4 * (H_BUTTON + INTERVAL));

		tmpS = playerLVL;
		tmpI = H_BUTTON + INTERVAL;
		button [NumButton++] = new EditButton (buttonImage, "",                    "InputMyLVL",      font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * tmpI, W_BUTTON, H_BUTTON,    188, 45);
		button [NumButton++] = new Static (                 "Enter name your LVL", "StaticMyLVL",     font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 3 * tmpI);
		button [NumButton++] = new Button (buttonImage,     "Back",                "BackToMenuMyLVL", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);


		Image pictureImage; //загрузка спрайта стен
		tmpS = editLVL;
		tmpI = GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new Button (buttonImage, "Back",     "BackToMenuAd", font, tmpS, GLOBAL_W / 2 - 2 * W_BUTTON - 6 * EDGE, tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Open",     "OpenAd",       font, tmpS, GLOBAL_W / 2 - 1 * W_BUTTON - 3 * EDGE, tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Save",     "SaveAd",       font, tmpS, GLOBAL_W / 2 + 0 * W_BUTTON + 0 * EDGE, tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Delete",   "DeleteAd",     font, tmpS, GLOBAL_W / 2 + 1 * W_BUTTON + 3 * EDGE, tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "List lvl", "ListAd",       font, tmpS, GLOBAL_W / 2 + 2 * W_BUTTON + 6 * EDGE, tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		pictureImage.loadFromFile ("Resources/Textures/button2.png"); 
		tmpI = GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new PictureButton (buttonImage, "Rectangle", tmpS, GLOBAL_W / 2 - 9 - H_BUTTON * 3, tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Triangle",  tmpS, GLOBAL_W / 2 - 6 - H_BUTTON * 2, tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Circle",    tmpS, GLOBAL_W / 2 - 3 - H_BUTTON,     tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Wall",      tmpS, GLOBAL_W / 2,                    tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Start",     tmpS, GLOBAL_W / 2 + 3 + H_BUTTON,     tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30); 
		button [NumButton++] = new PictureButton (buttonImage, "Finish",    tmpS, GLOBAL_W / 2 + 6 + H_BUTTON * 2, tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Save",      tmpS, GLOBAL_W / 2 + 9 + H_BUTTON * 3, tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);

		tmpS = openLVL;
		tmpI = (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new EditButton (buttonImage, "", "EditLVL",        font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - tmpI, W_BUTTON, H_BUTTON,    188, 45);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToAdminSel", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = saveLVL;
		tmpI = (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new EditButton (buttonImage, "", "AdSaveLVL",       font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - tmpI, W_BUTTON, H_BUTTON,    188, 45);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToAdminSave", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = deleteLVL;
		tmpI = (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new EditButton (buttonImage, "", "AdDeleteLVL",       font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - tmpI, W_BUTTON, H_BUTTON,    188, 45);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToAdminDelete", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = listLVL;
		tmpI = (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new Button (buttonImage, "Back", "BackToAdminList", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);


		tmpS = play;
		tmpI = NUM_SQUARE * SQUARE / 2 + (W_WIN - NUM_SQUARE * SQUARE) / 4;
		button [NumButton++] = new Button (buttonImage,    "Pause",       "BackToMenuPl", font, tmpS, GLOBAL_W / 2 + tmpI, GLOBAL_H / 2 - 0 * (H_BUTTON + INTERVAL), W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = pause;
		tmpI = GLOBAL_W / 2 + NUM_SQUARE * SQUARE / 2 + (W_WIN - NUM_SQUARE * SQUARE) / 4;
		button [NumButton++] = new Static (             "Pause",    "Pause",             font, tmpS, tmpI, GLOBAL_H / 2 - 2 * (H_BUTTON + INTERVAL), Color (25, 36, 68));
		button [NumButton++] = new Button (buttonImage, "Leave",    "LeaveToSel",        font, tmpS, tmpI, GLOBAL_H / 2 - 1 * (H_BUTTON + INTERVAL), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Settings", "SettingsIntoPause", font, tmpS, tmpI, GLOBAL_H / 2 - 0 * (H_BUTTON + INTERVAL), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Continue", "BackToPlPause",     font, tmpS, tmpI, GLOBAL_H / 2 + 1 * (H_BUTTON + INTERVAL), W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = startLVL;
		tmpI = GLOBAL_W / 2 + NUM_SQUARE * SQUARE / 2 + (W_WIN - NUM_SQUARE * SQUARE) / 4;
		button [NumButton++] = new Static ("Press Escape to leave",  "StartLVL", font, tmpS, tmpI, GLOBAL_H / 2 - 1 * (H_BUTTON + INTERVAL), Color (6, 10, 25));
		button [NumButton++] = new Static ("Press any key to start", "StartLVL", font, tmpS, tmpI, GLOBAL_H / 2 - 0 * (H_BUTTON + INTERVAL), Color (6, 10, 25));
	}

	void initializeLines (){
		
		lines = VertexArray (Lines, (NUM_CELL_Y + NUM_CELL_X + 4) * 2); //массив линий
		int i = 0; //i-счетчик линий занесенных в массив
		for (int k = 0; k < (NUM_CELL_Y + NUM_CELL_X + 2) * 2; k++)
			lines [k].color = Color (43, 34, 65); //30, 30, 30//79,75,5
		for (; i < (NUM_CELL_X + 1) * 2; i += 2){ //создание вертикальных линий
			lines [i].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H + NUM_CELL_Y * EDGE);
		}
		for (int k = 0; i < (NUM_CELL_Y + NUM_CELL_X + 2) * 2; i += 2, k += 2){ //создание горизонтальных линий
			lines [i].position = Vector2f ((float) GLOB_IND_W, (float) EDGE * k / 2 + GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + NUM_CELL_X * EDGE, (float) EDGE * k / 2 + GLOB_IND_H);
		}
	}

	void initializeSomething (){
		Start.x = GLOB_IND_H; Start.y = GLOB_IND_H + NUM_CELL_Y * EDGE; //инициализируем стартовую точку
		Finish.x = GLOB_IND_W + NUM_CELL_X * EDGE; Finish.y = GLOB_IND_H; //инициализируем финиш

		Font font;
		font.loadFromFile ("Resources/Fonts/modeka.otf");
		message.initialize (font);
		message.setColor (Color (75, 30, 34));

		Image playerImage; //зарузка спрайта игрока
		playerImage.loadFromFile ("Resources/Textures/player2.png");
		pl  = new Player (playerImage, Start.x, Start.y, SQUARE, SQUARE, 40, 40); //создание объекта игрок

		backgroundMusic.openFromFile ("Resources/Music/DJVI_-_Dry_Out.ogg"); //музыка
		backgroundMusic.play (); 
		backgroundMusic.setLoop (true);
		if (!F_musicOff) changeMusicVolume (volumeMusic);
		else             changeMusicVolume (0);

		bufferClickButt.loadFromFile ("Resources/Sounds/button-30.wav"); //звук нажатия на кнопку
		sndClickButt.setBuffer (bufferClickButt);
		bufferTeleport.loadFromFile ("Resources/Sounds/teleportation.wav"); //звук телепортации игрока
		sndTeleport.setBuffer (bufferTeleport);
		if (F_soundOff) changeSoundVolume (0);
		else            changeSoundVolume (volumeSound);

		Image tmpI; //работа со спрайтом курсора
		tmpI.loadFromFile ("Resources/Textures/cursor2.png");
		textureCursor.loadFromImage (tmpI);
		cursor.setTexture (&textureCursor);
		cursor.setTextureRect (IntRect (0, 0, 46, 54));
		cursor.setSize (Vector2f ((float) 23 * (3 * H_BUTTON / 5) / 27, (float) 3 * H_BUTTON / 5));

		char tmpC [2];
		for (int i = 0; i < 3; i++){
			if (key [i] >= 0 && key [i] <= 25){
				tmpC [0] = key [i] + 65;
				tmpC [1] = '\0';
			}
			else if (key [i] >= 26 && key [i] <= 35){
				tmpC [0] = key [i] + 22;
				tmpC [1] = '\0';
			}
			button [indexControlBut [i]] -> updateText (tmpC);
		}
	}

	void initializeWalls (){
		wallImage.loadFromFile ("Resources/Textures/wall.png");
		wallImagePL.loadFromFile ("Resources/Textures/wall2.png");
	
		NumWall = 0; //количество стен
		wallCoordinate = new bool* [NUM_CELL_X];
		for (int i = 0; i < NUM_CELL_X; i++){
			wallCoordinate [i] = new bool [NUM_CELL_Y];
			for (int j = 0; j < NUM_CELL_Y; j++)
				wallCoordinate [i][j] = false;
		}
		NumBorderWall = 0; //создание граничнх стен для игрока (что б было красиво)
		for (int i = -1; i < 65; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", i, -1, SQUARE, SQUARE, 40, 40);
		for (int i = -1; i < 65; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", i, 32, SQUARE, SQUARE, 40, 40);
		for (int i = 0; i < 32; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", -1, i, SQUARE, SQUARE, 40, 40);
		for (int i = 0; i < 32; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", 64, i, SQUARE, SQUARE, 40, 40);

	}

	Game (){ //конструктор в котором инициализируем основные параметры
		AdOrPlMode = "PlayerMode"; //строка хранящая имя текущего мода игры (игрок или админ)
		strcpy (Pass, "");
		strcpy (lvlOpenByAdmin, "");
		strcpy (playerLVLOpenByPlayer, "");
		F_showMessage = false;
		F_lvlComplete = false;
		F_isPlayerLVL = false;
		F_escapeReleased = false;
		F_changeStates = false; 
		F_secPhaseChangeState = false;
		F_inSetingIntoPause = false;
		F_changeKey = false;
		F_enlargeWholeType = false;
		F_reduceWholeType = false;
		F_loadLVL = false;
		F_playerOnStart = false;
		type = menu;
		subtype = launcher;
		whichWall = wall;
		key [0] = 27;
		key [1] = 28;
		key [2] = 29;
		CurrentLVL = 1;
		timer = 0;
		NumWall = 0;
		NumLVLDeath = 0;
		whatButChange = 0;
		indexFinish = -1;

		readInfo ();
		initializeButtons (); //вызываем остальные инициализации
		initializeSomething ();
		initializeLines ();
		initializeWalls ();
		initializeBackground ();
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
				if (timer > 0.3){ //стены можно ставить раз в 0.3 сек
					timer = 0;	
					tmpX = (int) posMouse.x; tmpX -= GLOB_IND_W; tmp = tmpX % EDGE; tmpX -= tmp; tmpX /= EDGE; //перевожу координаты мыши в игровые
					tmpY = (int) posMouse.y; tmpY -= GLOB_IND_H; tmp = tmpY % EDGE; tmpY -= tmp; tmpY /= EDGE;
					for (int i = 0; i < NumWall; i++){
						if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY)
							if (ArrWall [i] -> name != "Start" && ArrWall [i] -> name != "Finish"){ //удаляю стену если это не финиш или старт
								deleted = true;
								if (ArrWall [i] -> name == "Wall")             wallDeleted = true; //запоминаю какую именно стену я удалил (понадобится в дальнейшем)
								else if (ArrWall [i] -> name == "Circle")      circleDeleted = true;
								else if (ArrWall [i] -> name == "Rectangle")   rectangleDeleted = true;
								else if (ArrWall [i] -> name == "Triangle")    triangleDeleted = true;
								else if (ArrWall [i] -> name == "Save")        saveDeleted = true;
								if (i != NumWall - 1) //меняю последнюю стену на ту которую надо удалить
									*ArrWall [i] = *ArrWall [NumWall - 1];
								if (ArrWall [NumWall - 1] -> name == "Start") //меняю индекс старта и финиша (если вдруг последней стеной оказался старт или финиш
									indexStart = i;
								else if (ArrWall [NumWall - 1] -> name == "Finish")
									indexFinish = i;
								wallCoordinate [tmpX][tmpY] = false;
								delete ArrWall [NumWall - 1]; //удаляю последнюю стену (мы туда переместили стену которую надо удалить)
								NumWall--;
								break;
							}
					}

					if (whichWall == startW){
						if (tmpX != ArrWall [indexFinish] -> x || tmpY != ArrWall [indexFinish] -> y){ //если мы хотим поставить старт не на финиш
							*ArrWall [indexStart] = *ArrWall [NumWall - 1]; //помещаю старый старт на последнее место

							if (ArrWall [NumWall - 1] -> name == "Finish") //если двруг на последнем месте был старт
								indexFinish = indexStart;

							delete ArrWall [NumWall - 1]; //удаляю старый старт
							wallCoordinate [tmpX][tmpY] = false;
							NumWall--;

							indexStart = NumWall; //меняю индекс и завожу новый
							ArrWall [NumWall++] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE, 20, 20);
							Start.x = tmpX; Start.y = tmpY;
						}
					}
					else if (whichWall == finishW){ //аналогично как со стартом
						if (tmpX != ArrWall [indexStart] -> x || tmpY != ArrWall [indexStart] -> y){
							*ArrWall [indexFinish] = *ArrWall [NumWall - 1];

							if (ArrWall [NumWall - 1] -> name == "Start")
								indexStart = indexFinish;

							delete ArrWall [NumWall - 1];
							wallCoordinate [tmpX][tmpY] = false;
							NumWall--;

							indexFinish = NumWall;
							ArrWall [NumWall++] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE, 20, 20);
							Finish.x = tmpX; Finish.y = tmpY;
						}
					}
					else{ //если мы хотим поставить не на старт и финиш
						if ((tmpX == ArrWall [indexStart] -> x && tmpY == ArrWall [indexStart] -> y) || (tmpX == ArrWall [indexFinish] -> x && tmpY == ArrWall [indexFinish] -> y))
							tmp = 0;
						else {
							if ((whichWall == wall) && !wallDeleted){
								ArrWall [NumWall++] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE, 20, 20);
								wallCoordinate [tmpX][tmpY] = true; //если это стена, то нужно заполнить массив стен
							}
							else if ((whichWall == rectangleW) && !rectangleDeleted)   ArrWall [NumWall++] = new Wall (wallImage, "Rectangle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							else if ((whichWall == triangleW) && !triangleDeleted)     ArrWall [NumWall++] = new Wall (wallImage, "Triangle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							else if ((whichWall == circleW) && !circleDeleted)         ArrWall [NumWall++] = new Wall (wallImage, "Circle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							else if ((whichWall == saveW) && !saveDeleted)             ArrWall [NumWall++] = new Wall (wallImage, "Save",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
						}
					}
				}	
	}

	void saveLVL_AD (char *tmpC){ //сохранение уровня админом
		Coordinate tmp1, tmp2; //сохраняем координаты старта и финиша
		tmp1.x = ArrWall [indexStart] -> x;
		tmp1.y = ArrWall [indexStart] -> y;
		tmp2.x = ArrWall [indexFinish] -> x;
		tmp2.y = ArrWall [indexFinish] -> y;
		quickSort (0, NumWall - 1, ArrWall); //сортируем
		indexStart = binSearch (0, NumWall - 1, ArrWall, tmp1); //после сортировки находим индексы старта и финиша
		indexFinish = binSearch (0, NumWall - 1, ArrWall, tmp2);

		ofstream outF (tmpC);
		int tmp = 0;
		outF << NumWall << endl;
		outF << (Start.x - GLOB_IND_W) / EDGE << " " << (Start.y - GLOB_IND_H) / EDGE << endl; //координаты старта, надо перевести
		outF << NumLVLDeath << endl;
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
				wallCoordinate [i][j] = false;
		NumAnsw = 0;

		ifstream inF (tmpC);
		inF >> NumWall; 
		inF >> Start.x >> Start.y;
		inF >> NumLVLDeath;
		Start.x = Start.x * EDGE + GLOB_IND_W; //координаты старта, надо перевести
		Start.y = Start.y * EDGE + GLOB_IND_H;

		for (int i = 0; i < NumWall; i++){
			inF >> tmpX >> tmpY >> tmpC;
			ArrWall [i] = new Wall (wallImagePL, tmpC, tmpX, tmpY, SQUARE, SQUARE, 40, 40);

			if (strcmp (tmpC, "Wall") == 0)
				wallCoordinate [tmpX][tmpY] = true;
			else if (strcmp (tmpC, "Start") == 0)
				indexStart = i;
			else if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX * EDGE + GLOB_IND_W; 
				Finish.y = tmpY * EDGE + GLOB_IND_H; indexFinish = i;
			}
		}

		Coordinate tmp1, tmp2; //сохраняем координаты старта и финиша
		tmp1.x = ArrWall [indexStart] -> x;
		tmp1.y = ArrWall [indexStart] -> y;
		tmp2.x = ArrWall [indexFinish] -> x;
		tmp2.y = ArrWall [indexFinish] -> y;
		quickSort (0, NumWall - 1, ArrWall); //сортируем
		indexStart = binSearch (0, NumWall - 1, ArrWall, tmp1); //после сортировки находим индексы старта и финиша
		indexFinish = binSearch (0, NumWall - 1, ArrWall, tmp2);
	}

	void openLVL_AD (char *tmpName){
		char tmpC [50];
		strcpy (tmpC, tmpName);
		int tmpX, tmpY;

		for (int i = 0; i < NumWall; i++)
			ArrWall [i] -> ~Wall ();
		for (int i = 0; i < NUM_CELL_X; i++)
			for (int j = 0; j < NUM_CELL_Y; j++)
				wallCoordinate [i][j] = false;
		NumAnsw = 0;

		ifstream inF (tmpC);
		inF >> NumWall; 
		inF >> Start.x >> Start.y;
		Start.x = Start.x * EDGE + GLOB_IND_W; //координаты старта, надо перевести
		Start.y = Start.y * EDGE + GLOB_IND_H;
		inF >> NumLVLDeath;

		for (int i = 0; i < NumWall; i++){
			inF >> tmpX >> tmpY >> tmpC;
			ArrWall [i] = new Wall (wallImage, tmpC, tmpX, tmpY, EDGE, EDGE, 20, 20);

			if (strcmp (tmpC, "Wall") == 0)
				wallCoordinate [tmpX][tmpY] = true;
			else if (strcmp (tmpC, "Start") == 0)
				indexStart = i;
			else if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX * EDGE + GLOB_IND_W; 
				Finish.y = tmpY * EDGE + GLOB_IND_H; indexFinish = i;
			}
		}

		Coordinate tmp1, tmp2; //сохраняем координаты старта и финиша
		tmp1.x = ArrWall [indexStart] -> x;
		tmp1.y = ArrWall [indexStart] -> y;
		tmp2.x = ArrWall [indexFinish] -> x;
		tmp2.y = ArrWall [indexFinish] -> y;
		quickSort (0, NumWall - 1, ArrWall); //сортируем
		indexStart = binSearch (0, NumWall - 1, ArrWall, tmp1); //после сортировки находим индексы старта и финиша
		indexFinish = binSearch (0, NumWall - 1, ArrWall, tmp2);
	}

	void inputKeyboard (char *tmpC){ //ввод с клавиатуры
		if (event.type == Event::KeyPressed){
			mcText *text;
			Font font;
			font.loadFromFile ("Resources/Fonts/modeka.otf");
			text = new mcText (&font); //создаем текст который будет отображаться на кнопке
			text -> changeSize (SIZE_TEXT); //размер текста
			text -> add (tmpC);
			if (Keyboard::isKeyPressed (Keyboard::BackSpace)){ //если нажата кнопка стереть     
				int i = strlen (tmpC);
				if (i > 0){
					char tmpC2 [50];
					strcpy (tmpC2, tmpC);
					strncpy (tmpC, tmpC2, i - 1);
					tmpC [i - 1] = '\0';
				}
			}
			else if (text -> w < (W_BUTTON - SIZE_TEXT)){
				char tmpC2 [2];
				if (event.key.code >= 0 && event.key.code <= 25){ //буквы от А до Z
					tmpC2 [0] = event.key.code + 97;
					tmpC2 [1] = '\0';
					strcat (tmpC, tmpC2);
				}
				else if (event.key.code >= 26 && event.key.code <= 35){ //цифры от 0 до 9
					tmpC2 [0] = event.key.code + 22;
					tmpC2 [1] = '\0';
					strcat (tmpC, tmpC2);
				}
			}
		}
			
	}

	void createWay (){ //функция создания выхода из лабиринта
		Coordinate fn, sizeMap, st;
		st.x = (Start.x - GLOB_IND_W) / EDGE;
		st.y = (Start.y - GLOB_IND_H) / EDGE;
		fn.x = (Finish.x - GLOB_IND_W) / EDGE;
		fn.y = (Finish.y - GLOB_IND_H) / EDGE;
		pl -> currDirection = 0;
		sizeMap.x = NUM_CELL_X;
		sizeMap.y = NUM_CELL_Y;
		outputSearch (wallCoordinate, fn, st, sizeMap);
	}

	void changeState (){ //функция изменения состояния
		if (!F_secPhaseChangeState){ //первая фаза (уменьшение кнопок)
			bool tmpB = true;
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> subtype == whichStateWas){
					if (button [i] -> F_transformation == false && tmpB){ //уменьшaем покa доконца не уменьшили одну кнопку
						F_secPhaseChangeState = true; F_reduceWholeType = false;
						subtype = whichStateWill; type = findType (subtype);
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> subtype == whichStateWill){
								button [i] -> F_draw  = true;
								button [i] -> clear ();
							}
							else
								button [i] -> F_draw  = false;

						if (whichStateWas == listLVL){ //мы создали динамически кнопки, надо их удалить когда закрываем список
							for (int k = NumButton - 1; k > NumButton - NumListLVL - 1; k--)
								delete button [k];
							NumButton -= NumListLVL;
							NumListLVL = 0;
						}
					}

					button [i] -> EFF_reduce ();
				}
				else if (F_reduceWholeType && button [i] -> subtype == wholeType)
					button [i] -> EFF_reduce (); 
		}
		else{ //вторя фаза (увелечение кнопок)
			bool tmpB = true;
			for (int i = 0; i < NumButton; i++){
				if (button [i] -> subtype == whichStateWill){
					button [i] -> EFF_enlarge (); 
					if (button [i] -> F_transformation == false && tmpB){ //увеличиваем пока одну доконца не увеличили
						F_changeStates = false; F_secPhaseChangeState = false; F_enlargeWholeType = false; tmpB = false;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> subtype == subtype)
								button [i] -> F_draw = true;
							else
								button [i] -> F_draw = false;
					}
				}
				else if (F_enlargeWholeType && button [i] -> subtype == wholeType)
					button [i] -> EFF_enlarge (); 
			}
		}
	}

	void startChangeState (SubtypeState tmpS){ //вспомогательная функция для изменения состояния
		sndClickButt.play ();
		F_changeStates = true; 
		whichStateWas = subtype; whichStateWill = tmpS;
		TypeState tmpT = findType (tmpS);
		if ((tmpT == player && type == unknown) || (tmpT == player && type == menu))
			F_enlargeWholeType = true;
		else if (tmpT == menu && type == player)
			F_reduceWholeType = true;
		for (int i = 0; i < NumButton; i++){
			if (button [i] -> subtype == whichStateWas)
				button [i] -> F_transformation = true;
			else if (button [i] -> subtype == whichStateWill)
				button [i] -> F_transformation = true;
			else if (button [i] -> subtype == wholeType && F_enlargeWholeType){
				button [i] -> F_transformation = true; button [i] -> F_draw = true; button [i] -> clear ();
			}
			else if (button [i] -> subtype == wholeType && F_reduceWholeType)
				button [i] -> F_transformation = true;
		}
	}


	void StateLauncher (){
		if (F_changeStates)
			changeState ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (button [i] -> F_click && button [i] -> name == "Mode" && !F_changeStates){
					startChangeState (mode);
					break;
				}
				else if (((button [i] -> F_click && button [i] -> name == "Go!") || F_enterReleased) && !F_changeStates){
					if (AdOrPlMode == "AdminMode"){
						NumWall = 0;
						ArrWall [NumWall++] = new Wall (wallImage, "Start", 0, 0, EDGE, EDGE, 20, 20);
						ArrWall [NumWall++] = new Wall (wallImage, "Finish", 1, 0, EDGE, EDGE, 20, 20);
						indexStart = 0;
						indexFinish = 1;
						whichWall = wall;
						startChangeState (editLVL);
						backgroundMusic.pause ();
					}
					if (AdOrPlMode == "PlayerMode"){  
						writeInfo ();
						startChangeState (selectLVL);
					}
					break;
				}
				else if (button [i] -> F_click && button [i] -> name == "Settings" && !F_changeStates){
					startChangeState (settings); break;
				}
				else if (((button [i] -> F_click && button [i] -> name == "Exit") || F_escapeReleased) && !F_changeStates){
					writeInfo ();
					startChangeState (exitt); break;
				}
			}
	}
	void StateMode (){
		if (F_changeStates)
			changeState ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (((button [i] -> F_click && button [i] -> name == "BackToMenu") || F_escapeReleased) && !F_changeStates){
					startChangeState (launcher); break;
				}
				else if (button [i] -> F_click && button [i] -> name == "AdminMode" && !F_changeStates){
					 sndClickButt.play ();
					 if (PassedLVL == 8)
						 AdOrPlMode = "AdminMode"; 
					 else
						 message.showMessage (GLOBAL_W / 2, GLOBAL_H / 2 - 4 * (H_BUTTON + INTERVAL), "Finish all levels", 0.8f);
					 break;
				}
				else if (button [i] -> F_click && button [i] -> name == "PlayerMode" && !F_changeStates){
					sndClickButt.play (); AdOrPlMode = "PlayerMode"; break;
				}
			}
	}
	void StateSettings (){
		if (F_changeStates)
			changeState ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (((button [i] -> F_click && button [i] -> name == "BackToMenuSet") || F_escapeReleased) && !F_changeStates){
					writeInfo ();
					if (!F_inSetingIntoPause)
						startChangeState (launcher);
					else{
						startChangeState (pause);
						F_inSetingIntoPause = false;
					}
					break;
				}
				else if (button [i] -> F_click && button [i] -> name == "ControlsSet" && !F_changeStates){
					char tmpC [2];

					for (int k = 0; k < NumButton; k++)
						if (button [k] -> name == "ChangeKey"){
							if (key [button [k] -> value - 1] >= 0 && key [button [k] -> value - 1] <= 25){
								tmpC [0] = key [button [k] -> value - 1] + 65;
								tmpC [1] = '\0';
							}
							else if (key [button [k] -> value - 1] >= 26 && key [button [k] -> value - 1] <= 35){
								tmpC [0] = key [button [k] -> value - 1] + 22;
								tmpC [1] = '\0';
							}
							button [k] -> updateText (tmpC);
						}
					startChangeState (controlSeting); 
					break;
				}
				else if (button [i] -> F_click && button [i] -> name == "AudioSet" && !F_changeStates){
					startChangeState (audioSetting); break;
				}
			}
	}
	void StateExitt (){
		if (F_changeStates)
			changeState ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (((button [i] -> F_click && button [i] -> name == "QuitNo") || F_escapeReleased) && !F_changeStates){
					startChangeState (launcher); break;
				}
				else if (((button [i] -> F_click && button [i] -> name == "QuitYes") || F_enterReleased) && !F_changeStates){
					sndClickButt.play (); 
					window -> close (); break;
				}
			}
	}
	void StateSelectLVL (){
		if (F_changeStates)
			changeState ();
		char tmpC2 [30];
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (button [i] -> F_click && button [i] -> name == "SelectLVL" && !F_changeStates){
					if (button [i] -> value <= PassedLVL + 1){
						CurrentLVL = button [i] -> value;
						_itoa (button [i] -> value, tmpC2, 10);
						
						char nameFile [30] = "Resources/LVLs/lvl";
						strcat (nameFile, tmpC2);
						strcat (nameFile, ".txt");
						strcpy (lvlOpenByPlayer, nameFile);
						
						F_isPlayerLVL = false;
						timer = 0;
						startChangeState (loadingLVL);
						
					}
				}
				else if (((button [i] -> F_click && button [i] -> name == "BackToMenuSel") || F_escapeReleased) && !F_changeStates){
					startChangeState (launcher); break;
				}
				else if (button [i] -> F_click && button [i] -> name == "My lvls" && !F_changeStates){
					readInfo ();
					startChangeState (playerLVL); break;
				}
			}
	}
	void StateAudioSeting (){
		if (F_changeStates)
			changeState ();	

		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (((button [i] -> F_click && button [i] -> name == "BackToSetAudio") || F_escapeReleased) && !F_changeStates){
					writeInfo ();
					startChangeState (settings);
					break;
				}
				else if (button [i] -> F_click && button [i] -> name == "MusicSlider" && !F_changeStates){
					sndClickButt.play (); break;
				}
				else if (button [i] -> F_click && button [i] -> name == "SoundSlider" && !F_changeStates){
					sndClickButt.play (); break;
				}
				else if (button [i] -> F_click && button [i] -> name == "SwitchMusic" && !F_changeStates){
					sndClickButt.play ();
					if (F_musicOff) changeMusicVolume (0);
					else            changeMusicVolume (volumeMusic);
					break;
				}
				else if (button [i] -> F_click && button [i] -> name == "SwitchSound" && !F_changeStates){
					sndClickButt.play (); 
					if (F_soundOff) changeSoundVolume (0);
					else            changeSoundVolume (volumeSound);
					break;
				}
			}
	}
	void StateControlSeting (){
		if (F_changeStates)
			changeState ();

		if (F_anyKeyReleased && F_changeKey){
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> name == "ChangeKey" && button [i] -> value == whatButChange){
					char tmpC [2];
					bool tmpB = true;

					for (int k = 0; k < 3; k++)
						if (key [k] == event.key.code && k != button [i] -> value - 1)
							tmpB = false;

					if (event.key.code >= 0 && event.key.code <= 25 && tmpB){
						tmpC [0] = event.key.code + 65;
						tmpC [1] = '\0';
						button [i] -> updateText (tmpC);
						whatButChange = -1;
						key [button [i] -> value - 1] = event.key.code;
						button [indexControlBut [button [i] -> value - 1]] -> updateText (tmpC);
					}
					else if (event.key.code >= 26 && event.key.code <= 35 && tmpB){
						tmpC [0] = event.key.code + 22;
						tmpC [1] = '\0';
						button [i] -> updateText (tmpC);
						whatButChange = -1;
						key [button [i] -> value - 1] = event.key.code;
						button [indexControlBut [button [i] -> value - 1]] -> updateText (tmpC);
					}
					break;
				}
		}

		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (((button [i] -> F_click && button [i] -> name == "BackToControlSet") || F_escapeReleased) && !F_changeStates){
					writeInfo ();
					whatButChange = -1;
					startChangeState (settings);
					break;
				}
				if (button [i] -> F_click && button [i] -> name == "ChangeKey" && !F_changeStates){
					F_changeKey = true;
					sndClickButt.play ();
					whatButChange = button [i] -> value;
					break;
				}
			}
	}
	void StatePlayerLVL (){
		if (F_changeStates)
			changeState ();
		inputKeyboard (playerLVLOpenByPlayer);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (button [i] -> name == "InputMyLVL" && !F_changeStates)
						button [i] -> updateText (playerLVLOpenByPlayer);
				if (((button [i] -> F_click && button [i] -> name == "InputMyLVL") || F_enterReleased) && !F_changeStates){
					sndClickButt.play (); 
					char tmpC [100] = "Resources/LVLs/";
					bool findLVL = false;
					ifstream inF ("Resources/LVLs/listLVLs.txt");
					char tmpC2 [30];
					int tmpI;
					inF >> tmpI;
					for (int j = 0; j < tmpI; j++){
						inF >> tmpC2;
						if (strcmp (tmpC2, playerLVLOpenByPlayer) == 0){
							if (strstr (playerLVLOpenByPlayer, "lvl") == NULL || strpbrk (playerLVLOpenByPlayer, "12345678") == NULL || strlen (playerLVLOpenByPlayer) > 4){
								strcat (tmpC, playerLVLOpenByPlayer);
								strcat (tmpC, ".txt");
								strcpy (lvlOpenByPlayer, tmpC);
								

								timer = 0;
								F_isPlayerLVL = true;
								findLVL = true;
								startChangeState (loadingLVL);
							}
							break;
						}
					}

					if (!findLVL){
						for (int k = 0; k < NumButton; k++)
							if (button [k] -> name == "InputMyLVL"){
								strcpy (playerLVLOpenByPlayer, "");
								break;
							}
						message.showMessage (GLOBAL_W / 2, GLOBAL_H / 2 - 4 * (H_BUTTON + INTERVAL), "Level not found", 0.7f);
					}
				}
				else if (((button [i] -> F_click && button [i] -> name == "BackToMenuMyLVL") || F_escapeReleased) && !F_changeStates)
					startChangeState (selectLVL);
			}
	}

	void StateEditLVL (){
		timer += time;
		if (F_changeStates)
			changeState ();
		createWalls ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (button [i] -> F_click && button [i] -> name == "SaveAd" && !F_changeStates){
					startChangeState (saveLVL); break;
				}
				else if (button [i] -> F_click && button [i] -> name == "OpenAd" && !F_changeStates){
					startChangeState (openLVL); break;
				}
				else if (button [i] -> F_click && button [i] -> name == "DeleteAd" && !F_changeStates){
					startChangeState (deleteLVL); break;
				}
				else if (button [i] -> F_click && button [i] -> name == "ListAd" && !F_changeStates){
					Font font;
					font.loadFromFile ("Resources/Fonts/modeka.otf");
					ifstream inF ("Resources/LVLs/listLVLs.txt");
					char tmpC [50];
					int tmpI;
					SubtypeState tmpS = listLVL;
					inF >> tmpI;
					for (int i = 0; i < tmpI; i++){
						inF >> tmpC;
						if (strstr (tmpC, "lvl") == NULL || strpbrk (tmpC, "12345678") == NULL || strlen (tmpC) > 4){
							button [NumButton++] = new Static (tmpC, "ListLVL", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + NUM_CELL_Y * EDGE / 2 - H_BUTTON * (NumListLVL + 1));
							NumListLVL++;
						}
					}
					button [NumButton++] = new Static ("List of LVLs:", "ListLVL", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + NUM_CELL_Y * EDGE / 2 - H_BUTTON * (NumListLVL + 1));
					NumListLVL++;

					startChangeState (listLVL); 

					break;
				}
				else if (((button [i] -> F_click && button [i] -> name == "BackToMenuAd") || F_escapeReleased) && !F_changeStates){
					startChangeState (launcher); timer = 0; 
					backgroundMusic.play ();
					break;
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
	void StateOpenLVL (){
		if (F_changeStates)
			changeState ();
		inputKeyboard (lvlOpenByAdmin);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (button [i] -> name == "EditLVL" && !F_changeStates)
					button [i] -> updateText (lvlOpenByAdmin);
				if (((button [i] -> F_click && button [i] -> name == "EditLVL") || F_enterReleased) && !F_changeStates){
					bool findLVL = false;
					char tmpC [100] = "Resources/LVLs/";
					ifstream inF ("Resources/LVLs/listLVLs.txt");
					char tmpC2 [30];
					int tmpI;
					inF >> tmpI;
					for (int i = 0; i < tmpI; i++){
						inF >> tmpC2;
						if (strcmp (tmpC2, lvlOpenByAdmin) == 0){
							strcat (tmpC, lvlOpenByAdmin);
							strcat (tmpC, ".txt");
							openLVL_AD (tmpC);
							startChangeState (editLVL);
							findLVL = true;
							break;
						}
					}
					if (!findLVL){
						sndClickButt.play ();
						for (int k = 0; k < NumButton; k++)
							if (button [k] -> name == "EditLVL"){
								strcpy (lvlOpenByAdmin, "");
								message.showMessage (GLOBAL_W / 2 + 2 * W_BUTTON, button [k] -> y, "Level not found", 0.7f);
								break;
							}
					}
				}
				else if (((button [i] -> F_click && button [i] -> name == "BackToAdminSel") || F_escapeReleased) && !F_changeStates)
					startChangeState (editLVL);
			}
	}
	void StateSaveLVL (){
		if (F_changeStates)
			changeState ();
		inputKeyboard (lvlOpenByAdmin);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (button [i] -> name == "AdSaveLVL" && !F_changeStates)
					button [i] -> updateText (lvlOpenByAdmin);
				if (((button [i] -> F_click && button [i] -> name == "AdSaveLVL") || F_enterReleased) && !F_changeStates){
					startChangeState (editLVL);

					int tmpI; 
					char tmpC2 [100][30]; 
					bool edit = true;
					ifstream inF ("Resources/LVLs/listLVLs.txt");
					inF >> tmpI;
					if (tmpI - 8 < 16){
						for (int i = 0; i < tmpI; i++){
							inF >> tmpC2 [i];
							if (strcmp (tmpC2 [i], lvlOpenByAdmin) == 0)
								edit = false;
						}
						inF.close ();

						ofstream outF ("Resources/LVLs/listLVLs.txt"); //создали новый уровень
						if (edit){
							outF << ++tmpI << endl;
							for (int i = 0; i < tmpI - 1; i++)
								outF << tmpC2 [i] << endl;
							outF << lvlOpenByAdmin << endl;
							NumLVLDeath = 0;
						}
						else if (!edit){ //редактируем старый уровень
							outF << tmpI << endl;
							for (int i = 0; i < tmpI; i++)
								outF << tmpC2 [i] << endl;
						}
						char tmpC [100] = "Resources/LVLs/";
						strcat (tmpC, lvlOpenByAdmin);
						strcat (tmpC, ".txt");
						Start.x = ArrWall [indexStart] -> x;
						Start.y = ArrWall [indexStart] -> y;
						Start.x = Start.x * EDGE + GLOB_IND_W;
						Start.y = Start.y * EDGE + GLOB_IND_H;
						saveLVL_AD (tmpC);
					}
				}
				else if (((button [i] -> F_click && button [i] -> name == "BackToAdminSave") || F_escapeReleased) && !F_changeStates)
					startChangeState (editLVL);
			}
	}
	void StateDeleteLVL (){
		if (F_changeStates)
			changeState ();
		inputKeyboard (lvlOpenByAdmin);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (button [i] -> name == "AdDeleteLVL" && !F_changeStates)
					button [i] -> updateText (lvlOpenByAdmin);
				if (((button [i] -> F_click && button [i] -> name == "AdDeleteLVL") || F_enterReleased) && !F_changeStates){
					//startChangeState (editLVL);
					if (strstr (lvlOpenByAdmin, "lvl") == NULL || strpbrk (lvlOpenByAdmin, "12345678") == NULL || strlen (lvlOpenByAdmin) > 4){
						int tmpI; 
						char tmpC2 [100][30]; 
						bool isDelete = false;
						ifstream inF ("Resources/LVLs/listLVLs.txt");
						inF >> tmpI;
					
						for (int j = 0; j < tmpI; j++){
							inF >> tmpC2 [j];
							if (strcmp (tmpC2 [j], lvlOpenByAdmin) == 0)
								isDelete = true;
						}
						inF.close ();

						if (isDelete){
							char tmpC [50];
							strcpy (tmpC, "Resources/LVLs/");
							strcat (tmpC, lvlOpenByAdmin);
							strcat (tmpC, ".txt");
							remove (tmpC);
							ofstream outF ("Resources/LVLs/listLVLs.txt"); //создали новый уровень
							outF << tmpI - 1 << endl;
							for (int i = 0; i < tmpI; i++)
								if (strcmp (tmpC2 [i], lvlOpenByAdmin) != 0)
									outF << tmpC2 [i] << endl;
							startChangeState (editLVL);
						}
						else{
							sndClickButt.play ();
							for (int k = 0; k < NumButton; k++)
								if (button [k] -> name == "AdDeleteLVL"){
									strcpy (lvlOpenByAdmin, "");
									message.showMessage (GLOBAL_W / 2 + 2 * W_BUTTON, button [k] -> y, "Level not found", 0.7f);
									break;
								}
					
						}
					}
				}
				else if (((button [i] -> F_click && button [i] -> name == "BackToAdminDelete") || F_escapeReleased) && !F_changeStates)
					startChangeState (editLVL);
			}
	}
	void StateListLVL (){
		if (F_changeStates)
			changeState ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (((button [i] -> F_click && button [i] -> name == "BackToAdminList") || F_escapeReleased) && !F_changeStates){
					startChangeState (editLVL); break;
				}
			}
	}

	void StatePlay (){
		if (F_changeStates)
			changeState ();
		if (!F_changeStates) //именно так, без else!!
			pl -> update ();

		if (((float) pl -> x == pl -> xx) && ((float) pl -> y == pl -> yy))
			if (!F_changeStates && !pl -> F_move && !pl -> F_reduce){
				Coordinate tmp;
				int tmpIndex;
				tmp.x = (pl -> x - GLOB_IND_W) / EDGE;
				tmp.y = (pl -> y - GLOB_IND_H) / EDGE;
				tmpIndex = binSearch (0, NumWall - 1, ArrWall, tmp);
				if (tmpIndex != -1){
					if ((ArrWall [tmpIndex] -> name == "Rectangle" && pl -> state != rectangle) || (ArrWall [tmpIndex] -> name == "Circle" && pl -> state != circle) || (ArrWall [tmpIndex] -> name == "Triangle" && pl -> state != triangle)){
						if (!pl -> F_teleportation){
							sndTeleport.play (); timer = 0;
						}
						pl -> EFF_teleportation (Start.x, Start.y); 
						timer += time;
				
						if (!pl -> F_teleportation){
							startChangeState (startLVL);
							NumLVLDeath++;
							char tmpC [30]; //обновление времени и количества смертей
							button [indexDeathPlayerBut] -> updateText (_itoa (NumLVLDeath, tmpC, 10));
							createWay ();
						}
					}
					else if (ArrWall [tmpIndex] -> name == "Save"){
						Start.x = ArrWall [tmpIndex] -> x * EDGE + GLOB_IND_W;
						Start.y = ArrWall [tmpIndex] -> y * EDGE + GLOB_IND_H;
						createWay ();
					}
				}
			}

		if (!F_changeStates){
			char tmpC [30]; //обновление времени и количества смертей
			button [indexTimePlBut] -> updateText (_itoa ((int) lvlTime, tmpC, 10));
			lvlTime += time;
		}

		for (int i = 0; i < NumButton; i++)
			if (button [i] -> F_draw){
				button [i] -> checkCursor ();
				if (((button [i] -> F_click && button [i] -> name == "BackToMenuPl") || F_escapeReleased) && !F_changeStates){
					lvlTime -= time;
					F_escapeReleased = false;
					startChangeState (pause);
				}
				else if (F_lvlComplete && !F_changeStates){
					AllTime += lvlTime;
					lvlTime = 0;
					sndClickButt.play (); 
					F_lvlComplete = false;
					if (!F_isPlayerLVL){
						if (CurrentLVL < 8){
							if (PassedLVL < 8 && CurrentLVL - 1 == PassedLVL)
								PassedLVL++;
							writeInfo ();
							CurrentLVL++; 
							Start.x = ArrWall [indexStart] -> x;
							Start.y = ArrWall [indexStart] -> y;
							Start.x = Start.x * EDGE + GLOB_IND_W;
							Start.y = Start.y * EDGE + GLOB_IND_H;
							saveLVL_AD (lvlOpenByPlayer);
							char tmpC [40], *tmpC2;
							tmpC2 = _itoa (CurrentLVL, tmpC, 10);
							char nameFile [40] = "Resources/LVLs/lvl";
							strcat (nameFile, tmpC2);
							strcat (nameFile, ".txt");
							strcpy (lvlOpenByPlayer, nameFile);
							
							pl -> F_reduce = false;
							startChangeState (loadingLVL);
						}
						else{
							pl -> F_reduce = false;
							Start.x = ArrWall [indexStart] -> x;
							Start.y = ArrWall [indexStart] -> y;
							Start.x = Start.x * EDGE + GLOB_IND_W;
							Start.y = Start.y * EDGE + GLOB_IND_H;
							saveLVL_AD (lvlOpenByPlayer);
							PassedLVL = 8;
							writeInfo ();
							startChangeState (selectLVL);
						}
					}
					else{
						F_isPlayerLVL = false;
						pl -> F_reduce = false;
						NumAnsw = 0;
						Start.x = ArrWall [indexStart] -> x;
						Start.y = ArrWall [indexStart] -> y;
						Start.x = Start.x * EDGE + GLOB_IND_W;
						Start.y = Start.y * EDGE + GLOB_IND_H;
						saveLVL_AD (lvlOpenByPlayer);
						startChangeState (selectLVL);
					}
				}
			}
	}
	void StatePause (){
		if (F_changeStates)
			changeState ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> subtype == pause){
				button [i] -> checkCursor ();
				if (((button [i] -> F_click && button [i] -> name == "LeaveToSel") || F_enterReleased) && !F_changeStates){
					AllTime += lvlTime;
					lvlTime = 0;
					NumAnsw = 0;
					writeInfo ();
					saveLVL_AD (lvlOpenByPlayer);
					startChangeState (selectLVL);
					F_lvlComplete = false;
					break;
				}
				if (((button [i] -> F_click && button [i] -> name == "BackToPlPause") || F_escapeReleased) && !F_changeStates){
					startChangeState (play); break;
				}
				if (((button [i] -> F_click && button [i] -> name == "SettingsIntoPause") || F_escapeReleased) && !F_changeStates){
					startChangeState (settings); F_inSetingIntoPause = true; break;
				}
			}
	}
	void StateStartLVL (){
		if (F_changeStates)
			changeState ();
		pl -> changeFigureKey ();
		pl -> EFF_enlarge ();
		F_loadLVL = false;
		if (F_anyKeyReleased && !F_changeStates && !F_escapeReleased){
			if (event.key.code != key [0] && event.key.code != key [1] && event.key.code != key [2] && !pl -> F_enlarge)
				startChangeState (play);
		}
		else if (F_escapeReleased && !F_changeStates){
			AllTime += lvlTime; lvlTime = 0;
			NumAnsw = 0; writeInfo ();
			saveLVL_AD (lvlOpenByPlayer); 
			startChangeState (selectLVL);
			F_lvlComplete = false;
		}
	}

	void StateLoadingLVL (){
		if (F_changeStates)
			changeState ();

		timer += time;
		if (timer >= 0.5 && timer < 1.5){
			if (!F_loadLVL){
				F_loadLVL = true;
				openLVL_PL (lvlOpenByPlayer);
				char tmpC2 [30]; //обновление времени и количества смертей
				button [indexDeathPlayerBut] -> updateText (_itoa (NumLVLDeath, tmpC2, 10));
				button [indexDeathPlayerBut] -> clear ();
				button [indexTimePlBut] -> updateText ("0");
				button [indexTimePlBut] -> clear ();
				pl -> changeLocation (Start.x, Start.y);
				pl -> F_enlarge = true;
				pl -> state = rectangle;
				pl -> changeFigureStatic ();
				createWay ();		
				F_playerOnStart = true;
			}
		}
		else if (timer >= 2.5){
			timer = 0;
			startChangeState (startLVL);
		}
	}


	void update (){
		switch (subtype){
		case launcher:
			StateLauncher ();
			break;
		case mode:
			StateMode ();
			break;
		case settings:
			StateSettings ();
			break;
		case exitt:
			StateExitt ();
			break;
		case selectLVL:
			StateSelectLVL ();
			break;
		case audioSetting:
			StateAudioSeting ();
			break;
		case controlSeting:
			StateControlSeting ();
			break;
		case playerLVL:
			StatePlayerLVL ();
			break;
		
		case editLVL:
			StateEditLVL ();
			break;
		case openLVL:
			StateOpenLVL ();
			break;
		case saveLVL:
			StateSaveLVL ();
			break;
		case deleteLVL:
			StateDeleteLVL ();
			break;
		case listLVL:
			StateListLVL ();
			break;

		case play:
		//	StatePlay ();
			break;
		case pause:
			StatePause ();
			break;
		case startLVL:
			StateStartLVL ();
			break;

		case loadingLVL:
			StateLoadingLVL ();
			break;
		}
	}
};

int main (){
	System system;
	view.reset (FloatRect (0, 0, (float) system.W_WIN, (float) system.H_WIN)); //создание камеры
	setCoordinateForView ((float) system.GLOBAL_W / 2, (float) system.GLOBAL_H / 2); //двигаем камеру

	vector <VideoMode> modes = VideoMode::getFullscreenModes ();
	/*for (std::size_t i = 0; i < modes.size (); ++i){
		VideoMode mode = modes [i];
		cout << "Mode #" << i << ": "
			<< mode.width << "x" << mode.height << " - "
			<< mode.bitsPerPixel << " bpp" << endl;
	}*/
	VideoMode mode = modes [0];
	
	Game game;
	system.window = new RenderWindow (mode, "Figure", Style::Fullscreen, ContextSettings (0, 0, 1)); //создание окна
	//system.window = new RenderWindow (mode, "Figure"); //создание окна
	system.window -> setMouseCursorVisible (false); //не рисуем курсор
	system.window -> setFramerateLimit (60);
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

			if (system.event.type == Event::KeyReleased && system.event.key.code == 36) game.F_escapeReleased = true; //флаг, о том нажат ли ескейп
			else game.F_escapeReleased = false;
			if (system.event.type == Event::KeyReleased && system.event.key.code == 58) game.F_enterReleased = true; //флаг, о том нажат ли ентер
			else game.F_enterReleased = false; 
			if (system.event.type == Event::KeyReleased) game.F_anyKeyReleased = true; //флаг, о том нажата ли какая-либо кнопка
			else game.F_anyKeyReleased = false;

			if (system.event.type == Event::KeyPressed) //нужна при изменении формы игрока, что б менялась при нажатии клавиши, а не отпускании
				system.keyCodePressed = system.event.key.code;
		}
		if (!isUpdate){ //обновляем игру если не зашли в предыдущий while
			game.update ();
			game.F_escapeReleased = false;
			game.F_enterReleased = false;
			game.F_anyKeyReleased = false;
		}
		
		isUpdate = false;
		if (game.subtype == play)
			game.StatePlay ();
		system.keyCodePressed = -1;

		game.draw ();
	}
	return 0;
}