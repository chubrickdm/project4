#include "mcText.h"

void mcText::data()
{
	dx = 0;
	dy = 0;
	charSize = 16;
	setLimit(false, 0);
	add("");
}

mcText::mcText(sf::Font* f)
{
	font = f;
	data();
}

mcText::~mcText(void)
{
}
// установить ограничение по горизонтали
void mcText::setLimit(bool flag, int num)
{
	isLimitW = flag;
	limitW = num;
}
// добавить небольшую строку
void mcText::add(sf::String str)
{
	add(str, sf::Color::Black);
}
// доавить небольшую цветную строку
void mcText::add(sf::String str, sf::Color color)
{
	mcTextInfo *t = new mcTextInfo();
	t->color = color;
	t->str = str;

	// Создаём текст
	sf::Text text;
	text.setFont(*font);
	text.setCharacterSize(charSize);
	text.setFillColor(t->color);
	text.setString(t->str);

	// Как всё гениально и просто, я аж сам офигел когда придумал
	t->w = text.getLocalBounds().width;
	t->h = text.getLocalBounds().height;
	t->dx = dx;
	dx += t->w;
	t->dy = dy;
		
	// Если включено ограничение по W
	if(isLimitW && (t->dx + t->w > limitW))
	{
		enter();
		t->dx = dx;
		t->dy = dy;
	}

	text.move(t->dx, t->dy);

	vecInfo.push_back(t);
	vecText.push_back(text);
}
// Добавление большого многострочного текста
void mcText::add2(sf::String str)
{
	std::vector<sf::String> vecWords;
	std::vector<sf::String> vecStrings;
	sf::Text text = vecText[0];
	
	//режем текст на слова
	vecWords = scanWords(str);
	//распределяем слова по строкам (учитывая ограничение)
	vecStrings = scanString(limitW, vecWords, &text);
	//добавляем строки в основной вектор строк
	for(int i=0; i<vecStrings.size(); i++)
	{
		add(vecStrings[i]);
		enter();
	}
};
// Переход на следующую строку
void mcText::enter()
{
	dx = 0;
	dy += charSize;
}
// отступит на value пикселей по горизонтали
void mcText::space(int value)
{
	dx += value;
}
// отступит на value пикселей по вертикали
void mcText::spaceY(int value)
{
	dy += value;
}
// переместить весь текст на dx:dy
void mcText::move(float dx, float dy)
{
	for(std::vector<sf::Text>::iterator it = vecText.begin(); it != vecText.end(); it++)
		(*it).move(dx, dy);
}
// переместить весь текст в координаты x:y
void mcText::setPosition(float x, float y)
{
	for(int i=0; i<vecText.size(); i++)
		vecText[i].setPosition(x + vecInfo[i]->dx, y + vecInfo[i]->dy);
}
// отрисовать во входящем окне
void mcText::draw(sf::RenderWindow* window)
{
	for(std::vector<sf::Text>::iterator it = vecText.begin(); it != vecText.end(); it++)
		window->draw(*it);
}
// Возвращает вектор слов без пробелов | принимает текст
std::vector<sf::String> mcText::scanWords(sf::String str)
{
	std::vector<sf::String> vecStr;
	sf::String buff;

	for(int i=0; i<str.getSize(); i++)
		if(str[i] != ' ')
			buff += str[i];
		else
			if(buff != "")
			{
				vecStr.push_back(buff);
				buff = "";
			}
	vecStr.push_back(buff);
	return vecStr;
}
// Возвращает вектор готовых строк | Принимает ширину и вектор слов и НАСТРОЕННЫЙ текст (с шрифтом и размером)
std::vector<sf::String> mcText::scanString(int w, std::vector<sf::String> vecWords_, sf::Text* text)
{
	std::vector<sf::String> vecStr;
	sf::String buff;
	sf::String buff2;

	for(int i=0; i<vecWords_.size(); i++)
	{
		buff = buff2;
		if(buff2 == "")
			buff2 += vecWords_[i];
		else
			buff2 += " " + vecWords_[i];
		text->setString(buff2); // делаем строку чтобы измерить ширину строки
		if(text->getLocalBounds().width > w)
		{// Если при добавлении слова ширина стала больше нужной
			vecStr.push_back(buff); // добавляем в вектор строк буфер1 (не содержит последнего слова)
			buff = "";           // чистим буфер1
			buff2 = vecWords_[i]; // в буфер2 засовываем последнее слово (т.к. перенос на след. строку)
		}
	}
	vecStr.push_back(buff2);
	return vecStr;
}
// Полностью очистить текст
void mcText::clear()
{
	vecInfo.clear();
	vecText.clear();
	data();
}
//--------------------------------------------------------------------------------------------------
//изменить размер шрифта
void mcText::changeSize (int tmpS){
	charSize = tmpS;
}