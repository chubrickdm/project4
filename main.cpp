//#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp> //�������� ���������� ��� ������ � �������� � ������������
#include <iostream> //��� ������ � �����
#include <fstream> //��� ������ � �������
#include "forMcText.h" //���� ���������� �� ������ kychka-pc.ru/wiki/svobodnaya-baza-znanij-sfml/test/sistemnyj-modul-system-module/potoki-threads/sfsound-sfmusic
#include "algorithm.h" //���������� ���� ������� ��� ���������� ������������ ���� � ��������� (����� � ������) ��� �������� �����
#include <Windows.h>
using namespace std;
using namespace sf;
//GetSystemMetrics(0); //������ ������ 
//GetSystemMetrics(1); //������ ������

#define W_WIN GetSystemMetrics (0) //������ ���� 700
#define H_WIN GetSystemMetrics(1) //������ ���� 500
#define EDGE 20 //������ ����� ������
#define INDENTATION 60 //������ ���� �� ������ ���� �� ��������� � ����������� � ��� ������� 60
#define NUM_H_LINE (H_WIN - 2 * INDENTATION + EDGE) / EDGE //���������� �������������� ������, ������� ������� ����
#define NUM_V_LINE (W_WIN - 2 * INDENTATION + EDGE) / EDGE //���������� ������������ ������, ������� ������� ����

enum StateList {menu, mode, admin, player, backToMenu, setting, exitt}; //�������� ������������ ������� �������� �� ��������� ����
String AdOrPlMode = "PlayerMode"; //������ �������� ��� �������� ���� ���� (����� ��� �����)
Coordinate Start, Finish; //���������� ������ (������ ����� ��������) � ����� (���� ������ ������)
bool lvlComplete = false; //���������� �������� �� ������ �������

/*class System{
public:
	static enum StateList {menu, mode, admin, player, backToMenu, setting, exitt} state;
	static String AdOrPlMode;
	static Wall *helpWall [1000];
	static int NumWall; //���������� ����
	static Wall *ArrWall [1000]; //������ ����
	static bool **CoordWall; //���������� ����
	static int NumButton; //���������� ������
	static Button *button [30]; //������ ������
	static Font font; //�����
	static Clock clock; //�����
};*/

class Wall{ //����� �����
public:
	int x, y; //����������
	int w, h; //������, ������
	bool drawThis; //�������� �� �����
	Texture texture; //��������
	Sprite sprite; //������
	String name; //���
public:
	Wall (Image &image, String Name, int X, int Y, int W, int H){ //����������� � ������
		x = X; y = Y; w = W; h = H; name = Name; drawThis = true; 
		texture.loadFromImage (image); 
		sprite.setTexture (texture);
		sprite.setTextureRect (IntRect (0, 0, w, h));
		sprite.setPosition (x, y);
		if (name == "Wall") //����� �������
			sprite.setTextureRect (IntRect (0, 0, w, h));
		if (name == "Finish") //���� ���� ������ ����
			sprite.setTextureRect (IntRect (0, h, w, h));
		if (name == "Start") //������ ����� ����� ��������, ������� ��� �������� �������� ����, ��� � ����� ����� ��� ����� �������� � ��� ����������
			sprite.setTextureRect (IntRect (0, h * 2, w, h));
		if (name == "HelpWall") //��������������� "�����" ������� ���������� ���������� ����
			sprite.setTextureRect (IntRect (0, h * 3, w, h));
	}

	Wall (Image &image, int X, int Y, int W, int H){ //����������� ��� �����
		x = X; y = Y; w = W; h = H; name = "Wall"; //�� �������� ��������� ������� �����
		drawThis = true;
		texture.loadFromImage (image);
		sprite.setTexture (texture);
		sprite.setTextureRect (IntRect (0, 0, w, h));
		sprite.setPosition (x, y);
	}
};

class Button{ //����� ������
public:
	int x, y; //����������
	int w, h; //������, ������
	bool drawThis, buttPressed, buttClick; //�������� �� ������, ������ �� ������ � �������� �� �� ������. ����- ��� ������ � ��������� ������ ����� ������ ���� �� ������
	Texture texture; //��������
	Sprite sprite; //������
	mcText *text; //����� ������� ��������� �� ������
	String name; //��� ������
	StateList state; //������ ������ ����� ����� ����� ������ � ������� ��� ���������
public:
	Button (Image &image, String tmpT, String Name, Font &font, int X, int Y, int W, int H){ //����������� � ������
		x = X; y = Y; w = W; h = H; 
		name = Name; buttPressed = false; buttClick = false; 
		texture.loadFromImage (image); 
		sprite.setTexture (texture);
		sprite.setTextureRect (IntRect (0, 0, w, h));
		sprite.setPosition (x, y);
		text = new mcText (&font); //������� ����� ������� ����� ������������ �� ������
		text -> changeSize (30); //������ ������
		text -> add (tmpT);
		float tmp = tmpT.getSize (); //�������� ������ ������ � ��������
		tmp = x + 50 - (tmp / 2) * 12; //�������� ����� � ������ ������ (����� ��������, �.�. ���������� ������ ����, �� ������� ������ ���������� ����, � �� ������ ������)
		text -> setPosition (tmp, y - 5); //����������� ����� �� ������

		if (Name == "Go!" || Name == "Mode" || Name == "Setting" || Name == "Exit"){ //������ ������-����, ������������ ��� ������� ����
			drawThis = true; state = menu;
		}
		if (Name == "PlayerMode" || Name == "AdminMode" || Name == "BackToMenu"){ //������ ������-����� � ���� ������ ������ Mode
			drawThis = false; state = mode;
		}
		if (Name == "BackToMenuAd" || Name == "OpenAd" || Name == "SaveAd"){ //������ ������-����� �� ����������� ����� � ������ �����
			drawThis = false; state = admin;
		}
		if (Name == "BackToMenuPl" || Name == "HelpPl"){ //���������� ������-����� �� ������ 
			drawThis = false; state = player;
		}
		if (Name == "lvl1Complete") //���� ������ ������� ����������, ���������� ������ ���������� �� ����
			drawThis = false;
	}

	void draw (RenderWindow &window){ //������� ��������� ������ � ������ ������� ����� ������ ������
		window.draw (sprite);
		text -> draw (&window);
	}

	void checkCursor (Vector2i mousePosWin){ //������� �������� �� ������� ������ ��� ��������� ������� �� ������
		buttClick = false;
		if ((mousePosWin.x >= x) && (mousePosWin.x <= x + w) && (mousePosWin.y >= y) && (mousePosWin.y <= y + h)){ //���� ������ ���� ��������� �� ������
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

class Player{
public:
	int x, y; //����������
	int tmpX, tmpY; //���������� ������� ������ ����� ���� �� ����� �������, ����� �������
	int w, h; //������, ������
	Texture texture; //��������
	Sprite sprite; //������
	String name; //���
	bool playerMove;
public:
	Player (Image &image, int X, int Y, int W, int H){ //����������� ��� �����
		x = X; y = Y; w = W; h = H; name = "Player"; tmpX = x; tmpY = y;
		texture.loadFromImage (image); playerMove = false;
		sprite.setTexture (texture);
		sprite.setTextureRect (IntRect (0, 0, w, h));
		sprite.setPosition (x, y);
	}

	void update (bool **CoordWall){
		if (Keyboard::isKeyPressed (Keyboard::W) && !playerMove){
			if (y != INDENTATION){
				tmpY = y - EDGE; playerMove = true;
			}
		}
		else
			if (Keyboard::isKeyPressed (Keyboard::S) && !playerMove){
				if (y != H_WIN - INDENTATION - EDGE){
					tmpY = y + EDGE; playerMove = true;
				}
			}
			else
				if (Keyboard::isKeyPressed (Keyboard::A) && !playerMove){
					if (x != INDENTATION){
						tmpX = x - EDGE; playerMove = true;
					}
				}
				else
					if (Keyboard::isKeyPressed (Keyboard::D) && !playerMove){
						if (x != W_WIN - INDENTATION - EDGE){
							tmpX = x + EDGE; playerMove = true;
						}
					}
		
		if ((!CoordWall [tmpX / EDGE - INDENTATION / EDGE][tmpY / EDGE - INDENTATION / EDGE]) && playerMove){ //���������, ��� �� ����� �� ��� ����� ���� �� ����� �������
			if (x == tmpX && y == tmpY) //���� �� ������ ���� ���� ������, �� ����� �� ��������
				playerMove = false;
			else{
				if (x < tmpX)
					x += 2; //�������� ����� ���� ��������
				else 
					x -= 2;
				if (y < tmpY)
					y += 2;
				else 
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
		sprite.setPosition (x, y); //������������� ������� ������
	}

	void changeCoord (int x2, int y2){ //������� ����� ��� ����������� ������ � ������ ���������� (����� ��� �������� ������ �������)
		x = x2; y = y2; sprite.setPosition (x, y);
	}
};

void createWalls (Vector2i &mousePosWin, float &timer, bool **CoordWall, Wall **ArrWall, int &NumWall, Image &wallImage){
	int tmpX, tmpY, tmpX2, tmpY2;
	int tmp; bool wallDeleted = false;
	if (Mouse::isButtonPressed (Mouse::Left)){
		if (mousePosWin.x >= INDENTATION && mousePosWin.x <= W_WIN -INDENTATION && mousePosWin.y >= INDENTATION && mousePosWin.y <= H_WIN - INDENTATION){
			if (timer > 200){
				timer = 0;	
				tmpX = mousePosWin.x; tmp = tmpX % EDGE; tmpX -= tmp; 
				tmpY = mousePosWin.y; tmp = tmpY % EDGE; tmpY -= tmp; 
				tmpX2 = tmpX / EDGE; tmpY2 = tmpY / EDGE;
				if (CoordWall [tmpX2 - INDENTATION / EDGE][tmpY2 - INDENTATION / EDGE]){ //��������� �� ������� �����, ���� ���� �� ����� ���������
					for (int i = 0; i < NumWall; i++)
						if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY){
							ArrWall [i] -> drawThis = false;
							CoordWall [tmpX2 - INDENTATION / EDGE][tmpY2 - INDENTATION / EDGE] = false;
							wallDeleted = true;
							break;
						}
				}
				if(Keyboard::isKeyPressed (Keyboard::LControl)){
					for (int i = 0; i < NumWall; i++)
					if (ArrWall [i] -> name == "Start"){
						ArrWall [i] -> drawThis = false;
						CoordWall [ArrWall [i] -> x / EDGE - INDENTATION / EDGE][ArrWall [i] -> y / EDGE - INDENTATION / EDGE] = false;
					}
					ArrWall [NumWall++] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE);
					Start.x = tmpX; Start.y = tmpY;
				}
				else
					if (Keyboard::isKeyPressed (Keyboard::LShift)){
						for (int i = 0; i < NumWall; i++)
							if (ArrWall [i] -> name == "Finish"){
								ArrWall [i] -> drawThis = false;
								CoordWall [ArrWall [i] -> x / EDGE - INDENTATION / EDGE][ArrWall [i] -> y / EDGE - INDENTATION / EDGE] = false;
							}
							ArrWall [NumWall++] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE);
							Finish.x = tmpX; Finish.y = tmpY;
					}
					else{
						if (!CoordWall [tmpX2 - INDENTATION / EDGE][tmpY2 - INDENTATION / EDGE] && !wallDeleted){
							bool tmpB = true;
							for (int i = 0; i < NumWall; i++){
								if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY){
									ArrWall [i] -> drawThis = true;
									tmpB = false;
								}
							}
							if (tmpB)
								ArrWall [NumWall++] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE);
							CoordWall [tmpX2 - INDENTATION / EDGE][tmpY2 - INDENTATION / EDGE] = true;
						}
					}
			}
		}
	}
}

void saveFile (Wall **ArrWall, int &NumWall){
	cout << "Enter name of file which you want save:" << endl;
	char tmpC [50];
	cin >> tmpC;
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

void openFile (Wall **ArrWall, int &NumWall, Image &wallImage, bool **CoordWall){
	int tmpX, tmpY;
	char tmpC [50];
	if (NumWall != 0){
		for (int i = 0; i < NumWall; i++)
			ArrWall [i] -> ~Wall ();
	}
	cout << "Enter name of file which you want open:" << endl;
	cin >> tmpC;
	ifstream inF (tmpC);

	//inF >> Start.x >> Start.y >> Finish.x >> Finish.y ;
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
			Finish.x = tmpX; Finish.y = tmpY;
			ArrWall [i] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE);
		}
		if (strcmp (tmpC, "Start") != 0){
			ArrWall [i] -> drawThis = true;
			CoordWall [tmpX / EDGE - INDENTATION / EDGE][tmpY / EDGE - INDENTATION / EDGE] = true;
			if (strcmp (tmpC, "Finish") == 0)
				CoordWall [tmpX / EDGE - INDENTATION / EDGE][tmpY / EDGE - INDENTATION / EDGE] = false;
		}
	}
}

void openSpecificFile (Wall **ArrWall, int &NumWall, Image &wallImage, bool **CoordWall, char* nameFile){
	int tmpX, tmpY;
	char tmpC [40];
	if (NumWall != 0){
		for (int i = 0; i < NumWall; i++)
			ArrWall [i] -> ~Wall ();
	}
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
			CoordWall [tmpX / EDGE - INDENTATION / EDGE][tmpY / EDGE - INDENTATION / EDGE] = false;
		}
		if (strcmp (tmpC, "Finish") == 0){
			Finish.x = tmpX; Finish.y = tmpY;
			ArrWall [i] = new Wall (wallImage, "Finish", tmpX, tmpY, EDGE, EDGE);
		}
		if (strcmp (tmpC, "Start") != 0){
			ArrWall [i] -> drawThis = true;
			CoordWall [tmpX / EDGE - INDENTATION / EDGE][tmpY / EDGE - INDENTATION / EDGE] = true;
			if (strcmp (tmpC, "Finish") == 0)
				CoordWall [tmpX / EDGE - INDENTATION / EDGE][tmpY / EDGE - INDENTATION / EDGE] = false;
		}
	}
}

int main (){
	RenderWindow window (VideoMode (W_WIN, H_WIN), "LABYRINTH PRO"/*, Style::Fullscreen*/); //�������� ����
//////////////////////////////////////�������� ����� ����//////////////////////////////////////////////////////
	VertexArray lines (Lines, (NUM_H_LINE + NUM_V_LINE + 2) * 2); //������ �����
	int i = 0; //i-������� ����� ���������� � ������
    for (; i < NUM_V_LINE * 2; i += 2){ //�������� ������������ �����
        lines [i].position = Vector2f (i * EDGE / 2 + INDENTATION, INDENTATION);
        lines [i + 1].position = Vector2f (i * EDGE / 2 + INDENTATION, H_WIN - INDENTATION);
	}
	for (int k = 0; i < (NUM_H_LINE + NUM_V_LINE) * 2; i += 2, k += 2){ //�������� �������������� �����
        lines [i].position = Vector2f (INDENTATION, EDGE * k / 2 + INDENTATION);
        lines [i + 1].position = Vector2f (W_WIN - INDENTATION, EDGE * k / 2 + INDENTATION);
	}
//////////////////////////////////////������//////////////////////////////////////////////////////

	Start.x = INDENTATION; Start.y = H_WIN - INDENTATION; //�������������� ��������� �����
	Finish.x = W_WIN - INDENTATION; Finish.y = INDENTATION; //�������������� �����
	StateList state = menu; //������� ������ ���������
	Image playerImage; //������� ������� ������
	playerImage.loadFromFile ("player.png");
	Player pl (playerImage, Start.x, Start.y, EDGE, EDGE); //�������� ������� �����
	Wall *helpWall [1000]; //������ ��������������� ����

//////////////////////////////////////////�����//////////////////////////////////////////////////
	Image wallImage; //�������� ������� ����
	wallImage.loadFromFile ("wall.png");
	
	int NumWall = 0; //���������� ����
	Wall *ArrWall [1000]; //������ ����
	bool **CoordWall; //���������� ����
	//cout << NUM_H_LINE << "-horizont " << NUM_V_LINE << "-vertical" << endl;
	CoordWall = new bool* [NUM_V_LINE];
	for (int i = 0; i < NUM_V_LINE; i++){
		CoordWall [i] = new bool [NUM_H_LINE];
		for (int j = 0; j < NUM_H_LINE; j++)
			CoordWall [i][j] = false;
	}
///////////////////////////////////////////������/////////////////////////////////////////////////
	Font font; //�����
	font.loadFromFile ("modeka.otf");

	Image buttonImage; //�������� ������� ����
	buttonImage.loadFromFile ("button.png");

	int NumButton = 0; //���������� ������
	Button *button [30]; //������ ������
	button [NumButton++] = new Button (buttonImage, "Go!", "Go!", font, (W_WIN - 120) / 2, H_WIN / 5, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Mode", "Mode", font, (W_WIN - 120) / 2, H_WIN / 5 + 50, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Setting", "Setting", font, (W_WIN - 120) / 2, H_WIN / 5 + 100, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Exit", "Exit", font, (W_WIN - 120) / 2, H_WIN / 5 + 150, 120, 30);

	button [NumButton++] = new Button (buttonImage, "Player", "PlayerMode", font, (W_WIN - 120) / 2, H_WIN / 5, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Admin", "AdminMode", font, (W_WIN - 120) / 2, H_WIN / 5 + 50, 120, 30);
	button [NumButton++] = new Button (buttonImage, "Back", "BackToMenu", font, (W_WIN - 120) / 2, H_WIN / 5 + 100, 120, 30);

	button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuAd", font, (W_WIN - 3 * 120) / 4, H_WIN - (30 + (INDENTATION - 30) / 2), 120, 30);
	button [NumButton++] = new Button (buttonImage, "Open", "OpenAd", font, (W_WIN - 3 * 120) / 2 + 120, H_WIN - (30 + (INDENTATION - 30) / 2), 120, 30);
	button [NumButton++] = new Button (buttonImage, "Save", "SaveAd", font, 3 * (W_WIN - 3 * 120) / 4 + 2 * 120, H_WIN - (30 + (INDENTATION - 30) / 2), 120, 30);

	button [NumButton++] = new Button (buttonImage, "Back", "BackToMenuPl", font, (W_WIN - 2 * 120) / 3, H_WIN - (30 + (INDENTATION - 30) / 2), 120, 30);
	button [NumButton++] = new Button (buttonImage, "Help", "HelpPl", font, 2 * (W_WIN - 2 * 120) / 3 + 120, H_WIN - (30 + (INDENTATION - 30) / 2), 120, 30);

	button [NumButton++] = new Button (buttonImage, "End lvl", "lvl1Complete", font, (W_WIN - 120) / 2, (INDENTATION - 30) / 2, 120, 30);
	
	cout << NumButton << endl;
//////////////////////////////////////////�������� �������//////////////////////////////////////////////////

	Clock clock; //�����

	while (window.isOpen ()){
		float time = clock.getElapsedTime ().asMicroseconds(); //����� ������ ��� �����������
		static float timer = 0;
		clock.restart ();
		time = time / 800;
		timer += time;

		Vector2i mousePosWin = Mouse::getPosition (window); //���������� ���� �����. ����
		Vector2f posMouse = window.mapPixelToCoords (mousePosWin); //���������� ���� �����. �����

		Event event; //��������� �������� ���� !����� ��� ����������� �������� ����
		while (window.pollEvent (event)){
			if (event.type == Event::Closed || Keyboard::isKeyPressed (Keyboard::Escape)) //������� ���� ����� � ��������
				window.close (); 
		}		
////////////////////////////////////////////���������� ����������� ����/////////////////////////////////////////////////////////////
		switch (state){
			case admin:///////////////////////////////////////////////////////
				createWalls (mousePosWin, timer, CoordWall, ArrWall, NumWall, wallImage);
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> drawThis){
						button [i] -> checkCursor (mousePosWin);
						if (button [i] -> buttClick && button [i] -> name == "SaveAd")
							saveFile (ArrWall, NumWall);
						if (button [i] -> buttClick && button [i] -> name == "OpenAd")
							openFile (ArrWall, NumWall, wallImage, CoordWall);
						if (button [i] -> buttClick && button [i] -> name == "BackToMenuAd"){
							state = menu;
							for (int i = 0; i < NumButton; i++)
								if (button [i] -> state == menu)
									button [i] -> drawThis = true;
								else
									button [i] -> drawThis = false;
						}
					}
				break;
				
			case player:////////////////////////////////////////////////////////////
				pl.update (CoordWall);
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> drawThis){
						button [i] -> checkCursor (mousePosWin);
						if (button [i] -> buttClick && button [i] -> name == "BackToMenuPl"){
							NumAnsw = 0;
							state = menu;
							for (int i = 0; i < NumButton; i++)
								if (button [i] -> state == menu)
									button [i] -> drawThis = true;
								else
									button [i] -> drawThis = false;
						}
						if (button [i] -> buttClick && button [i] -> name == "HelpPl"){
							Coordinate fn, sizeMap, st;
							sizeMap.x = (H_WIN - 2 * INDENTATION) / EDGE;
							sizeMap.y = (W_WIN - 2 * INDENTATION) / EDGE;
							st.x = pl.y / EDGE - INDENTATION / EDGE;
							st.y = pl.x / EDGE - INDENTATION / EDGE;
							fn.x = Finish.y / EDGE - INDENTATION / EDGE;
							fn.y = Finish.x / EDGE - INDENTATION / EDGE;
							//cout << "size: " << sizeMap.col << " " << sizeMap.row << endl;
							//cout << "player: " << st.col << " " << st.row << endl;
							//cout << "end: " << fn.col << " " << fn.row << endl;
							outputSearch (CoordWall, st, fn, sizeMap);

							cout << NumAnsw << endl;
							int tmp = NumAnsw;
							NumAnsw = NumAnsw / 5; 
							int j;
							cout << NumAnsw << endl;
							for (int i = 0; i < NumAnsw; i++){
								j = tmp - i;
								helpWall [i] = new Wall (wallImage, "HelpWall", Arr [j].y * EDGE + INDENTATION, Arr [j].x * EDGE + INDENTATION, EDGE, EDGE);
							}
						}
					}
					
					if (lvlComplete)
						button [NumButton - 1] -> drawThis = true;
					else
						button [NumButton - 1] -> drawThis = false;
						
				break;
				
			case menu:////////////////////////////////////////////////////////////////
				button [NumButton - 1] -> drawThis = false; 
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> drawThis){
						button [i] -> checkCursor (mousePosWin);
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
								state = admin; 
								for (int i = 0; i < NumButton; i++)
									if (button [i] -> state == admin)
										button [i] -> drawThis = true;
									else
										button [i] -> drawThis = false;
							}
							if (AdOrPlMode == "PlayerMode"){
								state = player;
								char nameFile [30];
								strcpy_s (nameFile, "lvl1.txt");
								openSpecificFile (ArrWall, NumWall, wallImage, CoordWall, nameFile);
								pl.changeCoord (Start.x, Start.y);
								for (int i = 0; i < NumButton; i++)
									if (button [i] -> state == player)
										button [i] -> drawThis = true;
									else
										button [i] -> drawThis = false;
							}
						}
						if (button [i] -> buttClick && button [i] -> name == "Exit")
							state = exitt;
					}
				break;
			case mode:///////////////////////////////////////////////////////////////////////
				for (int i = 0; i < NumButton; i++)
					if (button [i] -> drawThis){
						button [i] -> checkCursor (mousePosWin);
						if (button [i] -> buttClick && button [i] -> name == "BackToMenu"){
							state = menu;
							for (int i = 0; i < NumButton; i++)
								if (button [i] -> state == menu)
									button [i] -> drawThis = true;
								else
									button [i] -> drawThis = false;
						}
					}
				break;
			case exitt://///////////////////////////////////////////////////////////////////
				window.close ();
				break;
		}
//////////////////////////////////////���������////////////////////////////////////////////
		window.clear (Color (40, 36, 62));
		if (state == admin || state == player){
			window.draw (lines); //����� ������ �����
			for (int j = 0; j < NumWall; j++)
				if (ArrWall [j] -> drawThis)
					window.draw (ArrWall [j] -> sprite);
		}
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis)
				button [i] -> draw (window);
		if (state == player){
			window.draw (pl.sprite);
			for (int i = 0; i < NumAnsw; i++)
				window.draw (helpWall [i] -> sprite);
		}
			
		window.display ();
	}
	return 0;
}