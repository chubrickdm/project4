#include "stdafx.h"
#pragma once
/////////////////////////////////
// multiline colored text
// mcText
/////////////////////////////////

// структура содержащая информацию о тексте
struct mcTextInfo
{
	sf::String str;  // Строка
	sf::Color color; // Цвет
	int w;  // ширина текста
	int h;  // высота текста
	int dx; // смещение текста по X
	int dy; // смещение текста по Y 
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
	bool isLimitW; // нужно ли учитывать ограничение по ширине
	int dx; // меняеться в программе автоматически
	int dy; // меняеться в программе автоматически
	int limitW; // ограничение по ширине
	sf::Font* font; // ссылка на шрифт
	int charSize; // размер символа
	std::vector<mcTextInfo*> vecInfo; // вектор содержащий описания текстов
	std::vector<sf::Text> vecText;    // вектор содержащий сами тексты
	void data(); // задаёт начальную информацию
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