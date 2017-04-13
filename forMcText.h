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
// ���������� ����������� �� �����������
void mcText::setLimit(bool flag, int num)
{
	isLimitW = flag;
	limitW = num;
}
// �������� ��������� ������
void mcText::add(sf::String str)
{
	add(str, sf::Color::Black);
}
// ������� ��������� ������� ������
void mcText::add(sf::String str, sf::Color color)
{
	mcTextInfo *t = new mcTextInfo();
	t->color = color;
	t->str = str;

	// ������ �����
	sf::Text text;
	text.setFont(*font);
	text.setCharacterSize(charSize);
	text.setFillColor(t->color);
	text.setString(t->str);

	// ��� �� ��������� � ������, � �� ��� ������ ����� ��������
	t->w = text.getLocalBounds().width;
	t->h = text.getLocalBounds().height;
	t->dx = dx;
	dx += t->w;
	t->dy = dy;
		
	// ���� �������� ����������� �� W
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
// ���������� �������� �������������� ������
void mcText::add2(sf::String str)
{
	std::vector<sf::String> vecWords;
	std::vector<sf::String> vecStrings;
	sf::Text text = vecText[0];
	
	//����� ����� �� �����
	vecWords = scanWords(str);
	//������������ ����� �� ������� (�������� �����������)
	vecStrings = scanString(limitW, vecWords, &text);
	//��������� ������ � �������� ������ �����
	for(int i=0; i<vecStrings.size(); i++)
	{
		add(vecStrings[i]);
		enter();
	}
};
// ������� �� ��������� ������
void mcText::enter()
{
	dx = 0;
	dy += charSize;
}
// �������� �� value �������� �� �����������
void mcText::space(int value)
{
	dx += value;
}
// �������� �� value �������� �� ���������
void mcText::spaceY(int value)
{
	dy += value;
}
// ����������� ���� ����� �� dx:dy
void mcText::move(float dx, float dy)
{
	for(std::vector<sf::Text>::iterator it = vecText.begin(); it != vecText.end(); it++)
		(*it).move(dx, dy);
}
// ����������� ���� ����� � ���������� x:y
void mcText::setPosition(float x, float y)
{
	for(int i=0; i<vecText.size(); i++)
		vecText[i].setPosition(x + vecInfo[i]->dx, y + vecInfo[i]->dy);
}
// ���������� �� �������� ����
void mcText::draw(sf::RenderWindow* window)
{
	for(std::vector<sf::Text>::iterator it = vecText.begin(); it != vecText.end(); it++)
		window->draw(*it);
}
// ���������� ������ ���� ��� �������� | ��������� �����
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
// ���������� ������ ������� ����� | ��������� ������ � ������ ���� � ����������� ����� (� ������� � ��������)
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
		text->setString(buff2); // ������ ������ ����� �������� ������ ������
		if(text->getLocalBounds().width > w)
		{// ���� ��� ���������� ����� ������ ����� ������ ������
			vecStr.push_back(buff); // ��������� � ������ ����� �����1 (�� �������� ���������� �����)
			buff = "";           // ������ �����1
			buff2 = vecWords_[i]; // � �����2 ���������� ��������� ����� (�.�. ������� �� ����. ������)
		}
	}
	vecStr.push_back(buff2);
	return vecStr;
}
// ��������� �������� �����
void mcText::clear()
{
	vecInfo.clear();
	vecText.clear();
	data();
}
//--------------------------------------------------------------------------------------------------
//�������� ������ ������
void mcText::changeSize (int tmpS){
	charSize = tmpS;
}