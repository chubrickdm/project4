#pragma once
#include "system.h"

class Body : public System{ //����� ������� ������ ������� ��� ���� ������ ������� � ��������
protected:
	int w, h; //������, ������
	Texture texture; //��������
	RectangleShape shape; //��������
	int wTexture, hTexture; //��� �� �������������� ��������, ����� ����� ����� ������ ����� ����� ��������������
public:
	int x, y; //����������
	String name; //���
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

	Body (String Name, int X, int Y, int W, int H, int WTexture, int HTexture){
		x = X; y = Y; w = W; h = H; name = Name;
		wTexture = WTexture; hTexture = HTexture;
	}

	virtual void draw () = 0;
};