//#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp> //�������� ���������� ��� ������ � �������� � ������������
#include <iostream> //��� ������ � �����
#include <fstream> //��� ������ � �������
#include "forMcText.h" //���� ���������� �� ������ kychka-pc.ru/wiki/svobodnaya-baza-znanij-sfml/test/sistemnyj-modul-system-module/potoki-threads/sfsound-sfmusic
#include "algorithm.h" //���������� ���� ������� ��� ���������� ������������ ���� � ��������� (����� � ������) ��� �������� �����
#include <Windows.h>
#include "view.h"
#include <string.h>
using namespace std;
using namespace sf;

#define GLOBAL_W 2240
#define GLOBAL_H 1280
#define W_WIN 1366 //GetSystemMetrics (0) //������ ���� 700/ ����� ��������� 1366
#define H_WIN 768 //GetSystemMetrics(1) //������ ���� 500/ ����� ��������� 768
#define EDGE 20 //������ ����� ������
#define NUM_CELL_X 64
#define NUM_CELL_Y 32
#define INDENTATION_X (W_WIN - EDGE * NUM_CELL_X) / 2 //������
#define INDENTATION_Y (H_WIN - EDGE * NUM_CELL_Y) / 2 //������
#define NUM_H_LINE (NUM_CELL_Y + 1) //���������� �������������� ������, ������� ������� ����
#define NUM_V_LINE (NUM_CELL_X + 1) //���������� ������������ ������, ������� ������� ����
#define W_BUTTON 120
#define H_BUTTON 30
#define GLOB_IND_W (GLOBAL_W - NUM_CELL_X * EDGE) / 2
#define GLOB_IND_H (GLOBAL_H - NUM_CELL_Y * EDGE) / 2



enum StateList {menu, mode, admin, player, backToMenu, settings, exitt, reqPass, selectLVL, AdSelectLVL, AdSaveLVL, completeLVL}; //�������� ������������ ������� �������� �� ��������� ����
String AdOrPlMode; //������ �������� ��� �������� ���� ���� (����� ��� �����)
Coordinate Start, Finish; //���������� ������ (������ ����� ��������) � ����� (���� ������ ������)
bool lvlComplete; //���������� �������� �� ������ �������

class System{
public:
	static Vector2i mousePosWin; //���������� ���� �����. ����
	static Vector2f posMouse; //���������� ���� �����. �����

	static RenderWindow *window;

	static Event event;

	static Clock clock; //�����
	static float timer;
	static float time;
};

Vector2i System::mousePosWin; //���������� ���� �����. ����
Vector2f System::posMouse; //���������� ���� �����. �����

RenderWindow* System::window;

Event System::event;

Clock System::clock; //�����
float System::timer;
float System::time;

class Body : public System{
public:
	int x, y; //����������
	int w, h; //������, ������
	Texture texture; //��������
	Sprite sprite; //������
	String name; //���
public:
	Body (Image &image, String Name, int X, int Y, int W, int H){
		x = X; y = Y; w = W; h = H; name = Name;
		texture.loadFromImage (image); 
		sprite.setTexture (texture);
		sprite.setTextureRect (IntRect (0, 0, w, h));
		sprite.setPosition ((float) x, (float) y);
	}

	virtual void draw () = 0;
};


class Wall : public Body{ //����� �����
public:
	bool drawThis; //�������� �� �����
public:
	Wall (Image &image, String Name, int X, int Y, int W, int H) : Body (image, Name, X, Y, W, H){ //����������� � ������
		drawThis = true; 
		if (name == "Wall") //����� �������
			sprite.setTextureRect (IntRect (0, 0, w, h));
		if (name == "Finish") //���� ���� ������ ����
			sprite.setTextureRect (IntRect (0, h, w, h));
		if (name == "Start") //������ ����� ����� ��������, ������� ��� �������� �������� ����, ��� � ����� ����� ��� ����� �������� � ��� ����������
			sprite.setTextureRect (IntRect (0, h * 2, w, h));
		if (name == "HelpWall") //��������������� "�����" ������� ���������� ���������� ����
			sprite.setTextureRect (IntRect (0, h * 3, w, h));
	}

	void draw (){ 
		window -> draw (sprite);
	}
};


class Background : public Body{
	public:
	bool drawThis;
public:
	Background (Image &image, String Name, int X, int Y, int W, int H) : Body (image, Name, X, Y, W, H){
		drawThis = false;
		sprite.setOrigin ((float) w / 2, (float) h / 2);
	}

	void changeCoord (int x2, int y2){
		x = x2 + EDGE / 2; y = y2 + EDGE / 2;
		sprite.setPosition ((float) x, (float) y);
	}

	void changeCoord (int x2, int y2, bool fictiv){
		x = x2; y = y2;
		sprite.setPosition ((float) x, (float) y);
	}

	void draw (){ 
		window -> draw (sprite);
	}
};


class Player : public Body{
public:
	int tmpX, tmpY; //���������� ������� ������ ����� ���� �� ����� �������, ����� �������
	bool playerMove;
public:
	Player (Image &image, int X, int Y, int W, int H) : Body (image, "Player", X, Y, W, H){ //����������� ��� �����
	    tmpX = x; tmpY = y;
	    playerMove = false;
	}

	void update (bool **CoordWall){
		if (Keyboard::isKeyPressed (Keyboard::W) && !playerMove){
			if (y != GLOB_IND_H){
				tmpY = y - EDGE; playerMove = true;
			}
		}
		else if (Keyboard::isKeyPressed (Keyboard::S) && !playerMove){
			if (y != GLOB_IND_H + NUM_CELL_Y * EDGE){
				tmpY = y + EDGE; playerMove = true;
			}
		}
		else if (Keyboard::isKeyPressed (Keyboard::A) && !playerMove){
			if (x != GLOB_IND_W){
				tmpX = x - EDGE; playerMove = true;
			}
		}
		else if (Keyboard::isKeyPressed (Keyboard::D) && !playerMove){
			if (x != GLOB_IND_W + (NUM_CELL_X - 1) * EDGE){
				tmpX = x + EDGE; playerMove = true;
			}
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
		sprite.setPosition ((float) x, (float) y); //������������� ������� ������
	}

	void changeCoord (int x2, int y2){ //������� ����� ��� ����������� ������ � ������ ���������� (����� ��� �������� ������ �������)
		x = x2; y = y2; sprite.setPosition ((float) x, (float) y);
		tmpX = x; tmpY = y;
	}

	void draw (){ 
		window -> draw (sprite);
	}
};


class BodyButton : public Body{
public:
	mcText *text; //����� ������� ��������� �� ������
	bool drawThis, buttPressed, buttClick; //�������� �� ������, ������ �� ������ � �������� �� �� ������. ����- ��� ������ � ��������� ������ ����� ������ ���� �� ������
	Font font;
	StateList state; //������ ������ ����� ����� ����� ������ � ������� ��� ���������
	int value;
public:
	BodyButton (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H) : 
		    Body (image, Name, X, Y, W, H){
	    font = Font; drawThis = false; buttClick = false; 
		buttPressed = false; state = State;
		text = new mcText (&font); //������� ����� ������� ����� ������������ �� ������
		text -> changeSize (30); //������ ������
		text -> add (Text);
		float tmp = (float) Text.getSize (); //�������� ������ ������ � ��������
		tmp = x + 50 - (tmp / 2) * 12; //�������� ����� � ������ ������ (����� ��������, �.�. ���������� ������ ����, �� ������� ������ ���������� ����, � �� ������ ������)
		text -> setPosition ((float) tmp, (float) y - 5); //����������� ����� �� ������
	}

	void draw (){ }

	virtual void checkCursor () = 0;
};

class Button : public BodyButton{
public:
	Button (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int Value) : 
		    BodyButton (image, Text, Name, Font, State, X, Y, W, H){
		value = Value;
		if (state == menu)
			drawThis = true;
		else
			drawThis = false;
		if (name == "SelectLVL")
			text -> setPosition ((float) x + 3, (float) y - 5); //����������� ����� �� ������
	}

	void draw (){
		window -> draw (sprite);
		text -> draw (window);
	}

	void checkCursor (){ //������� �������� �� ������� ������ ��� ��������� ������� �� ������
		buttClick = false;
		if ((posMouse.x >= x) && (posMouse.x <= x + w) && (posMouse.y >= y) && (posMouse.y <= y + h)){ //���� ������ ���� ��������� �� ������
			if (Mouse::isButtonPressed (Mouse::Left)) //� ���� ������ �� ���
				buttPressed = true;
			else{
				if (buttPressed) //���� �� ������ �� ������ � ������ ���� ������, � ������ �� ������-������ �� �������� �� ���
					buttClick = true; 
				buttPressed = false;
			}
			sprite.setTextureRect (IntRect (0, h, w, h)); //���� ������� ������ �� �����, �� ������ ������ ��������
		}
		else{
			buttPressed = false; //���� ������ �� �� ���� �� ������ �������� ����
			sprite.setTextureRect (IntRect (0, 0, w, h));
		}

		if (buttClick && (name == "AdminMode" || name == "PlayerMode")) //���� �� � state = mode, ����� ������� ����� ����, ����� (��� �������������� � �������� ����) ��� ����� (������)
			AdOrPlMode = name; //���������� �������� ������� ����� ����

		if (name == AdOrPlMode) //� ���� ��� ���������� ������� ������ ��� ������ ������� � �������, �� ������ ������� (�������������� ����������)
			sprite.setTextureRect (IntRect (0, h * 2, w, h));
	}
};

class EditButton : public BodyButton{
public:
	EditButton (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H) : 
		    BodyButton (image, Text, Name, Font, State, X, Y, W, H){ }

	void draw (){
		window -> draw (sprite);
		text -> draw (window);
	}

	void checkCursor (){ //������� �������� �� ������� ������ ��� ��������� ������� �� ������
		buttClick = false;
		if ((posMouse.x >= x) && (posMouse.x <= x + w) && (posMouse.y >= y) && (posMouse.y <= y + h)){ //���� ������ ���� ��������� �� ������
			if (Mouse::isButtonPressed (Mouse::Left)) //� ���� ������ �� ���
				buttPressed = true;
			else{
				if (buttPressed) //���� �� ������ �� ������ � ������ ���� ������, � ������ �� ������-������ �� �������� �� ���
					buttClick = true; 
				buttPressed = false;
			}
			sprite.setTextureRect (IntRect (0, h, w, h)); //���� ������� ������ �� �����, �� ������ ������ ��������
		}
		else{
			buttPressed = false; //���� ������ �� �� ���� �� ������ �������� ����
			sprite.setTextureRect (IntRect (0, 0, w, h));
		}
	}
};

class Static : public BodyButton{
public:
	Static (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H) : 
		    BodyButton (image, Text, Name, Font, State, X, Y, W, H){ 
		if (name == "RequestPass"){
			float tmp = (float) Text.getSize (); //�������� ������ ������ � ��������
			tmp = x + 50 - (tmp / 2) * 12;
			text -> setPosition ((float) tmp - 20, (float) y - 5);
		}
		if (name == "SelectStatic"){
			text -> setPosition ((float) x - 19, (float) y - 5);
		}
	}

	void draw (){
		text -> draw (window);
	}

	void checkCursor (){ };
};



class Game : public System{
public:
	StateList state;
	int CurrentLVL;
	bool PassEnter;
	char Pass [30];
	int PassedLVL;

	Image wallImage; //�������� ������� ����
	int NumWall; //���������� ����
	Wall *ArrWall [10000]; //������ ����
	bool **CoordWall; //���������� ����
	Wall *helpWall [10000];
	int indexFinish;

	VertexArray lines;

	Player *pl;

	Background *plBackground;
	Background *plBackground2;

	int NumButton; //���������� ������
	BodyButton *button [70]; //������ ������

	char fileNameAd [50];

	Music backgroundMusic;
	float volumBackMusic;
public:
	void draw (){
		window -> setView (view); //��������� ������
		window -> clear (Color (40, 36, 62));
		if (state == admin || state == player || state == AdSelectLVL || state == AdSaveLVL){
			window -> draw (lines); //����� ������ �����
			for (int j = 0; j < NumWall; j++) //����� �����
				if (ArrWall [j] -> drawThis)
					ArrWall [j] -> draw ();
		}
		
		if (state == player){ //���
			plBackground -> draw ();
			plBackground2 -> draw (); 
		}

		if (state == admin || state == player)
			if (indexFinish != -1)
				ArrWall [indexFinish] -> draw ();

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
		font.loadFromFile ("modeka.otf");

		Image buttonImage; //�������� ������� ����
		buttonImage.loadFromFile ("button.png");

		StateList tmpS;

		NumButton = 0;

		tmpS = menu;
		button [NumButton++] = new Button (buttonImage, "Go!", "Go!", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7, 120, 30, 0);
		button [NumButton++] = new Button (buttonImage, "Mode", "Mode", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 50, 120, 30, 0);
		button [NumButton++] = new Button (buttonImage, "Setting", "Settings", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 100, 120, 30, 0);
		button [NumButton++] = new Button (buttonImage, "Exit", "Exit", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 150, 120, 30, 0);

		tmpS = settings;
		button [NumButton++] = new Button (buttonImage, "Volume", "Volume", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 100, 120, 30, 0);
		button [NumButton++] = new Button (buttonImage, "", "BackMusicSlider", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 100, 20, 30, 0);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuSet", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 150, 120, 30, 0);

		tmpS = mode;
		button [NumButton++] = new Button (buttonImage, "Player", "PlayerMode", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7, 120, 30, 0);
		button [NumButton++] = new Button (buttonImage, "Admin", "AdminMode", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 50, 120, 30, 0);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenu", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 100, 120, 30, 0);

		tmpS = admin;
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuAd", font, tmpS, GLOB_IND_W - (W_WIN - NUM_CELL_X * EDGE) / 2 + (W_WIN - 3 * 120) / 4, ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2 + GLOB_IND_H + NUM_CELL_Y * EDGE, 120, 30, 0);
		button [NumButton++] = new Button (buttonImage, "Open", "OpenAd", font, tmpS, GLOB_IND_W - (W_WIN - NUM_CELL_X * EDGE) / 2 + (W_WIN - 3 * 120) / 2 + 120, ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2 + GLOB_IND_H + NUM_CELL_Y * EDGE, 120, 30, 0);
		button [NumButton++] = new Button (buttonImage, "Save", "SaveAd", font, tmpS, GLOB_IND_W - (W_WIN - NUM_CELL_X * EDGE) / 2 + 3 * (W_WIN - 3 * 120) / 4 + 120 * 2, ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2 + GLOB_IND_H + NUM_CELL_Y * EDGE, 120, 30, 0);

		tmpS = player;
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuPl", font, tmpS, GLOB_IND_W - (W_WIN - NUM_CELL_X * EDGE) / 2 + (W_WIN - 2 * 120) / 3, ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2 + GLOB_IND_H + NUM_CELL_Y * EDGE, 120, 30, 0);
		button [NumButton++] = new Button (buttonImage, "Help", "HelpPl", font, tmpS, GLOB_IND_W - (W_WIN - NUM_CELL_X * EDGE) / 2 + 2 * (W_WIN - 2 * 120) / 3 + 120, ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2 + GLOB_IND_H + NUM_CELL_Y * EDGE, 120, 30, 0);

		tmpS = reqPass;
		button [NumButton++] = new EditButton (buttonImage, "", "Edit", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 100, 120, 30);
		button [NumButton++] = new Static (buttonImage, "Enter 4 characters", "RequestPass", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 150, 120, 30);

		tmpS = selectLVL;
		button [NumButton++] = new Button (buttonImage, "1", "SelectLVL", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 200, 29, 30, 1);
		button [NumButton++] = new Button (buttonImage, "2", "SelectLVL", font, tmpS, GLOBAL_W / 2 - 120 / 2 + 30, GLOB_IND_H + EDGE * 7 + 200, 29, 30, 2);
		button [NumButton++] = new Button (buttonImage, "3", "SelectLVL", font, tmpS, GLOBAL_W / 2 - 120 / 2 + 60, GLOB_IND_H + EDGE * 7 + 200, 29, 30, 3);
		button [NumButton++] = new Button (buttonImage, "4", "SelectLVL", font, tmpS, GLOBAL_W / 2 - 120 / 2 + 90, GLOB_IND_H + EDGE * 7 + 200, 29, 30, 4);
		button [NumButton++] = new Static (buttonImage, "Select LVL", "SelectStatic", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 150, 120, 30);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuSel", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H + EDGE * 7 + 100, 120, 30, 0);

		tmpS = AdSelectLVL;
		button [NumButton++] = new EditButton (buttonImage, "", "EditLVL", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, 120, 30);

		tmpS = AdSaveLVL;
		button [NumButton++] = new EditButton (buttonImage, "", "AdSaveLVL", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, 120, 30);

		tmpS = completeLVL;
		button [NumButton++] = new Button (buttonImage, "End lvl", "lvlComplete", font, tmpS, GLOBAL_W / 2 - 120 / 2, GLOB_IND_H - 30 - ((H_WIN - NUM_CELL_Y * EDGE) / 2 - 30) / 2, 120, 30, 0);
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
		playerImage.loadFromFile ("player.png");
		pl  = new Player (playerImage, Start.x, Start.y, EDGE, EDGE); //�������� ������� �����

		timer = 0;

		Image backgroundImage;
		backgroundImage.loadFromFile ("background.png");
		plBackground = new Background (backgroundImage, "PlayerBackground", 0, 0, 2560, 1280);

		Image backgroundImage2;
		backgroundImage2.loadFromFile ("background2.png");
		plBackground2 = new Background (backgroundImage2, "PlayerBackground", 0, 0, GLOBAL_W, GLOBAL_H);
		plBackground2 -> changeCoord (GLOBAL_W / 2, GLOBAL_H / 2, 0);

		ifstream inF ("player.txt");
		inF >> PassedLVL;

		strcpy (fileNameAd, "");

		volumBackMusic = 100;
		backgroundMusic.openFromFile ("deadbolt.ogg");
		backgroundMusic.play (); 
		backgroundMusic.setLoop (true);
		backgroundMusic.setVolume (volumBackMusic);
	}

	void initializeWall (){
		wallImage.loadFromFile ("wall.png");
	
		NumWall = 0; //���������� ����
		CoordWall = new bool* [NUM_CELL_X];
		for (int i = 0; i < NUM_CELL_X; i++){
			CoordWall [i] = new bool [NUM_CELL_Y];
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		}
	}

	Game (){
		AdOrPlMode = "PlayerMode"; //������ �������� ��� �������� ���� ���� (����� ��� �����)
		lvlComplete = false; //���������� �������� �� ������ �������
		CurrentLVL = 1;
		PassEnter = false;
		strcpy (Pass, "");
		PassedLVL = 0;
		state = menu;
		timer = 0;
		indexFinish = -1;

		initialize ();
		initializeButton ();
		initializeLine ();
		initializeWall ();
	}

	void createWalls (){
		int tmpX, tmpY, tmpX2, tmpY2;
		int tmp; bool wallDeleted = false;
		bool canNotPut = false;
		if (Mouse::isButtonPressed (Mouse::Left))
			if ((posMouse.x >= GLOB_IND_W) && (posMouse.x <= GLOB_IND_W + NUM_CELL_X * EDGE) && (posMouse.y >= GLOB_IND_H) && (posMouse.y <= GLOB_IND_H + NUM_CELL_Y * EDGE))
				if (timer > 200){
					timer = 0;	
					tmpX = (int) posMouse.x; tmp = tmpX % EDGE; tmpX -= tmp; 
					tmpY = (int) posMouse.y; tmp = tmpY % EDGE; tmpY -= tmp; 
					tmpX2 = (tmpX - GLOB_IND_W) / EDGE; tmpY2 = (tmpY - GLOB_IND_H) / EDGE;
					if (CoordWall [tmpX2][tmpY2]){ //��������� �� ������� �����, ���� ���� �� ����� ���������
						for (int i = 0; i < NumWall; i++)
							if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY)
								if (ArrWall [i] -> name != "Finish" && ArrWall [i] -> name != "Start"){
									ArrWall [i] -> drawThis = false;
									CoordWall [tmpX2][tmpY2] = false;
									wallDeleted = true;
									break;
								}
					}
					if(Keyboard::isKeyPressed (Keyboard::LControl)){
						for (int i = 0; i < NumWall; i++){
							if (ArrWall [i] -> name == "Start"){
								ArrWall [i] -> drawThis = false;
								CoordWall [(ArrWall [i] -> x - GLOB_IND_W) / EDGE][(ArrWall [i] -> y - GLOB_IND_H) / EDGE] = false;
								for (int j = i; j < NumWall - 1; j++)
									ArrWall [j] =  ArrWall [j + 1];
								NumWall--;
								break;
							}
							if (ArrWall [i] -> name == "Start" && ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY)
								canNotPut = true;
						}
						if (!canNotPut){
							ArrWall [NumWall++] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE);
							Start.x = tmpX; Start.y = tmpY;
						}
						canNotPut = false;
					}
					else if (Keyboard::isKeyPressed (Keyboard::LShift)){
						for (int i = 0; i < NumWall; i++){
							if (ArrWall [i] -> name == "Finish"){
								ArrWall [i] -> drawThis = false;
								CoordWall [(ArrWall [i] -> x - GLOB_IND_W) / EDGE][(ArrWall [i] -> y - GLOB_IND_H) / EDGE] = false;
								for (int j = i; j < NumWall - 1; j++)
									ArrWall [j] =  ArrWall [j + 1];
								NumWall--;
								break;
							}
							if (ArrWall [i] -> name == "Start" && ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY)
								canNotPut = true;
						}
						if (!canNotPut){
							ArrWall [NumWall++] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE);
							Finish.x = tmpX; Finish.y = tmpY;
						}
						canNotPut = false;
					}
					else if (!CoordWall [tmpX2][tmpY2] && !wallDeleted){
						bool tmpB = true;
						for (int i = 0; i < NumWall; i++){
							if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY){
								ArrWall [i] -> drawThis = true;
								tmpB = false;
							}
						}
						if (tmpB)
							ArrWall [NumWall++] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE);
						CoordWall [tmpX2][tmpY2] = true;
					}

				}	
	}


	void saveFile (char *tmpC){
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
					outF << ArrWall [i] -> x << " " << ArrWall [i] -> y << " Wall" << endl;
				if (ArrWall [i] -> name == "Start")
					outF << ArrWall [i] -> x << " " << ArrWall [i] -> y << " Start" << endl;
				if (ArrWall [i] -> name == "Finish")
					outF << ArrWall [i] -> x << " " << ArrWall [i] -> y << " Finish" << endl;
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
			if (strcmp (tmpC, "Wall") == 0)
				ArrWall [i] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE);
			if (strcmp (tmpC, "Start") == 0){
				Start.x = tmpX; Start.y = tmpY;
				ArrWall [i] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE);
			}
			if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX; Finish.y = tmpY; indexFinish = i; indexFinishUpdate = true;
				ArrWall [i] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE);
			}
			if (strcmp (tmpC, "Start") != 0){
				ArrWall [i] -> drawThis = true;
				CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE] = true;
				if (strcmp (tmpC, "Finish") == 0)
					CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE] = false;
			}
		}
		if (!indexFinishUpdate)
			indexFinish = -1;
	}

	void openSpecificFile (char *nameFile){
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
			if (strcmp (tmpC, "Wall") == 0)
				ArrWall [i] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE);
			if (strcmp (tmpC, "Start") == 0){
				Start.x = tmpX; Start.y = tmpY;
				ArrWall [i] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE);
				ArrWall [i] -> drawThis = false;
				CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE] = false;
			}
			if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX; Finish.y = tmpY; indexFinish = i; indexFinishUpdate = true;
				ArrWall [i] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE);
			}
			if (strcmp (tmpC, "Start") != 0){
				ArrWall [i] -> drawThis = true;
				CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE] = true;
				if (strcmp (tmpC, "Finish") == 0)
					CoordWall [(tmpX - GLOB_IND_W) / EDGE][(tmpY - GLOB_IND_H) / EDGE] = false;
			}
		}
		if (!indexFinishUpdate)
			indexFinish = -1;
	}

	void inputKeyboard (char *tmpC, bool fictiv){
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

	void StateMenu (){
		button [NumButton - 1] -> drawThis = false; 
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> buttClick && button [i] -> name == "Mode"){
					state = mode; 
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == mode)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
				if (button [i] -> buttClick && button [i] -> name == "Go!"){
					if (AdOrPlMode == "AdminMode"){
						openSpecificFile ("empty.txt");
						state = admin; 
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == admin)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
					if (AdOrPlMode == "PlayerMode"){
						ifstream inF ("player.txt");
						inF >> PassedLVL;
						state = selectLVL;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == selectLVL)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
				}
					if (button [i] -> buttClick && button [i] -> name == "Settings"){
						state = settings;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == settings)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
					if (button [i] -> buttClick && button [i] -> name == "Exit")
						state = exitt;
			}
	}
	void StateMode (){
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> buttClick && button [i] -> name == "BackToMenu"){
					state = menu;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == menu)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
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
					state = AdSaveLVL;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == AdSaveLVL)
							button [i] -> drawThis = true;
				}
				if (button [i] -> buttClick && button [i] -> name == "OpenAd"){
					state = AdSelectLVL;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == AdSelectLVL)
							button [i] -> drawThis = true;
				}
				if (button [i] -> buttClick && button [i] -> name == "BackToMenuAd"){
					state = menu;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == menu)
								button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
				}
			}
	}
	void StatePlayer (){
		timer += time;
		pl [0].update (CoordWall);
		plBackground -> changeCoord (pl [0]. x, pl [0].y);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> buttClick && button [i] -> name == "BackToMenuPl"){
					timer = 0;
					NumAnsw = 0;
					ifstream inF ("player.txt");
					inF >> PassedLVL;
					state = selectLVL;
					for (int i = 0; i < NumButton; i++)
						if (button [i] -> state == selectLVL)
							button [i] -> drawThis = true;
						else
							button [i] -> drawThis = false;
					lvlComplete = false;
				}
				if (button [i] -> buttClick && button [i] -> name == "lvlComplete"){
					if (CurrentLVL < 4){
						timer = 0;
						ofstream outF ("player.txt");

						outF << CurrentLVL;
						CurrentLVL++;
						char tmpC [30], *tmpC2;
						tmpC2 = _itoa (CurrentLVL, tmpC, 10);
						char nameFile [30] = "lvl";
						strcat (nameFile, tmpC2);
						strcat (nameFile, ".txt");
						//cout << nameFile << endl;
						openSpecificFile (nameFile);
						pl [0].changeCoord (Start.x, Start.y);
					}
				}
				if (button [i] -> buttClick && button [i] -> name == "HelpPl"){
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
							helpWall [i] = new Wall (wallImage, "HelpWall", Arr [j].x * EDGE + GLOB_IND_W, Arr [j].y * EDGE + GLOB_IND_H, EDGE, EDGE);
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
	void StateBackToMenu (){

	}
	void StateSettings (){
		for (int i = 0; i < NumButton; i++)
				if (button [i] -> drawThis){
					if (button [i] -> name != "BackMusicSlider")
						button [i] -> checkCursor ();
					if (button [i] -> buttClick && button [i] -> name == "BackToMenuSet"){
						state = menu;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == menu)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
				}
	}
	void StateExitt (){
		window[0].close ();
	}
	void StateReqPass (){
		inputKeyboard (Pass, 1);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> name == "Edit"){
					button [i] -> text -> clear ();
					button [i] -> text -> changeSize (30); //������ ������
					button [i] -> text -> add (Pass);
					float tmp = (float)  strlen (Pass); //�������� ������ ������ � ��������
					tmp = button [i] -> x + 50 - (tmp / 2) * 8; //�������� ����� � ������ ������ (����� ��������, �.�. ���������� ������ ����, �� ������� ������ ���������� ����, � �� ������ ������)
					button [i] -> text -> setPosition ((float) tmp, (float) button [i] -> y - 5); //����������� ����� �� ������
				}
				if ((button [i] -> buttClick && button [i] -> name == "Edit") || (event.type == Event::KeyPressed && Keyboard::isKeyPressed (Keyboard::Return))){
					if (strlen (Pass) == 4 && !PassEnter){
						if (strcmp (Pass, "4329") == 0){
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
					if (button [i] -> value <= PassedLVL + 1){
						//cout << button [i] -> value << " " << i;
						CurrentLVL = button [i] -> value;
						_itoa (button [i] -> value, tmpC2, 10);
						state = player;
						char nameFile [30] = "lvl";
						strcat (nameFile, tmpC2);
						strcat (nameFile, ".txt");
						//cout << nameFile << endl;
						openSpecificFile (nameFile);
						//cout << Start.x << " " << Start.y << endl;
						pl [0].changeCoord (Start.x, Start.y);
						//plBackground -> drawThis = true;
						plBackground -> changeCoord (Start.x, Start.y);
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == player)
								button [i] -> drawThis = true;
							else
								button [i] -> drawThis = false;
					}
				}
				if (button [i] -> buttClick && button [i] -> name == "BackToMenuSel"){
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
					if (button [i] -> name == "EditLVL"){
						button [i] -> text -> clear ();
						button [i] -> text -> changeSize (30); //������ ������
						button [i] -> text -> add (fileNameAd);
						float tmp = (float)  strlen (fileNameAd); //�������� ������ ������ � ��������
						tmp = button [i] -> x + 50 - (tmp / 2) * 8; //�������� ����� � ������ ������ (����� ��������, �.�. ���������� ������ ����, �� ������� ������ ���������� ����, � �� ������ ������)
						button [i] -> text -> setPosition ((float) tmp, (float) button [i] -> y - 5); //����������� ����� �� ������
					}
					if ((button [i] -> buttClick && button [i] -> name == "EditLVL") || (event.type == Event::KeyPressed && Keyboard::isKeyPressed (Keyboard::Return))){
						if (strlen (fileNameAd) > 0){
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
	}
	void StateAdSaveLVL (){
		inputKeyboard (fileNameAd, 0);
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> state == AdSaveLVL){
					button [i] -> checkCursor ();
					if (button [i] -> name == "AdSaveLVL"){
						button [i] -> text -> clear ();
						button [i] -> text -> changeSize (30); //������ ������
						button [i] -> text -> add (fileNameAd);
						float tmp = (float)  strlen (fileNameAd); //�������� ������ ������ � ��������
						tmp = button [i] -> x + 50 - (tmp / 2) * 8; //�������� ����� � ������ ������ (����� ��������, �.�. ���������� ������ ����, �� ������� ������ ���������� ����, � �� ������ ������)
						button [i] -> text -> setPosition ((float) tmp, (float) button [i] -> y - 5); //����������� ����� �� ������
					}
					if ((button [i] -> buttClick && button [i] -> name == "AdSaveLVL") || (event.type == Event::KeyPressed && Keyboard::isKeyPressed (Keyboard::Return))){
						if (strlen (fileNameAd) > 0){
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
	}
	void StateCompleteLVL (){

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
		case backToMenu:
			StateBackToMenu ();
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
		case completeLVL:
			StateCompleteLVL ();
			break;
		}
	}
};

int main (){
	view.reset (FloatRect (0, 0, W_WIN, H_WIN)); //�������� ������
	setCoordinateForView (GLOBAL_W / 2, GLOBAL_H / 2); //������� ������ � �������

	System system;
	Game game;
	system.window = new RenderWindow (VideoMode (W_WIN, H_WIN), "LABYRINTH PRO"/*, Style::Fullscreen*/); //�������� ����

	while (system.window -> isOpen ()){
		system.time = (float) system.clock.getElapsedTime ().asMicroseconds(); //����� ������ ��� �����������
		system.clock.restart ();
		system.time = system.time / 800;
		system.timer += system.time;

		system.mousePosWin = Mouse::getPosition (system.window [0]); //���������� ���� �����. ����
		system.posMouse = system.window -> mapPixelToCoords (system.mousePosWin); //���������� ���� �����. �����

		while (system.window [0].pollEvent (system.event)){
			if ((system.event.type == Event::Closed) || Keyboard::isKeyPressed (Keyboard::Escape)) //������� ���� ����� � ��������
				system.window -> close (); 
			game.update ();
		}		
		if (game.state == player)
			game.StatePlayer ();
		game.draw ();
	}
	return 0;
}