#include <SFML/Graphics.hpp> //�������� ���������� ��� ������ � �������� � ������������
#include <SFML/Audio.hpp> //��� ������ �� ������ � �������
#include <iostream> //��� ������ � �����
#include <fstream> //��� ������ � ������� (������������ ���� ������ ���� �� ������ � �.�.)
#include "forMcText.h" //���� ���������� �� ������ kychka-pc.ru/wiki/svobodnaya-baza-znanij-sfml/test/sistemnyj-modul-system-module/potoki-threads/sfsound-sfmusic
#include "searchWay.h" //���������� ���� ������� ��� ���������� ������������ ���� � ��������� (����� � ������) ��� �������� �����
#include "view.h" //���������� �������
#include "system.h"
#include "body.h"
#include "general classes.h"
#include "quickSort.h"
#include "binSearch.h"
using namespace std;
using namespace sf;


//�������������� ��, ����� ��� � ���� ���������� �������� ���-��, �� ��� �������� � �� ���� ������� �����������
Vector2i      System::mousePosWin; //��������� ����������
Vector2f      System::posMouse;
RenderWindow* System::window;
Event         System::event;

float         System::timer; //���������� ��������� �� ��������
float         System::time;
int           System::FPS;
Clock         System::clock;

float         System::volSndClickButt; //���������� ��������� � ������� � �������
float         System::volumBackMusic;
Music         System::backgroundMusic;
SoundBuffer   System::buffer;
Sound         System::sndClickButt;

bool          System::lvlComplete; //������������ ������
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

	void checkCursor (){ //������� �������� �� ������� ������ ��� ���������� ������� �� ������
		if (name == "SelectLVL")
			if (value > PassedLVL + 1)  buttLocked = true;
			else                        buttLocked = false;
		if (name == "My lvls")
			if (PassedLVL != 8)         buttLocked = true;
			else                        buttLocked = false;

		buttClick = false;
		if (!buttLocked){
			if ((posMouse.x >= x - w / 2) && (posMouse.x <= x + w / 2) && (posMouse.y >= y - h / 2) && (posMouse.y <= y + h / 2)){ //���� ������ ���� ��������� �� ������
				//cout << "x & y - " << x << " & " << y << " w & h - " << w << " & " << h << " wT & hT - " << wTexture << " & " << hTexture << endl;
				if (Mouse::isButtonPressed (Mouse::Left)) //� ���� ������ �� ���
					buttPressed = true;
				else{
					if (buttPressed) //���� �� ������ �� ������ � ������ ���� ������, � ������ �� ������-������ �� �������� �� ���
						buttClick = true; 
					buttPressed = false;
				}
				shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture)); //���� ������� ������ �� �����, �� ������ ������ ��������
				if (name == "SelectLVL")
					shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture));
			}
			else{ //���� ������ �� �� ������
				buttPressed = false; //���� ������ �� �� ���� �� ������ �������� ����
				shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
				if (name == "SelectLVL")
					shape.setTextureRect (IntRect (0, 4 * hTexture, wTexture, hTexture));
			}
		}
		else{ //���� ������ �������������
			shape.setTextureRect (IntRect (0, hTexture * 3, wTexture, hTexture));
			if (name == "SelectLVL")
				shape.setTextureRect (IntRect (3 * wTexture, 4 * hTexture, wTexture, hTexture));
		}

		if (name == AdOrPlMode) //���� ��� ���������� ������� ������ ��� ������ ������� � �������, �� ������ ������� (�������������� ����������)
			shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
	}

	void updateText (char *Pass){ }
};

class EditButton : public BodyButton{
public:
	EditButton (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    BodyButton (image, Text, Name, Font, State, X, Y, W, H, WTexture, HTexture){
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
			if (name == "ChangeKey")
				shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture));
		}
		else{
			buttPressed = false; //���� ������ �� �� ���� �� ������ �������� ����
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
			if (name == "ChangeKey")
				shape.setTextureRect (IntRect (0, 4 * hTexture, wTexture, hTexture));
		}
		
		if (name == "ChangeKey" && whatButChange == value)
			shape.setTextureRect (IntRect (2 * wTexture, 4 * hTexture, wTexture, hTexture));
	}

	void updateText (char *Pass){ //������� ���������� ������
		buttText = Pass;
		text -> clear ();
		text -> changeSize (SIZE_TEXT); //������ ������
		text -> add (buttText);
		float tmp = (float) buttText.getSize (); //�������� ������ ������ � ��������
		tmp /= 2;
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //����������� ����� �� ������
	}
};

class Static : public BodyButton{
public:
	Static (String Text, String Name, Font &Font, StateList &State, int X, int Y, Color ccolor) : //���������� ��� ������� ����� ������
		    BodyButton (Text, Name, Font, State, X, Y){ 
		text -> clear ();
		text -> changeSize (SIZE_TEXT); //������ ������
		color = ccolor;
		text -> add (buttText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //����������� ����� �� ������
	}

	Static (String Text, String Name, Font &Font, StateList &State, int X, int Y) : //�� ��������� ���� ������ ����� ������
		    BodyButton (Text, Name, Font, State, X, Y){ }

	void draw (){
		text -> draw (window);
	}

	void checkCursor (){ }

	void updateText (char *Pass){
		text -> clear ();
		text -> changeSize (SIZE_TEXT); //������ ������
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
		else if (name == "DeathPlayer"){
			strcpy (tmpC, "Death: ");
			strcat (tmpC, Pass);
			text -> add (tmpC, Color (211, 25, 12));
		}
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //����������� ����� �� ������
	}
};

class HorizontScrollBar : public BodyButton{
private:
	int leftBorder, rightBorder; //����� � ������ ������� �� ������� ����� ������������ ������ (�� ��� ��������� �������, �.�. ������ ����� ������)
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
		window -> draw (backgroundd);
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
			shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture)); //���� ������� ������ �� �����, �� ������ ������ ��������
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

			reducePrecent -= speedChangeSt * time;
			if (reducePrecent < speedChangeSt * time){
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

			enlargePrecent += speedChangeSt * time;
			if (enlargePrecent > 100 - speedChangeSt * time){
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
private:
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
			shape.setTextureRect (IntRect (wTexture, 4 * hTexture, wTexture, hTexture));
		}
		else{
			buttPressed = false; //���� ������ �� �� ���� �� ������ �������� ����
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

			reducePrecent -= speedChangeSt * time;
			if (reducePrecent < speedChangeSt * time){
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

			enlargePrecent += speedChangeSt * time;
			if (enlargePrecent > 100 - speedChangeSt * time){
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


class Game : public System{ //��� �������� � ������������� ���� � ���� ������
public:
	StateList state; //��������� ����
	StateList whichStateWas; //����� ��������� ����, ����� ��� ��������� ���������
	StateList whichStateWill; //����� ��������� ������ �����, ����� ��� ��������� ���������

	float lvlTime; //���� �����
	float AllTime; //����� ����� ������ ������������ � ����
	int lvlDeath; //����������� ������� �� ������
	int CurrentLVL; //������� �������
	int NumWall; //���������� ����
	int NumBorderWall; //���������� ��������� ����
	int NumButton; //���������� ������
	int NumListLVL; //���������� ������� � ������ (��� � ����� ����� ������� ������� ��������� ������)

	char Pass [30]; //������
	char myLVLname [50];  //��� ����� ��������� �������, � ������� ��� ���� ��������� �������
	char fileNameAd [50]; //��� ����� ��������� �������
	char fileNamePl [70]; //��� ����� ��������� �������

	bool escapeReleased; //���� ����� 1 ���� ������ ��������� (�� ��� ������, � ����� ���������)
	bool enterReleased; //���� ����� 1 ���� Enter ��������� (�� ��� ������, � ����� ���������)
	bool anyKeyReleased; //���� ����� 1 ���� Enter ��������� (�� ��� ������, � ����� ���������)
	bool playerLVL; //����� ������ � ���� ��������� �������?
	bool changeStates; //���� ������� ����������, �������� �� ��������� � ������ ������
	bool changeKey; //���� ������� ����������, �������� �� ������ ����� ������� (����� ������ ������� �� ������� �������� ������)
	bool secondPhase; //���� ������� ����������, ������ �� ������ ���� ��������� ��������� (����������)
	bool inSetIntoPause; //���� ������� ����������, ����� �� �� � ��������� ����� ������ (����� ��� � ����� �������� �� �������� ������������ � ����)
	
	int indexFinish; //������ ������ (��� � ����� �� ������)
	int indexStart; //������ ������ (��� � ����� �� ������)
	int indexDeathPlBut; //������ ������ �� ������� ��������� ���������� ������� �� ������
	int indexFPSBut; //������ ������ �� ������� ��������� �������� ���
	int indexTimePlBut; //������ ������ �� ������� ��������� ����� ������

	Image wallImage; //����������� ����
	Image wallImagePL; //����������� ������
	VertexArray lines; //����� ������� � ����� ���� ��������, ��� � ����� ���� ��������� ������
	Sprite cursor; //������
	Texture textureCursor; //�������� ������� 
	
	Player *pl; //�����
	mcText *timePlText; //�����, � ������ �������� ����� ������� ������ �����
	BodyButton *button [100]; //������ ������
	Wall *ArrWall [4000]; //������ ����
	bool **CoordWall; //���������� ����
	Wall *BorderWall [250]; //������ ���� ������� ����� �������� ��� ������ (����� ��� �������)
	Background *plBackground; //������� �����������, ������, ������� ��������� ��������
	Background *logo; //�������
public:
	void readInfo (){ //������� ���������� �� ������
		ifstream inF ("Resources/Info/Player.txt");
		inF >> PassedLVL >> volumBackMusic >> volSndClickButt >> AllTime >> key [0] >> key [1] >> key [2]; //��������� 3-������ ������ �� ������� �������� ������ ������
	}

	void writeInfo (){ //�������� ����������� �� ������
		ofstream outF ("Resources/Info/Player.txt");
		outF << PassedLVL << " " << volumBackMusic << " " << volSndClickButt  << " " << AllTime;
		outF << " " << key [0] << " " << key [1] << " " << key [2] << endl; //��������� 3-������ ������ �� ������� �������� ������ ������
	}

	void draw (){ //������� � ������������ ������� ���������
		window -> setView (view); //��������� ������
		window -> clear (Color (40, 36, 62));

		if (state == admin || state == AdSelectLVL || state == AdSaveLVL || state == AdDeleteLVL){
			window -> draw (lines); //����� ��������������� ����� ��� ������
			for (int i = 0; i < NumWall; i++) //����� �����
					ArrWall [i] -> draw ();
		}

		if (state == player || state == pause || state == startLVL){
			int tmpX, tmpY;
			float tmpX2, tmpY2;
			tmpX = (int) pl -> x; tmpX -= GLOB_IND_W;
			tmpY = (int) pl -> y; tmpY -= GLOB_IND_H;
			for (int i = 0; i < NumWall; i++){ //����� ����� ������� ����� ����� ������, � ������� �� � �����		
				if (abs (ArrWall [i] -> x * EDGE - tmpX) < EDGE * (NUM_SQUARE + 1) / 2)
					if ( abs (ArrWall [i] -> y * EDGE - tmpY) < EDGE * (NUM_SQUARE + 1) / 2){
					tmpX2 = (float) ArrWall [i] -> x * EDGE - tmpX;
					tmpY2 = (float) ArrWall [i] -> y * EDGE - tmpY;
					tmpX2 = tmpX2 * ((float) SQUARE / (float) EDGE);
					tmpY2 = tmpY2 * ((float) SQUARE / (float) EDGE);
					ArrWall [i] -> changeCoord (GLOBAL_W / 2 + (int) tmpX2 - SQUARE / 2, GLOBAL_H / 2 + (int) tmpY2 - SQUARE / 2);
					ArrWall [i] -> draw ();
				}
			}
			for (int i = 0; i < NumBorderWall; i++) //����� ��������� ����� ������� ����� ����� ������, � ������� �� � �����
				if (abs (BorderWall [i] -> x * EDGE - tmpX) < EDGE * (NUM_SQUARE + 1) / 2)
					if (abs (BorderWall [i] -> y * EDGE - tmpY) < EDGE * (NUM_SQUARE + 1) / 2){
					tmpX2 = (float) BorderWall [i] -> x * EDGE - tmpX;
					tmpY2 = (float) BorderWall [i] -> y * EDGE - tmpY;
					tmpX2 = tmpX2 * ((float) SQUARE / (float) EDGE);
					tmpY2 = tmpY2 * ((float) SQUARE / (float) EDGE);
					BorderWall [i] -> changeCoord (GLOBAL_W / 2 + (int) tmpX2 - SQUARE / 2, GLOBAL_H / 2 + (int) tmpY2 - SQUARE / 2);
					BorderWall [i] -> draw ();
				}
			
			plBackground -> draw (); //����� ���, ������� ��������� ������� ���� ������� ���������
			pl -> draw (); 
			button [indexTimePlBut] -> draw (); //����� ������ ��� ������������ ����� (�� �������� ���������� ��� ������� if)
			button [indexDeathPlBut] -> draw (); //����� ������ ��� ������������ ���������� ������� �� ������
		}
		else if (state != admin && state != AdSelectLVL && state != AdSaveLVL && state!= completeLVL && state != AdDeleteLVL && state != AdListLVL) //������ ������� � ����������� ���������
			logo -> draw ();

		for (int i = 0; i < NumButton; i++) //����� ������
			if (button [i] -> drawThis || button [i] -> state == allState)
				button [i] -> draw ();

		cursor.setPosition (posMouse.x, posMouse.y); //����� ������ ������� ��� ������ ���� ������
		window -> draw (cursor);

		window -> display ();
	}

	void initializeBackground (){
		Image backgroundImage;
		backgroundImage.loadFromFile ("Resources/Textures/background.png"); //��� ������� ��������� ������� ���� ����� ������ �����
		plBackground = new Background (backgroundImage, "PlayerBackground", 0, 0, 1000 * (NUM_SQUARE) * SQUARE / 500, 1000 * (NUM_SQUARE) * SQUARE / 500, 1000, 1000); //�� ����� ����� ��������� 2 ���������
		plBackground -> changeCoord (GLOBAL_W / 2, GLOBAL_H / 2);

		backgroundImage.loadFromFile ("Resources/Textures/logo2.png"); //�������
		logo = new Background (backgroundImage, "Logo", 0, 0, W_BUTTON * 2, H_BUTTON * 2, 392, 91); 
		logo -> changeCoord (GLOBAL_W / 2, GLOBAL_H / 2 - 7 * (H_BUTTON + 6)); 
	}

	void initializeButton (){
		Font font;
		font.loadFromFile ("Resources/Fonts/modeka.otf");

		Image buttonImage; //�������� ������� ����
		buttonImage.loadFromFile ("Resources/Textures/button.png");

		StateList tmpS;
		int tmpI;

		NumButton = 0;

		tmpS = allState;
		indexFPSBut = NumButton;
		button [NumButton++] = new Static ("FPS: 0", "FPS", font, tmpS, GLOBAL_W / 2 - W_WIN / 2 + W_BUTTON / 2, GLOBAL_H / 2 + H_WIN / 2 - H_BUTTON / 2);

		tmpS = menu;
		button [NumButton++] = new Button (buttonImage, "Go!",     "Go!",      font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Mode",    "Mode",     font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Setting", "Settings", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Exit",    "Exit",     font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 0 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = exitt;
		button [NumButton++] = new Static (             "Quit game?", "Quit game?", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6));
		button [NumButton++] = new Button (buttonImage, "No!",        "QuitNo",     font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 0 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Yes.",       "QuitYes",    font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = settings;
		button [NumButton++] = new Button (buttonImage, "Controls", "ControlsSet",   font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Audio",    "AudioSet",      font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Back",     "BackToMenuSet", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 0 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = controlsSet;
		button [NumButton++] = new Static (                 "Rectangle:", "Rectangle",        font, tmpS, GLOBAL_W / 2 - W_BUTTON / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6));
		button [NumButton++] = new Static (                 "Triangle:",  "Triangle",         font, tmpS, GLOBAL_W / 2 - W_BUTTON / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6));
		button [NumButton++] = new Static (                 "Circle:",    "Circle",           font, tmpS, GLOBAL_W / 2 - W_BUTTON / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6));
		button [NumButton++] = new EditButton (buttonImage, "1",          "ChangeKey",        font, tmpS, GLOBAL_W / 2 + W_BUTTON / 4, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), W_BUTTON / 4, H_BUTTON, 47,  45);
		button [NumButton++] = new EditButton (buttonImage, "2",          "ChangeKey",        font, tmpS, GLOBAL_W / 2 + W_BUTTON / 4, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON / 4, H_BUTTON, 47,  45);
		button [NumButton++] = new EditButton (buttonImage, "3",          "ChangeKey",        font, tmpS, GLOBAL_W / 2 + W_BUTTON / 4, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON / 4, H_BUTTON, 47,  45);
		button [NumButton++] = new Button (buttonImage, "Back",           "BackToControlSet", font, tmpS, GLOBAL_W / 2,                GLOBAL_H / 2 - 0 * (H_BUTTON + 6), W_BUTTON,     H_BUTTON, 0, 188, 45);
		
		tmpS = audioSet;
		button [NumButton++] = new Static (             "Sound:",  "VolSound",       font, tmpS, GLOBAL_W / 2 - W_BUTTON / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6));
		button [NumButton++] = new Static (             "Music:",  "VolMusic",       font, tmpS, GLOBAL_W / 2 - W_BUTTON / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6));
		button [NumButton++] = new Button (buttonImage, "Back",    "BackToSetAudio", font, tmpS, GLOBAL_W / 2,                GLOBAL_H / 2 - 0 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new HorizontScrollBar (buttonImage, "SoundSlider",    font, tmpS, GLOBAL_W / 2 + W_BUTTON / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), 20, H_BUTTON, GLOBAL_W / 2 + 10, GLOBAL_W / 2 + W_BUTTON - 10, 47, 45, 188, 45);
		button [NumButton++] = new HorizontScrollBar (buttonImage, "MusicSlider",    font, tmpS, GLOBAL_W / 2 + W_BUTTON / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), 20, H_BUTTON, GLOBAL_W / 2 + 10, GLOBAL_W / 2 + W_BUTTON - 10, 47, 45, 188, 45);

		tmpS = mode;
		button [NumButton++] = new Button (buttonImage, "Player", "PlayerMode", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Admin",  "AdminMode",  font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Back",   "BackToMenu", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);

		Image pictureImage; //�������� ������� ����
		tmpS = admin;
		tmpI = GLOBAL_H / 2 + (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new Button (buttonImage, "Back",     "BackToMenuAd", font, tmpS, GLOBAL_W / 2 - 2 * W_BUTTON - 6 * EDGE, tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Open",     "OpenAd",       font, tmpS, GLOBAL_W / 2 - 1 * W_BUTTON - 3 * EDGE, tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Save",     "SaveAd",       font, tmpS, GLOBAL_W / 2 + 0 * W_BUTTON + 0 * EDGE, tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Delete",   "DeleteAd",     font, tmpS, GLOBAL_W / 2 + 1 * W_BUTTON + 3 * EDGE, tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "List lvl", "ListAd",       font, tmpS, GLOBAL_W / 2 + 2 * W_BUTTON + 6 * EDGE, tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);
		pictureImage.loadFromFile ("Resources/Textures/button2.png"); 
		tmpI = GLOBAL_H / 2 - (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new PictureButton (buttonImage, "Rectangle", font, tmpS, GLOBAL_W / 2 - 9 - H_BUTTON * 3, tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Triangle",  font, tmpS, GLOBAL_W / 2 - 6 - H_BUTTON * 2, tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Circle",    font, tmpS, GLOBAL_W / 2 - 3 - H_BUTTON,     tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Wall",      font, tmpS, GLOBAL_W / 2,                    tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Start",     font, tmpS, GLOBAL_W / 2 + 3 + H_BUTTON,     tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30); 
		button [NumButton++] = new PictureButton (buttonImage, "Finish",    font, tmpS, GLOBAL_W / 2 + 6 + H_BUTTON * 2, tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);
		button [NumButton++] = new PictureButton (buttonImage, "Save",      font, tmpS, GLOBAL_W / 2 + 9 + H_BUTTON * 3, tmpI, H_BUTTON, H_BUTTON, 47, 45, pictureImage, 30, 30);

		tmpS = player;
		tmpI = NUM_SQUARE * SQUARE / 2 + (W_WIN - NUM_SQUARE * SQUARE) / 4;
		button [NumButton++] = new Button (buttonImage, "Pause",    "BackToMenuPl", font, tmpS, GLOBAL_W / 2 + tmpI, GLOBAL_H / 2 - 0 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Static (             "Time: 0",  "TimePlayer",   font, tmpS, GLOBAL_W / 2 - tmpI, GLOBAL_H / 2 - 7 * (H_BUTTON + 6), Color (211, 25, 12));
		indexTimePlBut = NumButton - 1;
		button [NumButton++] = new Static (             "Death: 0", "DeathPlayer",  font, tmpS, GLOBAL_W / 2 - tmpI, GLOBAL_H / 2 - 6 * (H_BUTTON + 6), Color (211, 25, 12));
		indexDeathPlBut = NumButton - 1;

		tmpS = pause;
		tmpI = GLOBAL_W / 2 + NUM_SQUARE * SQUARE / 2 + (W_WIN - NUM_SQUARE * SQUARE) / 4;
		button [NumButton++] = new Static (             "Pause",    "Pause",             font, tmpS, tmpI, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), Color (193, 180, 180));
		button [NumButton++] = new Button (buttonImage, "Leave",    "LeaveToSel",        font, tmpS, tmpI, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Settings", "SettingsIntoPause", font, tmpS, tmpI, GLOBAL_H / 2 - 0 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Continue", "BackToPlPause",     font, tmpS, tmpI, GLOBAL_H / 2 + 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = myLVLs;
		button [NumButton++] = new EditButton (buttonImage, "",                    "InputMyLVL",      font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 2 * (H_BUTTON + 6), W_BUTTON, H_BUTTON,    188, 45);
		button [NumButton++] = new Static (                 "Enter name your LVL", "StaticMyLVL",     font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 3 * (H_BUTTON + 6));
		button [NumButton++] = new Button (buttonImage,     "Back",                "BackToMenuMyLVL", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = selectLVL;
		tmpI = GLOBAL_H / 2 - 3 * (H_BUTTON + 6);
		button [NumButton++] = new Button (buttonImage, "1",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 - 3 * (W_BUTTON) / 8, tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 1, 47,  45);
		button [NumButton++] = new Button (buttonImage, "2",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 - (W_BUTTON) / 8,     tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 2, 47,  45);
		button [NumButton++] = new Button (buttonImage, "3",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 + (W_BUTTON) / 8,     tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 3, 47,  45);
		button [NumButton++] = new Button (buttonImage, "4",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 + 3 * (W_BUTTON) / 8, tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 4, 47,  45);
		tmpI = GLOBAL_H / 2 - 2 * (H_BUTTON + 6);
		button [NumButton++] = new Button (buttonImage, "5",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 - 3 * (W_BUTTON) / 8, tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 5, 47,  45);
		button [NumButton++] = new Button (buttonImage, "6",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 - (W_BUTTON) / 8,     tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 6, 47,  45);
		button [NumButton++] = new Button (buttonImage, "7",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 + (W_BUTTON) / 8,     tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 7, 47,  45);
		button [NumButton++] = new Button (buttonImage, "8",          "SelectLVL",     font, tmpS, GLOBAL_W / 2 + 3 * (W_BUTTON) / 8, tmpI,         (W_BUTTON - 4) / 4, H_BUTTON, 8, 47,  45);
		button [NumButton++] = new Button (buttonImage, "My lvls",    "My lvls",       font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), W_BUTTON,           H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Button (buttonImage, "Back",       "BackToMenuSel", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 0 * (H_BUTTON + 6), W_BUTTON,           H_BUTTON, 0, 188, 45);
		button [NumButton++] = new Static (             "Select LVL", "SelectStatic",  font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - 4 * (H_BUTTON + 6));

		tmpS = AdSelectLVL;
		tmpI = (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new EditButton (buttonImage, "", "EditLVL",        font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - tmpI, W_BUTTON, H_BUTTON,    188, 45);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToAdminSel", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = AdSaveLVL;
		tmpI = (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new EditButton (buttonImage, "", "AdSaveLVL",       font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - tmpI, W_BUTTON, H_BUTTON,    188, 45);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToAdminSave", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = AdDeleteLVL;
		tmpI = (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new EditButton (buttonImage, "", "AdDeleteLVL",       font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 - tmpI, W_BUTTON, H_BUTTON,    188, 45);
		button [NumButton++] = new Button (buttonImage, "Back", "BackToAdminDelete", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = AdListLVL;
		tmpI = (H_WIN + NUM_CELL_Y * EDGE) / 4;
		button [NumButton++] = new Button (buttonImage, "Back", "BackToAdminList", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + tmpI, W_BUTTON, H_BUTTON, 0, 188, 45);

		tmpS = startLVL;
		tmpI = GLOBAL_W / 2 + NUM_SQUARE * SQUARE / 2 + (W_WIN - NUM_SQUARE * SQUARE) / 4;
		button [NumButton++] = new Static ("Press Escape to leave",  "StartLVL", font, tmpS, tmpI, GLOBAL_H / 2 - 1 * (H_BUTTON + 6), Color (193, 180, 180));
		button [NumButton++] = new Static ("Press any key to start", "StartLVL", font, tmpS, tmpI, GLOBAL_H / 2 - 0 * (H_BUTTON + 6), Color (193, 180, 180));
	}

	void initializeLine (){
		
		lines = VertexArray (Lines, (NUM_CELL_Y + NUM_CELL_X + 4) * 2); //������ �����
		int i = 0; //i-������� ����� ���������� � ������
		for (int k = 0; k < (NUM_CELL_Y + NUM_CELL_X + 2) * 2; k++)
			lines [k].color = Color (128, 128, 128); //30, 30, 30//79,75,5
		for (; i < (NUM_CELL_X + 1) * 2; i += 2){ //�������� ������������ �����
			lines [i].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + i * EDGE / 2, (float) GLOB_IND_H + NUM_CELL_Y * EDGE);
		}
		for (int k = 0; i < (NUM_CELL_Y + NUM_CELL_X + 2) * 2; i += 2, k += 2){ //�������� �������������� �����
			lines [i].position = Vector2f ((float) GLOB_IND_W, (float) EDGE * k / 2 + GLOB_IND_H);
			lines [i + 1].position = Vector2f ((float) GLOB_IND_W + NUM_CELL_X * EDGE, (float) EDGE * k / 2 + GLOB_IND_H);
		}
	}

	void initialize (){
		Start.x = GLOB_IND_H; Start.y = GLOB_IND_H + NUM_CELL_Y * EDGE; //�������������� ��������� �����
		Finish.x = GLOB_IND_W + NUM_CELL_X * EDGE; Finish.y = GLOB_IND_H; //�������������� �����

		Image playerImage; //������� ������� ������
		playerImage.loadFromFile ("Resources/Textures/player2.png");
		pl  = new Player (playerImage, Start.x, Start.y, SQUARE, SQUARE, 40, 40); //�������� ������� �����

		backgroundMusic.openFromFile ("Resources/Music/DJVI_-_Dry_Out.ogg"); //������
		backgroundMusic.play (); 
		backgroundMusic.setLoop (true);
		backgroundMusic.setVolume (volumBackMusic);

		buffer.loadFromFile ("Resources/Sounds/button-30.wav"); //����
		sndClickButt.setBuffer (buffer);
		sndClickButt.setVolume (volSndClickButt);

		Image tmpI; //������ �� �������� �������
		tmpI.loadFromFile ("Resources/Textures/cursor.png");
		textureCursor.loadFromImage (tmpI);
		cursor.setTexture (textureCursor);
		cursor.setTextureRect (IntRect (0, 0, 23, 27));
	}

	void initializeWall (){
		wallImage.loadFromFile ("Resources/Textures/wall.png");
		wallImagePL.loadFromFile ("Resources/Textures/wall2.png");
	
		NumWall = 0; //���������� ����
		CoordWall = new bool* [NUM_CELL_X];
		for (int i = 0; i < NUM_CELL_X; i++){
			CoordWall [i] = new bool [NUM_CELL_Y];
			for (int j = 0; j < NUM_CELL_Y; j++)
				CoordWall [i][j] = false;
		}
		NumBorderWall = 0; //�������� �������� ���� ��� ������ (��� � ���� �������)
		for (int i = -1; i < 65; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", i, -1, SQUARE, SQUARE, 40, 40);
		for (int i = -1; i < 65; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", i, 32, SQUARE, SQUARE, 40, 40);
		for (int i = 0; i < 32; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", -1, i, SQUARE, SQUARE, 40, 40);
		for (int i = 0; i < 32; i++)
			BorderWall [NumBorderWall++] = new Wall (wallImagePL, "Wall", 64, i, SQUARE, SQUARE, 40, 40);

	}

	Game (){ //����������� � ������� �������������� �������� ���������
		AdOrPlMode = "PlayerMode"; //������ �������� ��� �������� ���� ���� (����� ��� �����)
		strcpy (Pass, "");
		strcpy (fileNameAd, "");
		strcpy (myLVLname, "");
		lvlComplete = false;
		playerLVL = false;
		escapeReleased = false;
		changeStates = false; 
		secondPhase = false;
		inSetIntoPause = false;
		changeKey = false;
		state = menu;
		whichWall = wall;
		key [0] = 27;
		key [1] = 28;
		key [2] = 29;
		CurrentLVL = 1;
		timer = 0;
		NumWall = 0;
		lvlDeath = 0;
		whatButChange = 0;
		indexFinish = -1;

		readInfo ();
		initialize (); //�������� ��������� �������������
		initializeButton ();
		initializeLine ();
		initializeWall ();
		initializeBackground ();
	}


	void createWalls (){ //�������� ���� � ����� ����
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
				if (timer > 0.5){ //����� ����� ������� ��� � 0.5 ���
					timer = 0;	
					tmpX = (int) posMouse.x; tmpX -= GLOB_IND_W; tmp = tmpX % EDGE; tmpX -= tmp; tmpX /= EDGE; //�������� ���������� ���� � �������
					tmpY = (int) posMouse.y; tmpY -= GLOB_IND_H; tmp = tmpY % EDGE; tmpY -= tmp; tmpY /= EDGE;
					for (int i = 0; i < NumWall; i++){
						if (ArrWall [i] -> x == tmpX && ArrWall [i] -> y == tmpY)
							if (ArrWall [i] -> name != "Start" && ArrWall [i] -> name != "Finish"){ //������ ����� ���� ��� �� ����� ��� �����
								deleted = true;
								if (ArrWall [i] -> name == "Wall")             wallDeleted = true; //��������� ����� ������ ����� � ������ (����������� � ����������)
								else if (ArrWall [i] -> name == "Circle")      circleDeleted = true;
								else if (ArrWall [i] -> name == "Rectangle")   rectangleDeleted = true;
								else if (ArrWall [i] -> name == "Triangle")    triangleDeleted = true;
								else if (ArrWall [i] -> name == "Save")        saveDeleted = true;
								if (i != NumWall - 1) //����� ��������� ����� �� �� ������� ���� �������
									*ArrWall [i] = *ArrWall [NumWall - 1];
								if (ArrWall [NumWall - 1] -> name == "Start") //����� ������ ������ � ������ (���� ����� ��������� ������ �������� ����� ��� �����
									indexStart = i;
								else if (ArrWall [NumWall - 1] -> name == "Finish")
									indexFinish = i;
								CoordWall [tmpX][tmpY] = false;
								delete ArrWall [NumWall - 1]; //������ ��������� ����� (�� ���� ����������� ����� ������� ���� �������)
								NumWall--;
								break;
							}
					}

					if (whichWall == startW){
						if (tmpX != ArrWall [indexFinish] -> x || tmpY != ArrWall [indexFinish] -> y){ //���� �� ����� ��������� ����� �� �� �����
							*ArrWall [indexStart] = *ArrWall [NumWall - 1]; //������� ������ ����� �� ��������� �����

							if (ArrWall [NumWall - 1] -> name == "Finish") //���� ����� �� ��������� ����� ��� �����
								indexFinish = indexStart;

							delete ArrWall [NumWall - 1]; //������ ������ �����
							CoordWall [tmpX][tmpY] = false;
							NumWall--;

							indexStart = NumWall; //����� ������ � ������ �����
							ArrWall [NumWall++] = new Wall (wallImage, "Start", tmpX, tmpY, EDGE, EDGE, 20, 20);
							Start.x = tmpX; Start.y = tmpY;
						}
					}
					else if (whichWall == finishW){ //���������� ��� �� �������
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
					else{ //���� �� ����� ��������� �� �� ����� � �����
						if ((tmpX == ArrWall [indexStart] -> x && tmpY == ArrWall [indexStart] -> y) || (tmpX == ArrWall [indexFinish] -> x && tmpY == ArrWall [indexFinish] -> y))
							tmp = 0;
						else {
							if ((whichWall == wall) && !wallDeleted){
								ArrWall [NumWall++] = new Wall (wallImage, "Wall", tmpX, tmpY, EDGE, EDGE, 20, 20);
								CoordWall [tmpX][tmpY] = true; //���� ��� �����, �� ����� ��������� ������ ����
							}
							else if ((whichWall == rectangleW) && !rectangleDeleted)   ArrWall [NumWall++] = new Wall (wallImage, "Rectangle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							else if ((whichWall == triangleW) && !triangleDeleted)     ArrWall [NumWall++] = new Wall (wallImage, "Triangle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							else if ((whichWall == circleW) && !circleDeleted)         ArrWall [NumWall++] = new Wall (wallImage, "Circle",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
							else if ((whichWall == saveW) && !saveDeleted)             ArrWall [NumWall++] = new Wall (wallImage, "Save",  tmpX,  tmpY, EDGE, EDGE, 20, 20);
						}
					}
				}	
	}

	void saveLVL (char *tmpC){ //���������� ������ �������
		Coordinate tmp1, tmp2; //��������� ���������� ������ � ������
		tmp1.x = ArrWall [indexStart] -> x;
		tmp1.y = ArrWall [indexStart] -> y;
		tmp2.x = ArrWall [indexFinish] -> x;
		tmp2.y = ArrWall [indexFinish] -> y;
		quickSort (0, NumWall - 1, ArrWall); //���������
		indexStart = binSearch (0, NumWall - 1, ArrWall, tmp1); //����� ���������� ������� ������� ������ � ������
		indexFinish = binSearch (0, NumWall - 1, ArrWall, tmp2);

		ofstream outF (tmpC);
		int tmp = 0;
		outF << NumWall << endl;
		outF << (Start.x - GLOB_IND_W) / EDGE << " " << (Start.y - GLOB_IND_H) / EDGE << endl; //���������� ������, ���� ���������
		outF << lvlDeath << endl;
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
		inF >> lvlDeath;
		Start.x = Start.x * EDGE + GLOB_IND_W; //���������� ������, ���� ���������
		Start.y = Start.y * EDGE + GLOB_IND_H;

		for (int i = 0; i < NumWall; i++){
			inF >> tmpX >> tmpY >> tmpC;
			ArrWall [i] = new Wall (wallImagePL, tmpC, tmpX, tmpY, SQUARE, SQUARE, 40, 40);

			if (strcmp (tmpC, "Wall") == 0)
				CoordWall [tmpX][tmpY] = true;
			else if (strcmp (tmpC, "Start") == 0)
				indexStart = i;
			else if (strcmp (tmpC, "Finish") == 0){
				Finish.x = tmpX * EDGE + GLOB_IND_W; 
				Finish.y = tmpY * EDGE + GLOB_IND_H; indexFinish = i;
			}
		}

		Coordinate tmp1, tmp2; //��������� ���������� ������ � ������
		tmp1.x = ArrWall [indexStart] -> x;
		tmp1.y = ArrWall [indexStart] -> y;
		tmp2.x = ArrWall [indexFinish] -> x;
		tmp2.y = ArrWall [indexFinish] -> y;
		quickSort (0, NumWall - 1, ArrWall); //���������
		indexStart = binSearch (0, NumWall - 1, ArrWall, tmp1); //����� ���������� ������� ������� ������ � ������
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
				CoordWall [i][j] = false;
		NumAnsw = 0;

		ifstream inF (tmpC);
		inF >> NumWall; 
		inF >> Start.x >> Start.y;
		Start.x = Start.x * EDGE + GLOB_IND_W; //���������� ������, ���� ���������
		Start.y = Start.y * EDGE + GLOB_IND_H;
		inF >> lvlDeath;

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

		Coordinate tmp1, tmp2; //��������� ���������� ������ � ������
		tmp1.x = ArrWall [indexStart] -> x;
		tmp1.y = ArrWall [indexStart] -> y;
		tmp2.x = ArrWall [indexFinish] -> x;
		tmp2.y = ArrWall [indexFinish] -> y;
		quickSort (0, NumWall - 1, ArrWall); //���������
		indexStart = binSearch (0, NumWall - 1, ArrWall, tmp1); //����� ���������� ������� ������� ������ � ������
		indexFinish = binSearch (0, NumWall - 1, ArrWall, tmp2);
	}

	void inputKeyboard (char *tmpC, bool fictiv){ //���� � ����������
		if (event.type == Event::KeyPressed){
			if (Keyboard::isKeyPressed (Keyboard::BackSpace)){ //���� ������ ������ �������     
				int i = strlen (tmpC);
				if (i > 0){
					char tmpC2 [50];
					strcpy (tmpC2, tmpC);
					strncpy (tmpC, tmpC2, i - 1);
					tmpC [i - 1] = '\0';
				}
			}
			else if ((strlen (tmpC) < 8 && !fictiv) || (strlen (tmpC) < 4 && fictiv)){
				char tmpC2 [2];
				if (event.key.code >= 0 && event.key.code <= 25){ //����� �� � �� Z
					tmpC2 [0] = event.key.code + 97;
					tmpC2 [1] = '\0';
					strcat (tmpC, tmpC2);
				}
				else if (event.key.code >= 26 && event.key.code <= 35){ //����� �� 0 �� 9
					tmpC2 [0] = event.key.code + 22;
					tmpC2 [1] = '\0';
					strcat (tmpC, tmpC2);
				}
			}
		}
			
	}

	void createWay (){ //������� �������� ������ �� ���������
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

	void changeState2 (){ //������� ��������� ���������
		if (!secondPhase){ //������ ���� (���������� ������)
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> state == whichStateWas){
					if (button [i] -> changeForm == false){ //������a�� ���a ������� �� ��������� ���� ������
						secondPhase = true; state = whichStateWill;
						for (int i = 0; i < NumButton; i++)
							if (button [i] -> state == whichStateWill){
								button [i] -> drawThis  = true;
								button [i] -> clearButton ();
							}
						else
							button [i] -> drawThis  = false;

						if (whichStateWas == AdListLVL){
							for (int k = NumButton - 1; k > NumButton - NumListLVL - 1; k--)
								delete button [k];
							NumButton -= NumListLVL;
							NumListLVL = 0;
						}

						break;
					}
					button [i] -> reduceButton ();
				}
		}
		else{ //����� ���� (���������� ������)
			for (int i = 0; i < NumButton; i++)
				if (button [i] -> state == whichStateWill){
					button [i] -> enlargeButton (); 
					if (button [i] -> changeForm == false){ //����������� ���� ���� ������� �� ���������
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

	void changeState (StateList tmpS){ //��������������� ������� ��� ��������� ���������
		sndClickButt.play ();
		changeStates = true; 
		whichStateWas = state; whichStateWill = tmpS;
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
				else if (button [i] -> buttClick && button [i] -> name == "AdminMode" && !changeStates){
					 sndClickButt.play ();
					 if (PassedLVL == 8)
						 AdOrPlMode = "AdminMode"; 
					 break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "PlayerMode" && !changeStates){
					sndClickButt.play (); AdOrPlMode = "PlayerMode"; break;
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
				else if (button [i] -> buttClick && button [i] -> name == "DeleteAd" && !changeStates){
					changeState (AdDeleteLVL); break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "ListAd" && !changeStates){
					Font font;
					font.loadFromFile ("Resources/Fonts/modeka.otf");
					ifstream inF ("Resources/LVLs/listLVLs.txt");
					char tmpC [50];
					int tmpI;
					StateList tmpS = AdListLVL;
					inF >> tmpI;
					for (int i = 0; i < tmpI; i++){
						inF >> tmpC;
						if (strstr (tmpC, "lvl") == NULL || strcspn (tmpC, "1234") == NULL){
							button [NumButton++] = new Static (tmpC, "ListLVL", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + NUM_CELL_Y * EDGE / 2 - H_BUTTON * (NumListLVL + 1));
							NumListLVL++;
						}
					}
					button [NumButton++] = new Static ("List of LVLs:", "ListLVL", font, tmpS, GLOBAL_W / 2, GLOBAL_H / 2 + NUM_CELL_Y * EDGE / 2 - H_BUTTON * (NumListLVL + 1));
					NumListLVL++;

					changeState (AdListLVL); 

					break;
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
			pl -> update ();
		pl -> enlarge ();

		Coordinate tmp;
		int tmpIndex;
		tmp.x = (pl -> x - GLOB_IND_W) / EDGE;
		tmp.y = (pl -> y - GLOB_IND_H) / EDGE;
		tmpIndex = binSearch (0, NumWall - 1, ArrWall, tmp);

		if (tmpIndex != -1 && pl -> x == pl -> xx && pl -> y == pl -> yy){
			if ((ArrWall [tmpIndex] -> name == "Rectangle" && pl -> statePl != rectangle) || (ArrWall [tmpIndex] -> name == "Circle" && pl -> statePl != circle) || (ArrWall [tmpIndex] -> name == "Triangle" && pl -> statePl != triangle)){
				pl -> changeCoord (Start.x, Start.y);
				changeState (startLVL);
				lvlDeath++;
				createWay ();
			}
			else if (ArrWall [tmpIndex] -> name == "Save"){
				Start.x = ArrWall [tmpIndex] -> x * EDGE + GLOB_IND_W;
				Start.y = ArrWall [tmpIndex] -> y * EDGE + GLOB_IND_H;
				createWay ();
			}
		}

		
		char tmpC [30]; //���������� ������� � ���������� �������
		button [indexTimePlBut] -> updateText (_itoa ((int) lvlTime, tmpC, 10));
		button [indexDeathPlBut] -> updateText (_itoa (lvlDeath, tmpC, 10));
		lvlTime += time;

		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "BackToMenuPl") || escapeReleased) && !changeStates){
					lvlTime -= time;
					escapeReleased = false;
					changeState (pause);
				}
				else if (lvlComplete && !changeStates){
					AllTime += lvlTime;
					lvlTime = 0;
					sndClickButt.play (); 
					lvlComplete = false;
					if (!playerLVL){
						if (CurrentLVL < 8){
							if (PassedLVL < 8 && CurrentLVL - 1 == PassedLVL)
								PassedLVL++;
							writeInfo ();
							CurrentLVL++; 
							Start.x = ArrWall [indexStart] -> x;
							Start.y = ArrWall [indexStart] -> y;
							Start.x = Start.x * EDGE + GLOB_IND_W;
							Start.y = Start.y * EDGE + GLOB_IND_H;
							saveLVL (fileNamePl);
							char tmpC [30], *tmpC2;
							tmpC2 = _itoa (CurrentLVL, tmpC, 10);
							char nameFile [30] = "Resources/LVLs/lvl";
							strcat (nameFile, tmpC2);
							strcat (nameFile, ".txt");
							openLVL_PL (nameFile); 
							strcpy (fileNamePl, nameFile);
							pl -> changeCoord (Start.x, Start.y);
							pl -> enlargePl = true;
							createWay ();
							enterReleased = false;
							changeState (startLVL);
						}
						else{
							Start.x = ArrWall [indexStart] -> x;
							Start.y = ArrWall [indexStart] -> y;
							Start.x = Start.x * EDGE + GLOB_IND_W;
							Start.y = Start.y * EDGE + GLOB_IND_H;
							saveLVL (fileNamePl);
							PassedLVL = 8;
							writeInfo ();
							changeState (selectLVL);
						}
					}
					else{
						playerLVL = false;
						NumAnsw = 0;
						Start.x = ArrWall [indexStart] -> x;
						Start.y = ArrWall [indexStart] -> y;
						Start.x = Start.x * EDGE + GLOB_IND_W;
						Start.y = Start.y * EDGE + GLOB_IND_H;
						saveLVL (fileNamePl);
						changeState (selectLVL);
					}
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
					if (!inSetIntoPause)
						changeState (menu);
					else{
						changeState (pause);
						inSetIntoPause = false;
					}
					break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "ControlsSet" && !changeStates){
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
					changeState (controlsSet); 
					break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "AudioSet" && !changeStates){
					changeState (audioSet); break;
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
						openLVL_PL (fileNamePl);
						
						pl -> changeCoord (Start.x, Start.y);
						pl -> statePl = rectangle;
						pl -> changeFigure2 ();
						pl -> enlargePl = true;
						playerLVL = false;
						createWay ();
						changeState (startLVL);
						
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
			if (button [i] -> drawThis){
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
			if (button [i] -> drawThis){
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
					if (tmpI - 8 < 16){
						for (int i = 0; i < tmpI; i++){
							inF >> tmpC2 [i];
							if (strcmp (tmpC2 [i], fileNameAd) == 0)
								edit = false;
						}
						inF.close ();

						ofstream outF ("Resources/LVLs/listLVLs.txt"); //������� ����� �������
						if (edit){
							outF << ++tmpI << endl;
							for (int i = 0; i < tmpI - 1; i++)
								outF << tmpC2 [i] << endl;
							outF << fileNameAd << endl;
							lvlDeath = 0;
						}
						else if (!edit){ //����������� ������ �������
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
				}
				else if (((button [i] -> buttClick && button [i] -> name == "BackToAdminSave") || escapeReleased) && !changeStates)
					changeState (admin);
			}
	}
	void StateAdDeleteLVL (){
		if (changeStates)
			changeState2 ();
		inputKeyboard (fileNameAd, 0);
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (button [i] -> name == "AdDeleteLVL" && !changeStates)
					button [i] -> updateText (fileNameAd);
				if (((button [i] -> buttClick && button [i] -> name == "AdDeleteLVL") || enterReleased) && !changeStates){
					changeState (admin);
					if (strstr (fileNameAd, "lvl") == NULL || strcspn (fileNameAd, "12345678") == NULL){
						int tmpI; 
						char tmpC2 [100][30]; 
						bool isDelete = false;
						ifstream inF ("Resources/LVLs/listLVLs.txt");
						inF >> tmpI;
					
						for (int j = 0; j < tmpI; j++){
							inF >> tmpC2 [j];
							if (strcmp (tmpC2 [j], fileNameAd) == 0)
								isDelete = true;
						}
						inF.close ();

						if (isDelete){
							char tmpC [50];
							strcpy (tmpC, "Resources/LVLs/");
							strcat (tmpC, fileNameAd);
							strcat (tmpC, ".txt");
							remove (tmpC);
							ofstream outF ("Resources/LVLs/listLVLs.txt"); //������� ����� �������
							outF << tmpI - 1 << endl;
							for (int i = 0; i < tmpI; i++)
								if (strcmp (tmpC2 [i], fileNameAd) != 0)
									outF << tmpC2 [i] << endl;
						}
					}
				}
				else if (((button [i] -> buttClick && button [i] -> name == "BackToAdminDelete") || escapeReleased) && !changeStates)
					changeState (admin);
			}
	}
	void StateAdListLVL (){
		if (changeStates)
			changeState2 ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "BackToAdminList") || escapeReleased) && !changeStates){
					changeState (admin); break;
				}
			}
	}
	void StatePause (){
		if (changeStates)
			changeState2 ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> state == pause){
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "LeaveToSel") || enterReleased) && !changeStates){
					AllTime += lvlTime;
					lvlTime = 0;
					NumAnsw = 0;
					writeInfo ();
					saveLVL (fileNamePl);
					changeState (selectLVL);
					lvlComplete = false;
					button [indexTimePlBut] -> updateText ("0");
					button [indexDeathPlBut] -> updateText ("0");
					break;
				}
				if (((button [i] -> buttClick && button [i] -> name == "BackToPlPause") || escapeReleased) && !changeStates){
					changeState (player); break;
				}
				if (((button [i] -> buttClick && button [i] -> name == "SettingsIntoPause") || escapeReleased) && !changeStates){
					changeState (settings); inSetIntoPause = true; break;
				}
			}
	}
	void StateStartLVL (){
		if (changeStates)
			changeState2 ();
		pl -> changeFigure ();
		pl -> enlarge ();
		if (anyKeyReleased && !changeStates && !escapeReleased){
			if (event.key.code != key [0] && event.key.code != key [1] && event.key.code != key [2])
				changeState (player);
		}
		else if (escapeReleased && !changeStates){
			AllTime += lvlTime; lvlTime = 0;
			NumAnsw = 0; writeInfo ();
			saveLVL (fileNamePl); 
			button [indexTimePlBut] -> updateText ("0");
			button [indexDeathPlBut] -> updateText ("0");
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
							if (strstr (myLVLname, "lvl") == NULL || strcspn (myLVLname, "12345678") == NULL){
								strcat (tmpC, myLVLname);
								strcat (tmpC, ".txt");
								openLVL_PL (tmpC);
								strcpy (fileNamePl, tmpC);
								pl -> changeCoord (Start.x, Start.y);
								pl -> statePl = rectangle;
								pl -> changeFigure2 ();
								pl -> enlargePl = true;

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
	void StateAudioSet (){
		if (changeStates)
			changeState2 ();
		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "BackToSetAudio") || escapeReleased) && !changeStates){
					writeInfo ();
					changeState (settings);
					break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "MusicSlider" && !changeStates){
					sndClickButt.play (); break;
				}
				else if (button [i] -> buttClick && button [i] -> name == "SoundSlider" && !changeStates){
					sndClickButt.play (); break;
				}
			}
	}
	void StateControlsSet (){
		if (changeStates)
			changeState2 ();

		if (anyKeyReleased && changeKey){
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
					}
					else if (event.key.code >= 26 && event.key.code <= 35 && tmpB){
						tmpC [0] = event.key.code + 22;
						tmpC [1] = '\0';
						button [i] -> updateText (tmpC);
						whatButChange = -1;
						key [button [i] -> value - 1] = event.key.code;
					}
					break;
				}
		}

		for (int i = 0; i < NumButton; i++)
			if (button [i] -> drawThis){
				button [i] -> checkCursor ();
				if (((button [i] -> buttClick && button [i] -> name == "BackToControlSet") || escapeReleased) && !changeStates){
					writeInfo ();
					whatButChange = -1;
					changeState (settings);
					break;
				}
				if (button [i] -> buttClick && button [i] -> name == "ChangeKey" && !changeStates){
					changeKey = true;
					sndClickButt.play ();
					whatButChange = button [i] -> value;
					break;
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
		case selectLVL:
			StateSelectLVL ();
			break;
		case AdSelectLVL:
			StateAdSelectLVL ();
			break;
		case AdSaveLVL:
			StateAdSaveLVL ();
			break;
		case AdDeleteLVL:
			StateAdDeleteLVL ();
			break;
		case AdListLVL:
			StateAdListLVL ();
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
		case audioSet:
			StateAudioSet ();
			break;
		case controlsSet:
			StateControlsSet ();
			break;
		}
	}
};

int main (){
	System system;
	view.reset (FloatRect (0, 0, (float) system.W_WIN, (float) system.H_WIN)); //�������� ������
	setCoordinateForView ((float) system.GLOBAL_W / 2, (float) system.GLOBAL_H / 2); //������� ������

	
	Game game;
	system.window = new RenderWindow (VideoMode (system.W_WIN, system.H_WIN), "LABYRINTH PRO", Style::Fullscreen, ContextSettings (0, 0, 1)); //�������� ����
	//system.window = new RenderWindow (VideoMode (system.W_WIN, system.H_WIN), "LABYRINTH PRO"); //�������� ����
	system.window -> setMouseCursorVisible (false); //�� ������ ������
	bool isUpdate = false;

	while (system.window -> isOpen ()){
		system.time = system.clock.getElapsedTime().asSeconds(); //����� ������ ��� �����������
		system.clock.restart ().asSeconds ();
		system.FPS = (int) (1.0 / system.time);
		char tmpC [30];
		_itoa (system.FPS, tmpC, 10);
		game.button [game.indexFPSBut] -> updateText (tmpC);

		system.mousePosWin = Mouse::getPosition (system.window [0]); //���������� ���� �����. ����
		system.posMouse = system.window -> mapPixelToCoords (system.mousePosWin); //���������� ���� �����. �����


		while (system.window -> pollEvent (system.event)){ //������� � ���� ����� ���-�� ���������� (����� ������� ������ ��� �������� �� �������)
			if (system.event.type == Event::Closed) //���� ����������� ����� ������ �������
				system.window -> close (); 
			isUpdate = true;
			game.update ();

			if (system.event.type == Event::KeyReleased && system.event.key.code == 36) game.escapeReleased = true; //����, � ��� ����� �� ������
			else game.escapeReleased = false;
			if (system.event.type == Event::KeyReleased && system.event.key.code == 58) game.enterReleased = true; //����, � ��� ����� �� �����
			else game.enterReleased = false; 
			if (system.event.type == Event::KeyReleased) game.anyKeyReleased = true; //����, � ��� ������ �� �����-���� ������
			else game.anyKeyReleased = false;

			if (system.event.type == Event::KeyPressed) //����� ��� ��������� ����� ������, ��� � �������� ��� ������� �������, � �� ����������
				system.keyCodePressed = system.event.key.code;
		}
		if (!isUpdate){ //��������� ���� ���� �� ����� � ���������� while
			game.update ();
			game.escapeReleased = false;
			game.enterReleased = false;
			game.anyKeyReleased = false;
		}
		
		isUpdate = false;
		if (game.state == player)
			game.StatePlayer ();
		system.keyCodePressed = -1;

		game.draw ();
	}
	return 0;
}