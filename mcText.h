#include "stdafx.h"
#pragma once
/////////////////////////////////
// multiline colored text
// mcText
/////////////////////////////////

// ��������� ���������� ���������� � ������
struct mcTextInfo
{
	sf::String str;  // ������
	sf::Color color; // ����
	int w;  // ������ ������
	int h;  // ������ ������
	int dx; // �������� ������ �� X
	int dy; // �������� ������ �� Y 
	mcTextInfo()
	{
		data();
	}
	void data()
	{
		w = 0;
		h = 0;
		dx = 0;
		dy = 0;
		color = sf::Color::Black;
	}
};

class mcText
{
public:
	bool isLimitW; // ����� �� ��������� ����������� �� ������
	int dx; // ��������� � ��������� �������������
	int dy; // ��������� � ��������� �������������
	int limitW; // ����������� �� ������
	sf::Font* font; // ������ �� �����
	int charSize; // ������ �������
	std::vector<mcTextInfo*> vecInfo; // ������ ���������� �������� �������
	std::vector<sf::Text> vecText;    // ������ ���������� ���� ������
	void data(); // ����� ��������� ����������
public:
	mcText(sf::Font* f);
	~mcText(void);

	void setLimit(bool flag, int num);
	void add(sf::String str);
	void add(sf::String str, sf::Color color);
	void add2(sf::String str);
	void enter();
	void space(int value);
	void spaceY(int value);
	void move(float dx, float dy);
	void setPosition(float x, float y);
	void draw(sf::RenderWindow* window);
	std::vector<sf::String> scanWords(sf::String str);
	std::vector<sf::String> scanString(int w, std::vector<sf::String> vecWords_, sf::Text* text);
	void clear();
	//-------------------------------------------------------------------------------------------
	void changeSize (int tmpS);
};