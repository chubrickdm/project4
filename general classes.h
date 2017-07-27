#pragma once
#include "body.h"

class Wall : public Body{ //����� �����
public:
	Wall (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){ //����������� � ������
		shape.setPosition ((float) x * EDGE + GLOB_IND_W, (float) y * EDGE + GLOB_IND_H);
		if (name == "Save")            shape.setTextureRect (IntRect (0, 0,            wTexture, hTexture));
		else if (name == "Wall")       shape.setTextureRect (IntRect (0, hTexture,     wTexture, hTexture));
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
			if (name == "Save")            shape.setTextureRect (IntRect (0, 0,            wTexture, hTexture));
			else if (name == "Wall")       shape.setTextureRect (IntRect (0, hTexture,     wTexture, hTexture));
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

	void changeLocation (int tmpX, int tmpY){
		shape.setPosition ((float) tmpX, (float) tmpY);
	}
};

class Background : public Body{ //����� �������� �����������
public:
	Background (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){ 
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	void changeLocation (int x2, int y2){ //������� ��������� ��������� ������� ������� ���� (����� ���� ��������� ��� ����� ������)
		shape.setPosition ((float) x2, (float) y2);
	}

	void draw (){ 
		window -> draw (shape);
	}
};

class Player : public Body{ //����� ������
private:
	bool F_transformation; //����, ������� ������������ ���������������� �� �����
	bool F_secPhaseTransformation; //����, ������� ���������� ������ �� ���� ������������� ������ ������
	float reducePrecent; //������� ����������
	float enlargePrecent; //������� ����������
	float rotation; //���� �� ������� �������������� ����� ��� ������� ����� ������ � ������ ����
	int tmpX, tmpY; //���������� ������� ������ ����� ���� �� ����� �������, ����� �������
	StatePlayer stateWill; //������ ��������� � ������� ���������������� �����
public:
	bool F_move; //����, ������� ���������� �������� �� �����
	bool F_teleportation; //����, ������� ���������� ����� ��������������� ��
	bool F_reduce;
	bool F_enlarge; //����, ������� ���� �� ������ ������ ����
	float xx, yy; //�����, ������ ��� �������� �� ����� �����, � ����� ������ ���� �� ����� �����, � �� �������� � � � ��� ���� ��� � ���������� ���������� ���� �� ����� ���������
	int currDirection; //������� �����������
	StatePlayer state; //��������� ������ ������������ ������
private:
	void EFF_reduce (){ //���������� ������ (����� ������)
		shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
		shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
		shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
		shape.setRotation (rotation);

		rotation += speedChangeSt * time * 1.5f;
		reducePrecent -= speedChangeSt * time / 3; //����� ����������� ��������� ����� 
		if (reducePrecent < speedChangeSt * time / 3){
			reducePrecent = 100; F_lvlComplete = true;
			rotation = 0; F_reduce = false;
		}
	}

	void EFF_transformation (){
		if (F_secPhaseTransformation){
			shape.setSize (Vector2f ((float) w * timer / 0.15f, (float) h * timer / 0.15f));
			shape.setOrigin ((float) w * timer / 0.15f / 2, (float) h * timer / 0.15f / 2);
			shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
		}
		else{
			shape.setSize (Vector2f ((float) w * (1 - timer / 0.15f), (float) h * (1 - timer / 0.15f)));
			shape.setOrigin ((float) w * (1 - timer / 0.15f) / 2, (float) h * (1 - timer / 0.15f) / 2);
			shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
		}

		if (timer >= 0.15f && !F_secPhaseTransformation){
			F_secPhaseTransformation = true;
			timer = 0;
			state = stateWill;
			changeFigureStatic ();
		}
		else if (timer >= 0.15f && F_secPhaseTransformation){
			timer = 0;
			F_secPhaseTransformation = false;
			F_transformation = false;
			shape.setSize (Vector2f ((float) w, (float) h));
			shape.setOrigin ((float) w / 2, (float) h / 2);
			shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
		}
	}
public:
	Player (Image &image, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, "Player", X, Y, W, H, WTexture, HTexture){ //����������� ��� �����
	    xx = (float) x; yy = (float) y; 
		tmpX = x; tmpY = y;
		F_move = false; F_reduce = false;
		F_teleportation = false; F_transformation = false; F_secPhaseTransformation = false;
		reducePrecent = 100; rotation = 0; enlargePrecent = 1; F_enlarge = false;

		currDirection = 0; state = rectangle; 
		shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	void EFF_enlarge (){ //���������� ������ (������� ����)
		if (F_enlarge){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
			shape.setRotation (rotation);

			rotation += (float) (speedChangeSt * time * 1.5);
			enlargePrecent += speedChangeSt * time / 3; //����� ����������� ��������� �����
			if (enlargePrecent > 100 - speedChangeSt * time / 3){
				enlargePrecent = 1; F_enlarge = false;
				shape.setSize (Vector2f ((float) w, (float) h));
				shape.setOrigin ((float) w / 2, (float) h / 2);
				shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
				shape.setRotation (0); rotation = 0;
			}
		}
	}

	void changeFigureKey (){ //��������� ������ �� ������� �������
		if (!F_secPhaseTransformation){
			if (keyCodePressed == key [0]){
				if (state != rectangle){
					F_transformation = true; stateWill = rectangle;
				}
			}
			else if (keyCodePressed == key [1]){
				if (state != triangle){
					F_transformation = true; stateWill = triangle;
				}
			}
			else if (keyCodePressed == key [2]){
				if (state != circle){
					F_transformation = true; stateWill = circle;
				}
			}
		}

		if (F_transformation){
			EFF_transformation (); timer += time;
		}
	}

	void changeFigureStatic (){ //��������� ������ ��������������� ��������� ������
		if (state == rectangle)     shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		else if (state == triangle) shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		else if (state == circle)   shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
	}

	void update (){
		if (x == Finish.x && y == Finish.y){ //���� �� �������� ������, �� ����� �������� ������, ����������������� �� ����
			EFF_reduce (); F_transformation = false; 
			F_secPhaseTransformation = false; F_reduce = true;
		}
		else if (!F_teleportation){
			if (currDirection < NumMoves && !F_move){
				if (Direction [currDirection] == 4)        tmpY = y - EDGE; //���������� ���������� ���� �� ����� ������
				else if (Direction [currDirection] == 1)   tmpY = y + EDGE; //���������� ���������� ���� �� ����� ������
				else if (Direction [currDirection] == 2)   tmpX = x - EDGE; //���������� ���������� ���� �� ����� ������ 
				else if (Direction [currDirection] == 3)   tmpX = x + EDGE; //���������� ���������� ���� �� ����� ������
				currDirection++; F_move = true;
				xx = (float) x; yy = (float) y;
			}

			changeFigureKey ();

			if (F_move){ //���������, ��� �� ����� �� ��� ����� ���� �� ����� �������
				//cout << xx << "-xx   " << yy << "-yy\n" << tmpX << "-tmpX   " << tmpY << "-tmpY\n" << speed * time << "-speed * time\n" << abs (xx - (float) tmpX) << "-xx - tmpX   " << abs (yy - (float) tmpY) << "-yy - tmpY\n" << endl;

				if (abs (xx - (float) tmpX) <= speed * time && abs (yy - (float) tmpY) <= speed * time){ //�� �������� �������� ����� ����� ������ ��������� ������, ��������� ���������� � ������ ��������
					F_move = false; 
					xx = (float) tmpX; yy = (float) tmpY;
					x = tmpX; y = tmpY;
				}
				else{ //���� �������� ������
					if (x < tmpX)        xx += speed * time; //�������� �� �����������
					else if (x > tmpX)   xx -= speed * time; 
					else if (y < tmpY)   yy += speed * time; //���������� ���������
					else if (y > tmpY)   yy -= speed * time;  
					else{
						yy = (float) tmpY; xx = (float) tmpX; //�� �������� ����� � = tmpY, � yy - tmpY ������ ��� speed * time, �����
						//���������� ��������� ������, ���� �� ����� ����� �����, ��� speed * time ����� ������
					}
					x = (int) xx; y = (int) yy;
				}
			}
		}
	}

	void changeLocation (int x2, int y2){ //������� ����� ��� ����������� ������ � ������ ���������� (����� ��� �������� ������ �������)
		shape.setSize (Vector2f ((float) w, (float) h));
		shape.setOrigin ((float) w / 2, (float) h / 2);
		shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
		shape.setRotation (0);
		enlargePrecent = 1; reducePrecent = 100;
		rotation = 360 - ((FPS * (100 / speedChangeSt) * 3) * (float) (speedChangeSt * time * 1.5));

		x = x2; y = y2; 
		xx = (float) x; yy = (float) y;
		tmpX = x; tmpY = y; 
	}

	void EFF_teleportation (int x2, int y2){ //������� ����� ��� ����������� ������ � ������ ���������� (����� ��� �������� ������ �������)
		shape.setTextureRect (IntRect (0, hTexture * 3, wTexture, hTexture));
		shape.setSize (Vector2f ((float) w * (1 - timer / 0.3f), (float) h * (1 - timer / 0.3f)));
		shape.setOrigin ((float) w * (1 - timer / 0.3f) / 2, (float) h * (1 - timer / 0.3f) / 2);
		shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
		F_teleportation = true;
		if (timer > 0.3){
			timer = 0;
			x = x2; y = y2; 
			xx = (float) x; yy = (float) y;
			tmpX = x; tmpY = y; 

			F_teleportation = false; F_transformation = false; F_secPhaseTransformation = false;
			shape.setSize (Vector2f ((float) w, (float) h));
			shape.setOrigin ((float) w / 2, (float) h / 2);
			shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
			changeFigureStatic ();
		}
	}

	void draw (){ 
		window -> draw (shape);
	}
};

class BodyButton : public Body{ //���� ������
protected:
	Color color; //���� ������ ������
	mcText *text; //����� ������� ��������� �� ������
	String buttText; //����� ������� ����� ������������ �� ������
private:
	Font font; //�����
public:
	bool F_draw; //����, ������� ���������� �������� �� ��� ������
	bool F_pressed; //����, ������� ���������� ������ �� ������
	bool F_click; //����, ������� ���������� �������� �� �� ������ (����- ��� ������ � ��������� ������ ����� ������ ���� �� ������)
	bool F_transformation; //���� ������������ ���������� �� �����
	float reducePrecent; //������� ����������
	float enlargePrecent; //������� ����������
	int value; //�������� ������
	TypeState type;
	SubtypeState subtype;
public:
	BodyButton (Image &image, String Text, String Name, Font &Font, SubtypeState &Subtype, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    Body (image, Name, X, Y, W, H, WTexture, HTexture){
	    font = Font; buttText = Text; 
		subtype = Subtype; type = findType (subtype); 
		F_draw = false; F_click = false; F_pressed = false; 
		F_transformation = false; reducePrecent = 100; enlargePrecent = 1;

		color = Color::Black; //�� ��������� ����� ������
		text = new mcText (&font); //������� ����� ������� ����� ������������ �� ������
		text -> changeSize (SIZE_TEXT); //������ ������
		text -> add (buttText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //����������� ����� �� ������
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	BodyButton (Image &image, String Name, SubtypeState &Subtype, int X, int Y, int W, int H, int WTexture, int HTexture) : //��� ������ ������, ��� ������ � ���������� ����� ���
		    Body (image, Name, X, Y, W, H, WTexture, HTexture){
		subtype = Subtype; type = findType (subtype); 
		F_draw = false; F_click = false; F_pressed = false; 
		F_transformation = false; reducePrecent = 100; enlargePrecent = 1;

		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	BodyButton (String Text, String Name, Font &Font, SubtypeState &Subtype, int X, int Y) : //��� ������� ������� ����������
		    Body (Name, X, Y, 1, 1, 1, 1){
	    font = Font; buttText = Text;
		subtype = Subtype; type = findType (subtype);
		F_draw = false; F_click = false; F_pressed = false; 
		F_transformation = false; reducePrecent = 100; enlargePrecent = 1;

		color = Color::Black;
		text = new mcText (&font); //������� ����� ������� ����� ������������ �� ������
		text -> changeSize (SIZE_TEXT); //������ ������
		text -> add (buttText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //����������� ����� �� ������
	}

	void draw (){ }

	virtual void checkCursor () = 0;

	virtual void updateText (char *Pass) = 0;

	virtual void EFF_reduce (){ //���������� ������
		if (F_transformation){
			shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
			shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);
			text -> clear ();
			text -> changeSize (SIZE_TEXT * (int) reducePrecent / 100); //������ ������
			text -> add (buttText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * reducePrecent / 100 / 3); //����������� ����� �� ������

			reducePrecent -= speedChangeSt * time; //����� ����������� ��������� �����
			if (reducePrecent < speedChangeSt * time){
				reducePrecent = 100;
				F_transformation = false;
			}
		}
	}

	virtual void EFF_enlarge (){ //���������� ������
		if (F_transformation){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);
			text -> clear ();
			text -> changeSize (SIZE_TEXT * (int) enlargePrecent / 100); //������ ������
			text -> add (buttText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * enlargePrecent / 100 / 3); //����������� ����� �� ������

			enlargePrecent += speedChangeSt * time; //����� ����������� ��������� �����
			if (enlargePrecent > 100 - speedChangeSt * time){
				enlargePrecent = 1;
				F_transformation = false;

				shape.setSize (Vector2f ((float) w, (float) h));
				shape.setOrigin ((float) w / 2, (float) h / 2);
				shape.setPosition ((float) x, (float) y);
				text -> clear ();
				text -> changeSize (SIZE_TEXT); //������ ������
				text -> add (buttText, color);
				text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //����������� ����� �� ������
			}
		}
	}

	virtual void clear (){ //������� �������� ������, ����� ��� � ������ ������ ����������� (������ ����)
		shape.setSize (Vector2f (1, 1));
		text -> clear ();
	}
};