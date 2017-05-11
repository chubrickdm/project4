#include <SFML/Graphics.hpp> //�������� ���������� ��� ������ � �������� � ������������
#include <SFML/Audio.hpp> //��� ������ �� ������ � �������
#include <iostream> //��� ������ � �����
#include <fstream> //��� ������ � ������� (������������ ���� ������ ���� �� ������ � �.�.)
#include "forMcText.h" //���� ���������� �� ������ kychka-pc.ru/wiki/svobodnaya-baza-znanij-sfml/test/sistemnyj-modul-system-module/potoki-threads/sfsound-sfmusic
#include "algorithm.h" //���������� ���� ������� ��� ���������� ������������ ���� � ��������� (����� � ������) ��� �������� �����
#include <Windows.h> //����� ��� � ������ ���������� ������ ������
#include "view.h" //���������� �������
using namespace std;
using namespace sf;

#define GLOBAL_W 2240 //2240 //������������ ���������� ������ � ������� ���� ������, ������
#define GLOBAL_H 1280 //1280 //������
#define W_WIN 1366 //GetSystemMetrics (0) // ����� ��������� 1366 ���������� �� ������� ������ ����, ������
#define H_WIN 768 //GetSystemMetrics(1) // ����� ��������� 768, ������
#define EDGE 20 // + (W_WIN - 1366) / 300 //������ ����� ������
#define NUM_CELL_X 64 //���������� ������ ������ �� ������
#define NUM_CELL_Y 32 //���������� ������ ������ �� ������
#define NUM_H_LINE (NUM_CELL_Y + 1) //���������� �������������� ������, ������� ������� ����
#define NUM_V_LINE (NUM_CELL_X + 1) //���������� ������������ ������, ������� ������� ����
#define W_BUTTON 120 + (W_WIN - 1360) / 5 //������ ������
#define H_BUTTON 30 + (H_WIN - 760) / 10 //������ ������
#define GLOB_IND_W (GLOBAL_W - NUM_CELL_X * EDGE) / 2 //������ �� ������, � �������� ���������� ������� ������� ����� �����
#define GLOB_IND_H (GLOBAL_H - NUM_CELL_Y * EDGE) / 2 //������ �� ������, � �������� ���������� ������� ������� ����� �����



enum StateList {menu, mode, admin, player, backToMenu, settings, exitt, reqPass, selectLVL, AdSelectLVL, AdSaveLVL, completeLVL, pause}; //�������� ������������ ������� �������� �� ��������� ����
enum StatePlayer {rectangle, triangle, circle};
enum CreateWall {rectangleW, triangleW, circleW, wall, finishW, startW};

class System{ //�������� ����� ����, � ������� �������� ��� ����� ������
public:
	static Vector2i mousePosWin; //���������� ���� �����. ����
	static Vector2f posMouse; //���������� ���� �����. �����

	static RenderWindow *window; //����, � ������� ����������� ����

	static Event event; //�������

	static Clock clock; //�����
	static float timer; //������
	static float time; //�����

	static Music backgroundMusic; //������� ������
	static float volumBackMusic; //���������� ������� ������

	static SoundBuffer buffer; //����� ��� ����� ������� �� ������
	static Sound sndClickButt; //���� ������� �� ������
	static float volSndClickButt; //��������� �����

	static CreateWall whichWall;

	static String AdOrPlMode; //������ �������� ��� �������� ���� ���� (����� ��� �����)
	static Coordinate Start, Finish; //���������� ������ (������ ����� ��������) � ����� (���� ������ ������)
	static bool lvlComplete; //���������� �������� �������
};

//�������������� ��, ����� ��� � ���� ���������� �������� ���-��, �� ��� �������� � �� ���� ������� �����������
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


class Body : public System{ //����� ������� ������ ������� ��� ���� ������ ������� � ��������
public:
	int x, y; //����������
	int w, h; //������, ������
	Texture texture; //��������
	String name; //���
	RectangleShape shape; //��������
	int wTexture, hTexture; //��� �� �������������� ��������, ����� ����� ����� ������ ����� ����� ��������������
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


class Wall : public Body{ //����� �����
public:
	bool drawThis; //�������� �� �����
public:
	Wall (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){ //����������� � ������
		drawThis = true; 
		if (name == "Wall") //����� �������
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		if (name == "Finish") //���� ���� ������ ����
			shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		if (name == "Start") //������ ����� ����� ��������, ������� ��� �������� �������� ����, ��� � ����� ����� ��� ����� �������� � ��� ����������
			shape.setTextureRect (IntRect (0, hTexture * 3, wTexture, hTexture));
		if (name == "HelpWall") //��������������� "�����" ������� ���������� ���������� ����
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

class Background : public Body{ //����� �������� �����������
	public:
	bool drawThis;
public:
	Background (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){
		drawThis = false;
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	void changeCoord (int x2, int y2){ //������� ��������� ��������� ������� ������� ���� (����� ���� ��������� ��� ����� ������)
		x = x2 + EDGE / 2; y = y2 + EDGE / 2;
		shape.setPosition ((float) x, (float) y);
	}

	void changeCoord (int x2, int y2, bool fictiv){ //������� ��������� ��������� ������� ����, ������� ��������� ������ ���
		x = x2; y = y2;
		shape.setPosition ((float) x, (float) y);
	}

	void draw (){ 
		window -> draw (shape);
	}
};

class Player : public Body{ //����� ������
public:
	int tmpX, tmpY; //���������� ������� ������ ����� ���� �� ����� �������, ����� �������
	bool playerMove; //�������� �� �����
	StatePlayer statePl;
public:
	Player (Image &image, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, "Player", X, Y, W, H, WTexture, HTexture){ //����������� ��� �����
	    tmpX = x; tmpY = y;
	    playerMove = false;
		statePl = rectangle; shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
	}

	void update (bool **CoordWall){
		//����� ���� ������ ������������ ������ ���� �������
		if (Keyboard::isKeyPressed (Keyboard::W) && !playerMove){
			if (y != GLOB_IND_H){
				tmpY = y - EDGE; playerMove = true; //���������� ���������� ���� �� ����� ������
			}
		}
		else if (Keyboard::isKeyPressed (Keyboard::S) && !playerMove){
			if (y != GLOB_IND_H + NUM_CELL_Y * EDGE){
				tmpY = y + EDGE; playerMove = true; //���������� ���������� ���� �� ����� ������
			}
		}
		else if (Keyboard::isKeyPressed (Keyboard::A) && !playerMove){
			if (x != GLOB_IND_W){
				tmpX = x - EDGE; playerMove = true; //���������� ���������� ���� �� ����� ������
			}
		}
		else if (Keyboard::isKeyPressed (Keyboard::D) && !playerMove){
			if (x != GLOB_IND_W + (NUM_CELL_X - 1) * EDGE){
				tmpX = x + EDGE; playerMove = true; //���������� ���������� ���� �� ����� ������
			}
		}

		if (Keyboard::isKeyPressed (Keyboard::R)){
			statePl = rectangle;
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		}
		else if (Keyboard::isKeyPressed (Keyboard::E)){
			statePl = triangle;
			shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		}
		else if (Keyboard::isKeyPressed (Keyboard::C)){
			statePl = circle;
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
		}

		if ((!CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE]) && playerMove){ //���������, ��� �� ����� �� ��� ����� ���� �� ����� �������
			if (x == tmpX && y == tmpY) //���� �� ������ ���� ���� ������, �� ����� �� ��������
				playerMove = false;
			else{
				if (x < tmpX)
					x += 2; //�������� ����� ���� ��������
				if (x > tmpX)
					x -= 2;
				if (y < tmpY)
					y += 2;
				if (y > tmpY) 
					y -= 2;
			}
		}
		else{
			tmpX = x; tmpY = y;
		}
		if (x == Finish.x && y == Finish.y) //���� �� �������� ������, �� ����� �������� ������, ����������������� �� ����
			lvlComplete = true;
		else
			lvlComplete = false; 
		shape.setPosition ((float) x, (float) y); //������������� ������� ������
	}

	void changeCoord (int x2, int y2){ //������� ����� ��� ����������� ������ � ������ ���������� (����� ��� �������� ������ �������)
		x = x2; y = y2; shape.setPosition ((float) x, (float) y);
		tmpX = x; tmpY = y;
	}

	void draw (){ 
		window -> draw (shape);
	}
};


class BodyButton : public Body{ //���� ������
public:
	mcText *text; //����� ������� ��������� �� ������
	bool drawThis, buttPressed, buttClick; //�������� �� ������, ������ �� ������ � �������� �� �� ������. ����- ��� ������ � ��������� ������ ����� ������ ���� �� ������
	Font font; //�����
	StateList state; //������ ������ ����� ����� ����� ������ � ������� ��� ���������
	int value; //�������� ������
	float xText, yText; //���������� ������
public:
	BodyButton (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    Body (image, Name, X, Y, W, H, WTexture, HTexture){
	    font = Font; drawThis = false; buttClick = false; 
		buttPressed = false; state = State;
		text = new mcText (&font); //������� ����� ������� ����� ������������ �� ������
		text -> changeSize (30); //������ ������
		if (name != "Pause" && name != "Leave?")
			text -> add (Text);
		else
			text -> add (Text, Color::White);
		float tmp = (float) Text.getSize (); //�������� ������ ������ � ��������
		tmp /= 2;
		xText = (float) x - tmp * 14;
		yText = (float) y - h / 2 - 5;
		text -> setPosition (xText, yText); //����������� ����� �� ������
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

		//�.�. ������������ ������, ������� ����� �� �������, ��� � �� ��������� ����� �� ������
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

	void checkCursor (){ //������� �������� �� ������� ������ ��� ���������� ������� �� ������
		buttClick = false;
		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //���� ������ ���� ��������� �� ������
			if (Mouse::isButtonPressed (Mouse::Left)) //� ���� ������ �� ���
				buttPressed = true;
			else{
				if (buttPressed) //���� �� ������ �� ������ � ������ ���� ������, � ������ �� ������-������ �� �������� �� ���
					buttClick = true; 
				buttPressed = false;
			}
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture)); //���� ������� ������ �� �����, �� ������ ������ ��������
		}
		else{
			buttPressed = false; //���� ������ �� �� ���� �� ������ �������� ����
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
		}

		if (buttClick && (name == "AdminMode" || name == "PlayerMode")) //���� �� � state = mode, ����� ������� ����� ����, ����� (��� �������������� � �������� ����) ��� ����� (������)
			AdOrPlMode = name; //���������� �������� ������� ����� ����

		if (name == AdOrPlMode) //� ���� ��� ���������� ������� ������ ��� ������ ������� � �������, �� ������ ������� (�������������� ����������)
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

	void checkCursor (){ //������� �������� �� ������� ������ ��� ��������� ������� �� ������
		buttClick = false;
		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //���� ������ ���� ��������� �� ������
			if (Mouse::isButtonPressed (Mouse::Left)) //� ���� ������ �� ���
				buttPressed = true;
			else{
				if (buttPressed) //���� �� ������ �� ������ � ������ ���� ������, � ������ �� ������-������ �� �������� �� ���
					buttClick = true; 
				buttPressed = false;
			}
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture)); //���� ������� ������ �� �����, �� ������ ������ ��������
		}
		else{
			buttPressed = false; //���� ������ �� �� ���� �� ������ �������� ����
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
		}
	}

	void updateText (char *Pass){ //������� ���������� ������
		text -> clear ();
		text -> changeSize (30); //������ ������
		text -> add (Pass);
		float tmp = (float) strlen (Pass); //�������� ������ ������ � ��������
		tmp /= 2;
		xText = (float) x - tmp * 14;
		yText = (float) y - h / 2 - 5;
		text -> setPosition (xText, yText); //����������� ����� �� ������
		if (name == "Edit"){ //����� ������ ������ ������ ��� �����, ������� �������� �������
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
	}

	void draw (){
		text -> draw (window);
	}

	void checkCursor (){ }

	void updateText (char *Pass){
		text -> clear ();
		text -> changeSize (30); //������ ������
		char tmpC [40];
		strcpy (tmpC, "Time: ");
		strcat (tmpC, Pass);
		text -> add (tmpC, Color::Red);
		float tmp = (float) strlen (Pass); //�������� ������ ������ � ��������
		tmp /= 2;
		xText = (float) x - tmp * 14;
		yText = (float) y - h / 2 - 5;
		text -> setPosition (xText, yText); //����������� ����� �� ������
	}
};

class HorizontScrollBar : public BodyButton{
public:
	int leftBorder, rightBorder; //����� � ������ ������� �� ������� ����� ������������ ������ (�� ��� ��������� �������, �.�. ������ ����� ������)
public:
	HorizontScrollBar (Image &image, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int tmpBordL, int tmpBordR, int WTexture, int HTexture) : 
		    BodyButton (image, "", Name, Font, State, X, Y, W, H, WTexture, HTexture){ 
        leftBorder = tmpBordL; rightBorder = tmpBordR;
		if (name == "MusicSlider"){ //��������� � ����� ������ �� ���������� ������, � ������������� ������ � ������ ��������
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
		if (posMouse.x >= leftBorder && posMouse.x <= rightBorder) //���� ����� ��������� � ��������� ��� ������ �����
			if (posMouse.y >= y - h / 2 && posMouse.y <= y + h / 2){
				if (event.type == Event::MouseButtonPressed && !buttPressed){
					shape.setPosition ((float) posMouse.x, (float) y);
					x = (int) posMouse.x; buttPressed = true;
					if (name == "MusicSlider"){ //������ �������� ��������� ������
						volumBackMusic = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						backgroundMusic.setVolume (volumBackMusic); //�������� ��������� ������������� � ��������� �����������, ������ �� ����� �������
					}
					if (name == "SoundSlider"){ //������ �������� ��������� ������
						volSndClickButt = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						sndClickButt.setVolume (volSndClickButt); //�������� ��������� ������������� � ��������� �����������, ������ �� ����� �������
					}
				}
				if (buttPressed){ //���� ������ ������
					shape.setPosition ((float) posMouse.x, (float) y);
					x = (int) posMouse.x; 
					if (name == "MusicSlider"){ //������ �������� ��������� ������
						volumBackMusic = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						backgroundMusic.setVolume (volumBackMusic); //�������� ��������� ������������� � ��������� �����������, ������ �� ����� �������
					}
					if (name == "SoundSlider"){ //������ �������� ��������� ������
						volSndClickButt = (posMouse.x - leftBorder) / (rightBorder - leftBorder) * 100;
						sndClickButt.setVolume (volSndClickButt); //�������� ��������� ������������� � ��������� �����������, ������ �� ����� �������
					}
				}

				if (event.type == Event::MouseButtonPressed) //���� ����� ������, �� ������������� �����. ����
					buttClick = true;
				else
					buttClick = false;
		}
		if (event.type == Event::MouseButtonReleased) //���� ��������� �����
			buttPressed = false;


		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)) //���� ������ ���� ��������� �� ������
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture)); //���� ������� ������ �� �����, �� ������ ������ ��������
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

	void checkCursor (){ //������� �������� �� ������� ������ ��� ���������� ������� �� ������
		buttClick = false;
		if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //���� ������ ���� ��������� �� ������
			if (Mouse::isButtonPressed (Mouse::Left)) //� ���� ������ �� ���
				buttPressed = true;
			else{
				if (buttPressed){ //���� �� ������ �� ������ � ������ ���� ������, � ������ �� ������-������ �� �������� �� ���
					buttClick = true; whichWall = typeWall;
				}
				buttPressed = false;
			}
			shape.setTextureRect (IntRect (0, 30, wTexture, hTexture)); //���� ������� ������ �� �����, �� ������ ������ ��������
		}
		else{
			buttPressed = false; //���� ������ �� �� ���� �� ������ �������� ����
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
		}		

		if (typeWall == whichWall)
			picture.setTextureRect (IntRect (0, hPicture, wPicture, hPicture));
		else
			picture.setTextureRect (IntRect (0, 0, wPicture, hPicture));
	}

	void updateText (char *Pass){ }
};


class Game : public System{ //��� �������� � ������������� ���� � ���� ������
public:
	StateList state; //��������� ����
	int CurrentLVL; //������� �������
	bool PassEnter; //������ �� ������ ������� �� ����� ����
	char Pass [30]; //������
	int PassedLVL; //������� �������� �������
	bool escapeReleased; //���� ����� 1 ���� ������ ��������� (�� ��� ������, � ����� ���������)

	Image wallImage; //�������� ������� ����
	int NumWall; //���������� ����
	Wall *ArrWall [10000]; //������ ����
	bool **CoordWall; //���������� ����
	Wall *helpWall [10000]; //�������������� �����, ������� ���������� ���������� ����
	int indexFinish; //������ ������ (��� � ����� �� ������)

	VertexArray lines; //����� ������� � ����� ���� ��������, ��� � ����� ���� ��������� ������

	Background *plBackground; //������� �����������, ������, ������� ��������� ��������
	Background *plBackground2; //������� ������� ��������� ������ ���

	Player *pl; //�����
	mcText *timePlText;
	float timePl;

	int NumButton; //���������� ������
	BodyButton *button [100]; //������ ������

	char fileNameAd [50]; //��� ����� ��������� �������
public:
	void readInfo (){ //������� ���������� �� ������
		ifstream inF ("player.txt");
		inF >> PassedLVL >> volumBackMusic >> PassEnter >> volSndClickButt;
	}

	void writeInfo (){ //�������� ����������� �� ������
		ofstream outF ("player.txt");
		outF << PassedLVL << " " << volumBackMusic << " " << PassEnter << " " << volSndClickButt << endl;
	}

	void draw (){ //������� � ������������ ������� ���������
		window -> setView (view); //��������� ������
		window -> clear (Color (40, 36, 62));

		if (state == admin || state == player || state == AdSelectLVL || state == AdSaveLVL || state == pause){
			if (state != player && state != pause)
				window -> draw (lines); //����� ������ �����
			for (int j = 0; j < NumWall; j++) //����� �����
				if (ArrWall [j] -> drawThis)
					ArrWall [j] -> draw ();
		}
		
		if (state == player || state == pause){ //���
			plBackground -> draw ();
			plBackground2 -> draw (); 
		}

		if (state == admin || state == player || state == pause) //������ ����� ������ ������� ����
			if (indexFinish != -1)
				ArrWall [indexFinish] -> draw ();

		if (state == pause)
			pl -> draw ();

		for (int i = 0; i < NumButton; i++) //����� ������
			if (button [i] -> drawThis)
				button [i] -> draw ();

		if (state == player){
			pl -> draw (); //����� ������
			for (int i = 0; i < NumAnsw; i++) //����� ��������������� �����
				helpWall [i] -> draw ();
		}			

		window -> display ();
	}


	void initializeButton (){
		Font font;
		font.loadFromFile ("Resources/Fonts/modeka.otf");

		Image buttonImage; //�������� ������� ����
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
		button [NumButton++] = new HorizontScrollBar (buttonImage, "SoundSlider", font, tmpS, GLOBAL_W / 2 - 120 / 2 + 10, GLOB_IND_H + EDGE * 7 + 50, 20, H_BUTTON, GLOBAL_W / 2 - 120 / 2 + 10, GLOBAL_W / 2 - 120 / 2 + W_BUTTON - 10, 20, 30);
		button [NumButton++] = new Static (buttonImage, "Music", "VolMusic", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 100, W_BUTTON, H_BUTTON, 120, 30);
		button [NumButton++] = new HorizontScrollBar (buttonImage, "MusicSlider", font, tmpS, GLOBAL_W / 2 - 120 / 2 + 10, GLOB_IND_H + EDGE * 7 + 100, 20, H_BUTTON, GLOBAL_W / 2 - 120 / 2 + 10, GLOBAL_W / 2 - 120 / 2 + W_BUTTON - 10, 20, 30);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuSet", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 150, W_BUTTON, H_BUTTON, 0, 120, 30);

		tmpS = mode;
		button [NumButton++] = new Button (buttonImage, "Player", "PlayerMode", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Admin", "AdminMode", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 50, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenu", font, tmpS, GLOBAL_W / 2, GLOB_IND_H + EDGE * 7 + 100, W_BUTTON, H_BUTTON, 0, 120, 30);

		Image pictureImage; //�������� ������� ����
		tmpS = admin;
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuAd", font, tmpS, GLOBAL_W / 2 - W_WIN / 4, GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Open", "OpenAd", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Save", "SaveAd", font, tmpS, GLOBAL_W / 2 + W_WIN / 4, GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/circle.png");
		button [NumButton++] = new PictureButton (buttonImage, "Circle", font, tmpS, GLOBAL_W / 2 - 16 - W_BUTTON / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, 30, 30, EDGE, EDGE, pictureImage, 30, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/rectangle.png");
		button [NumButton++] = new PictureButton (buttonImage, "Rectangle", font, tmpS, GLOBAL_W / 2 - 48 - W_BUTTON / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, 30, 30, EDGE, EDGE, pictureImage, 30, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/triangle.png");
		button [NumButton++] = new PictureButton (buttonImage, "Triangle", font, tmpS, GLOBAL_W / 2 - 80 - W_BUTTON / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, 30, 30, EDGE, EDGE, pictureImage, 30, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/wall.png");
		button [NumButton++] = new PictureButton (buttonImage, "Wall", font, tmpS, GLOBAL_W / 2 + 16 + W_BUTTON / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, 30, 30, EDGE, EDGE, pictureImage, 30, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/start.png");
		button [NumButton++] = new PictureButton (buttonImage, "Start", font, tmpS, GLOBAL_W / 2 + 48 + W_BUTTON / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, 30, 30, EDGE, EDGE, pictureImage, 30, 30);
		pictureImage.loadFromFile ("Resources/Textures/Wall&Floor/finish.png");
		button [NumButton++] = new PictureButton (buttonImage, "Finish", font, tmpS, GLOBAL_W / 2 + 80 + W_BUTTON / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, 30, 30, EDGE, EDGE, pictureImage, 30, 30);


		tmpS = player;
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuPl", font, tmpS, GLOBAL_W / 2 - W_WIN / 6, GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Help", "HelpPl", font, tmpS, GLOBAL_W / 2 + W_WIN / 6, GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4, W_BUTTON, H_BUTTON, 0, 120, 30);
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
		button [NumButton++] = new EditButton (buttonImage, "", "EditLVL", font, tmpS, GLOBAL_W / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, W_BUTTON, H_BUTTON, 120, 30);

		tmpS = AdSaveLVL;
		button [NumButton++] = new EditButton (buttonImage, "", "AdSaveLVL", font, tmpS, GLOBAL_W / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, W_BUTTON, H_BUTTON, 120, 30);

		tmpS = completeLVL;
		button [NumButton++] = new Button (buttonImage, "End lvl", "lvlComplete", font, tmpS, GLOBAL_W / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, W_BUTTON, H_BUTTON, 0, 120, 30);
	}

	void initializeLine (){
		lines = VertexArray (Lines, (NUM_H_LINE + NUM_V_LINE + 2) * 2); //������ �����
		int i = 0; //i-������� ����� ���������� � ������
		for (; i < NUM_V_LINE * 2; i += 2){ //�������� ������������ �����
			lines [i].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H + NUM_CELL_Y * EDGE);
		}
		for (int k = 0; i < (NUM_H_LINE + NUM_V_LINE) * 2; i += 2, k += 2){ //�������� �������������� �����
			lines [i].position = Vector2f ((float) GLOB_IND_W, (float) EDGE * k / 2 + GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + NUM_CELL_X * EDGE, (float) EDGE * k / 2 + GLOB_IND_H);
		}
	}

	void initialize (){
		Start.x = GLOB_IND_H; Start.y = GLOB_IND_H + NUM_CELL_Y * EDGE; //�������������� ��������� �����
		Finish.x = GLOB_IND_W + NUM_CELL_X * EDGE; Finish.y = GLOB_IND_H; //�������������� �����

		state = menu; //�������������� ���������

		Image playerImage; //������� ������� ������
		playerImage.loadFromFile ("Resources/Textures/player.png");
		pl  = new Player (playerImage, Start.x, Start.y, EDGE, EDGE, 20, 20); //�������� ������� �����

		timer = 0;

		Image backgroundImage; //������ ���
		backgroundImage.loadFromFile ("Resources/Textures/PlayerBackGround/background.png");
		plBackground = new Background (backgroundImage, "PlayerBackground", 0, 0, 2560, 1280, 2560, 1280);

		Image backgroundImage2; //����������� ���
		backgroundImage2.loadFromFile ("Resources/Textures/PlayerBackGround/background2.png");
		plBackground2 = new Background (backgroundImage2, "PlayerBackground", 0, 0, GLOBAL_W, GLOBAL_H, GLOBAL_W, GLOBAL_H);
		plBackground2 -> changeCoord (GLOBAL_W / 2, GLOBAL_H / 2, 0);

		readInfo ();

		strcpy (fileNameAd, "");

		backgroundMusic.openFromFile ("Resources/Music/deadbolt.ogg"); //������
		backgroundMusic.play (); 
		backgroundMusic.setLoop (true);
		backgroundMusic.setVolume (volumBackMusic);

		buffer.loadFromFile ("Resources/Sounds/button-30.wav"); //����
		sndClickButt.setBuffer (buffer);
		sndClickButt.setVolume (volSndClickButt);

		escapeReleased = false;
		whichWall = wall;


		Font font;
		font.loadFromFile ("Resources/Fonts/modeka.otf");
		timePlText = new mcText (&font); //������� ����� ������� ����� ������������ �� ������
		timePlText -> changeSize (30); //������ ������
		timePlText -> add ("", Color::Red);
		timePlText -> setPosition ((float) GLOBAL_W / 2 + EDGE * NUM_CELL_X / 2 - 50, (float) GLOBAL_H / 2 - EDGE * NUM_CELL_Y / 2 - 30); //����������� ����� �� ������
		timePl = 0;
	}

	void initializeWall (){
		wallImage.loadFromFile ("Resources/Textures/wall.png");
	
		NumWall = 0; //���������� ����
		CoordWall = new bool* [NUM_CELL_X];
		for (int i = 0; i < NUM_CELL_X; i++){
			CoordWall [i] = new bool [NUM_CELL_Y];
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		}
	}

	Game (){ //����������� � ������� �������������� �������� ���������
		AdOrPlMode = "PlayerMode"; //������ �������� ��� �������� ���� ���� (����� ��� �����)
		lvlComplete = false; //���������� �������� �� ������ �������
		CurrentLVL = 1;
		PassEnter = false;
		strcpy (Pass, "");
		state = menu;
		timer = 0;
		indexFinish = -1;

		initialize (); //�������� ��������� �������������
		initializeButton ();
		initializeLine ();
		initializeWall ();
	}


	void createWalls (){ //�������� ���� � ����� ����
		int tmpX, tmpY, tmpX2, tmpY2;
		int tmp; 
		bool wallDeleted = false;
		bool circleDeleted = false;
		bool rectangleDeleted = false;
		bool triangleDeleted = false;
		if (Mouse::isButtonPressed (Mouse::Left))
			if ((posMouse.x >= GLOB_IND_W) && (posMouse.x <= GLOB_IND_W + NUM_CELL_X * EDGE) && (posMouse.y >= GLOB_IND_H) && (posMouse.y <= GLOB_IND_H + NUM_CELL_Y * EDGE))
				if (timer > 400){
					timer = 0;	
					tmpX = (int) posMouse.x; tmp = tmpX % EDGE; tmpX -= tmp; 
					tmpY = (int) posMouse.y; tmp = tmpY % EDGE; tmpY -= tmp; 
					tmpX2 = (tmpX - GLOB_IND_W) / EDGE; tmpY2 = (tmpY - GLOB_IND_H) / EDGE;
					for (int i = 0; i < NumWall; i++){
						if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY && ArrWall [i] -> name != "Start" && ArrWall [i] -> name != "Finish"){
							for (int j = i; j < NumWall - 1; j++)
								ArrWall [j] =  ArrWall [j + 1];
							NumWall--;
							CoordWall [tmpX2][tmpY2] = false;
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
									CoordWall [(ArrWall [i] -> x - GLOB_IND_W) / EDGE][(ArrWall [i] -> y - GLOB_IND_H) / EDGE] = false;
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
									CoordWall [(ArrWall [i] -> x - GLOB_IND_W) / EDGE][(ArrWall [i] -> y - GLOB_IND_H) / EDGE] = false;
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
									CoordWall [tmpX2][tmpY2] = true;
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
								ArrWall [NumWall++] = new Wall (wallImage, "Rectangle", tmpX, tmpY, EDGE, EDGE, 20, 20);
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
								ArrWall [NumWall++] = new Wall (wallImage, "Triangle", tmpX, tmpY, EDGE, EDGE, 20, 20);
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
								ArrWall [NumWall++] = new Wall (wallImage, "Circle", tmpX, tmpY, EDGE, EDGE, 20, 20);
							}
						}
						circleDeleted = false;
						break;
								 }
					}
				}	
	}

	void saveFile (char *tmpC){ //���������� ������ �������
		ofstream outF (tmpC);
		int tmp = 0;
		for (int i = 0; i < NumWall; i++){
			if (ArrWall [i] -> drawThis)
				tmp++;
		}
		outF << tmp << endl;
		for (int i = 0; i < NumWall; i++){
			if (ArrWall [i] -> drawThis){
				if (ArrWall [i] -> name == "Wall")
					outF << ArrWall [i] -> x / EDGE << " " << ArrWall [i] -> y / EDGE << " Wall" << endl;
				else if (ArrWall [i] -> name == "Start")
					outF << ArrWall [i] -> x / EDGE << " " << ArrWall [i] -> y / EDGE << " Start" << endl;
				else if (ArrWall [i] -> name == "Finish")
					outF << ArrWall [i] -> x / EDGE << " " << ArrWall [i] -> y / EDGE<< " Finish" << endl;
				else if (ArrWall [i] -> name == "Rectangle")
					outF << ArrWall [i] -> x / EDGE << " " << ArrWall [i] -> y / EDGE<< " Rectangle" << endl;
				else if (ArrWall [i] -> name == "Circle")
					outF << ArrWall [i] -> x / EDGE << " " << ArrWall [i] -> y / EDGE<< " Circle" << endl;
				else if (ArrWall [i] -> name == "Triangle")
					outF << ArrWall [i] -> x / EDGE << " " << ArrWall [i] -> y / EDGE<< " Triangle" << endl;
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
				ArrWall [i] = new Wall (wallImage, "Wall", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
				CoordWall [(tmpX * EDGE - GLOB_IND_W) / EDGE][(tmpY * EDGE - GLOB_IND_H) / EDGE] = true;
			}
			else if (strcmp (tmpC, "Rectangle") == 0)
				ArrWall [i] = new Wall (wallImage, "Rectangle", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Circle") == 0)
				ArrWall [i] = new Wall (wallImage, "Circle", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Triangle") == 0)
				ArrWall [i] = new Wall (wallImage, "Triangle", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Start") == 0){
				Start.x = tmpX * EDGE; Start.y = tmpY * EDGE;
				ArrWall [i] = new Wall (wallImage, "Start", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
			}
			else if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX * EDGE; Finish.y = tmpY * EDGE; indexFinish = i; indexFinishUpdate = true;
				ArrWall [i] = new Wall (wallImage, "Finish", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
			}
		}
		if (!indexFinishUpdate)
			indexFinish = -1;
	}

	void openSpecificFile (char *nameFile){ //�������� ������ ����� ����� �������
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
				ArrWall [i] = new Wall (wallImage, "Wall", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
				CoordWall [(tmpX * EDGE - GLOB_IND_W) / EDGE][(tmpY * EDGE - GLOB_IND_H) / EDGE] = true;
			}
			else if (strcmp (tmpC, "Rectangle") == 0)
				ArrWall [i] = new Wall (wallImage, "Rectangle", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Circle") == 0)
				ArrWall [i] = new Wall (wallImage, "Circle", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Triangle") == 0)
				ArrWall [i] = new Wall (wallImage, "Triangle", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
			else if (strcmp (tmpC, "Start") == 0){
				Start.x = tmpX * EDGE; Start.y = tmpY * EDGE;
				ArrWall [i] = new Wall (wallImage, "Start", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
				ArrWall [i] -> drawThis = false;
			}
			else if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX * EDGE; Finish.y = tmpY * EDGE; indexFinish = i; indexFinishUpdate = true;
				ArrWall [i] = new Wall (wallImage, "Finish", tmpX * EDGE, tmpY * EDGE, EDGE, EDGE, 20, 20);
			}
		}
		if (!indexFinishUpdate)
			indexFinish = -1;
	}

	void inputKeyboard (char *tmpC, bool fictiv){ //���� � ����������
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

	String toString(int val){
		std::ostringstream oss;
		oss<< val;
		return oss.str ();
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
						ArrWall [NumWall++] = new Wall (wallImage, "Start", GLOB_IND_W, GLOB_IND_H, EDGE, EDGE, 20, 20);
						ArrWall [NumWall++] = new Wall (wallImage, "Finish", GLOB_IND_W + EDGE, GLOB_IND_H + EDGE, EDGE, EDGE, 20, 20);
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
				if (Keyboard::isKeyPressed (Keyboard::Num1))       whichWall = triangleW;
				else if (Keyboard::isKeyPressed (Keyboard::Num2))  whichWall = rectangleW;
				else if (Keyboard::isKeyPressed (Keyboard::Num3))  whichWall = circleW;
			}
	}
	void StatePlayer (){
		//timer += time;
		pl -> update (CoordWall);
		plBackground -> changeCoord (pl [0]. x, pl [0].y);

		for (int i = 0; i < NumWall; i++){
			if (ArrWall [i] -> x == pl -> x && ArrWall [i] -> y == pl -> y){
				if (ArrWall [i] -> name == "Rectangle" && pl -> statePl != rectangle){
					pl -> changeCoord (Start.x, Start.y);
					break;
				}
				else if (ArrWall [i] -> name == "Circle" && pl -> statePl != circle){
					pl -> changeCoord (Start.x, Start.y);
					break;
				}
				else if (ArrWall [i] -> name == "Triangle" && pl -> statePl != triangle){
					pl -> changeCoord (Start.x, Start.y);
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
					if (strcmp (tmpC2, "120") == 0){
						pl -> x = Start.x;
						pl -> y = Start.y;
					}
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
				}
				if ((button [i] -> buttClick && button [i] -> name == "lvlComplete") || (lvlComplete && Keyboard::isKeyPressed (Keyboard::Return))){
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
						char nameFile [30] = "lvl";
						strcat (nameFile, tmpC2);
						strcat (nameFile, ".txt");
						openSpecificFile (nameFile);
						pl -> changeCoord (Start.x, Start.y);
					}
				}
				if (button [i] -> buttClick && button [i] -> name == "HelpPl"){
					sndClickButt.play (); 
					if (timer > 2000){ //20 000 = 5 ������ ��������� �������, 40 000=15, 80 000=30
						timer = 0;
						Coordinate fn, sizeMap, st;
						sizeMap.x = NUM_CELL_X;
						sizeMap.y = NUM_CELL_Y;
						st.x = (pl [0].x - GLOB_IND_W) / EDGE;
						st.y = (pl [0].y - GLOB_IND_H) / EDGE;
						fn.x = (Finish.x - GLOB_IND_W) / EDGE;
						fn.y = (Finish.y - GLOB_IND_H) / EDGE;
						outputSearch (CoordWall, st, fn, sizeMap);

						//cout << endl << NumAnsw << endl;
						int tmp = NumAnsw;
						if (NumAnsw >= 60) 
							NumAnsw = 20;
						else
							if (NumAnsw >= 30 && NumAnsw < 60) 
								NumAnsw = 10;
							else
								NumAnsw = 0;
						int j;
						for (int i = 0; i < NumAnsw; i++){
							j = tmp - i - 1;
							helpWall [i] = new Wall (wallImage, "HelpWall", Arr [j].x * EDGE + GLOB_IND_W, Arr [j].y * EDGE + GLOB_IND_H, EDGE, EDGE, 20, 20);
						}
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
						char nameFile [30] = "lvl";
						strcat (nameFile, tmpC2);
						strcat (nameFile, ".txt");
						openSpecificFile (nameFile);
						pl [0].changeCoord (Start.x, Start.y);
						plBackground -> changeCoord (Start.x, Start.y);
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
						openFileAd (fileNameAd);
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
						saveFile (fileNameAd);
					}
				}
	}
	void StatePause (){
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> state == pause){
				button [i] -> checkCursor ();
				if ((button [i] -> buttClick && button [i] -> name == "LeaveYes") || Keyboard::isKeyPressed (Keyboard::Return)){
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
		}
	}
};

int main (){
	view.reset (FloatRect (0, 0, (float) W_WIN, (float) H_WIN)); //�������� ������
	setCoordinateForView (GLOBAL_W / 2, GLOBAL_H / 2); //������� ������ � �������

	System system;
	Game game;
	system.window = new RenderWindow (VideoMode (W_WIN, H_WIN), "LABYRINTH PRO"/*, Style::Fullscreen*/); //�������� ����
	bool isUpdate = false;

	while (system.window -> isOpen ()){
		system.time = (float) system.clock.getElapsedTime ().asMicroseconds(); //����� ������ ��� �����������
		system.clock.restart ();
		system.time = system.time / 800;
		system.timer += system.time;

		system.mousePosWin = Mouse::getPosition (system.window [0]); //���������� ���� �����. ����
		system.posMouse = system.window -> mapPixelToCoords (system.mousePosWin); //���������� ���� �����. �����


		while (system.window -> pollEvent (system.event)){ //������� � ���� ����� ���-�� ���������� (����� ������� ������ ��� ��������� �� �������)
			if (system.event.type == Event::Closed) //������� ���� ����� � ��������
				system.window -> close (); 
			isUpdate = true;
			game.update ();
			if (system.event.type == Event::KeyReleased && system.event.key.code == 36) //���� ������ ������
				game.escapeReleased = true;
			else
				game.escapeReleased = false;
		}		
		if (!isUpdate){ //��������� ���� ���� �� ����� � ���������� while
			game.update (); game.escapeReleased = false;
		}
		if (game.state == player) //��������� ������ ������
			game.StatePlayer ();
		isUpdate = false;
		game.draw ();
	}
	return 0;
}