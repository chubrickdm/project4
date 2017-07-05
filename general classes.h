#pragma once
#include "body.h"

class Wall : public Body{ //класс стены
public:
	Wall (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){ //конструктор с именем
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

	void changeCoord (int tmpX, int tmpY){
		shape.setPosition ((float) tmpX, (float) tmpY);
	}
};

class Background : public Body{ //класс фонового изображения
public:
	Background (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){ 
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	void changeCoord (int x2, int y2){ //функция изменения координат черного заднего фона (центр фона находится где центр игрока)
		shape.setPosition ((float) x2, (float) y2);
	}

	void draw (){ 
		window -> draw (shape);
	}
};

class Player : public Body{ //класс игрока
private:
	int tmpX, tmpY; //переменные которые хранят место куда мы хотим попасть, нажав клавишу
	bool playerMove; //движется ли игрок
	float xx, yy; //нужны, потому что скорость не целое число, и коорд игрока тоже не целое число, и мы оставили х и у для того что б запоминать координаты куда мы хотим двигаться
	float reducePrecent; //процент уменьшения
	float rotation; //угол на который поворачивается игрок при эффекте конца уровня и начале игры
	float enlargePrecent; //процент увелечения
public:
	int currDir; //текущее направление
	StatePlayer statePl; //состояние игрока соотвествует фигуре
	bool enlargePl; //флаг, который идет ли эффект начала игры
private:
	void reduce (){ //уменьшение игрока (конец уровня)
		shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
		shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
		shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
		shape.setRotation (rotation);

		rotation += (float) (speedChangeSt * time * 1.5);
		reducePrecent -= speedChangeSt * time / 3; //когда прекратится изменение формы 
		if (reducePrecent < speedChangeSt * time / 3){
			reducePrecent = 100; lvlComplete = true;
			rotation = 0;
		}
	}
public:
	Player (Image &image, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, "Player", X, Y, W, H, WTexture, HTexture){ //конструктор без имени
	    xx = (float) x; yy = (float) y; 
		tmpX = x; tmpY = y;
		playerMove = false;
		reducePrecent = 100; rotation = 0; enlargePrecent = 1; enlargePl = false;

		currDir = 0; statePl = rectangle; 
		shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	void enlarge (){ //увелечение игрока (вначале игры)
		if (enlargePl){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
			shape.setRotation (rotation);

			rotation += (float) (speedChangeSt * time * 1.5);
			enlargePrecent += speedChangeSt * time / 3; //когда прекратится изменение формы
			if (enlargePrecent > 100 - speedChangeSt * time / 3){
				enlargePrecent = 1; enlargePl = false;
				shape.setSize (Vector2f ((float) w, (float) h));
				shape.setOrigin ((float) w / 2, (float) h / 2);
				shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
				shape.setRotation (0); rotation = 0;
			}
		}
	}

	void changeFigure (){ //изменение фигуры по нажатию клавиши
		if (keyCodePressed == key [0]){
			statePl = rectangle;
			shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		}
		else if (keyCodePressed == key [1]){
			statePl = triangle;
			shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		}
		else if (keyCodePressed == key [2]){
			statePl = circle;
			shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
		}
	}

	void changeFigure2 (){ //изменение фигуры соотвествующему состоянию игрока
		if (statePl == rectangle)     shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		else if (statePl == triangle) shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		else if (statePl == circle)   shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
	}

	void update (){
		if (x == Finish.x && y == Finish.y){ //есди мы достигли финиша, то будет показана кнопка, свидетельствующая об этом
			reduce ();
		}
		else{
			if (currDir < NumMoves && !playerMove){
				if (Direction [currDir] == 4)        tmpY = y - EDGE; //запоминаем координаты куда мы должы придти
				else if (Direction [currDir] == 1)   tmpY = y + EDGE; //запоминаем координаты куда мы должы придти
				else if (Direction [currDir] == 2)   tmpX = x - EDGE; //запоминаем координаты куда мы должы придти 
				else if (Direction [currDir] == 3)   tmpX = x + EDGE; //запоминаем координаты куда мы должы придти
				currDir++; playerMove = true;
				xx = (float) x; yy = (float) y;
			}

			changeFigure ();

			if (playerMove){ //проверяем, нет ли стены на том месте куда мы хотим перейти
				//cout << speed << " -speed" << endl; //очень удобно проверять физику движения игрока
				//cout << xx << " " << yy << " -xx & yy" << endl;
				//cout << tmpX << " " << tmpY << " -tmpX & tmpY" << endl;
				//cout << xx - tmpX << " " << yy - tmpY << " difference" << endl;
				//cout << endl;
				if (abs (xx - (float) tmpX) < speed * time && abs (yy - (float) tmpY) < speed * time){ //по разности понимаем когда игрок достиг следующей клетки, округляем координаты и дальше движемся
					playerMove = false; 
					xx = (float) tmpX; yy = (float) tmpY;
					x = tmpX; y = tmpY;
					xx = (float) x; yy = (float) y;
				}
				else{ //само движение игрока
					if (x < tmpX)        xx += speed * time; //движение по горизонтали
					else if (x > tmpX)   xx -= speed * time; 
					else if (y < tmpY)   yy += speed * time; //движениепо вертикали
					else if (y > tmpY)   yy -= speed * time;  
					x = (int) xx; y = (int) yy;
				}
			}
		}
	}

	void changeCoord (int x2, int y2){ //функция нужна для перемещения игрока в нужную координату (нужно при открытии уровня игркоом)
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

	void draw (){ 
		window -> draw (shape);
	}
};

class BodyButton : public Body{ //тело кнопок
protected:
	Color color; //цвет текста кнопки
	mcText *text; //текст который выводится на кнопке
	String buttText; //текст который будет отображаться на кнопке
private:
	Font font; //шрифт
public:
	bool drawThis, buttPressed, buttClick; //рисовать ли кнопку, нажата ли кнопка и кликнули ли по кнопке. Клик- это нажать и отпустить кнопку когда курсор мыши на кнопке
	StateList state; //каждая кнопка кроме имени имеет группу к которой она относится
	int value; //значение кнопки
	
	bool changeForm; //флаг показывающий изменилась ли форма
	float reducePrecent; //процент уменьшения
	float enlargePrecent; //процент увелечения
public:
	BodyButton (Image &image, String Text, String Name, Font &Font, StateList &State, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    Body (image, Name, X, Y, W, H, WTexture, HTexture){
	    font = Font; state = State; buttText = Text;
		drawThis = false; buttClick = false; buttPressed = false; 
		changeForm = false; reducePrecent = 100; enlargePrecent = 1;

		color = Color::Black; //по умолчанию текст черный
		text = new mcText (&font); //создаем текст который будет отображаться на кнопке
		text -> changeSize (SIZE_TEXT); //размер текста
		text -> add (buttText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	BodyButton (String Text, String Name, Font &Font, StateList &State, int X, int Y) : //для статика сделана перегрузка
		    Body (Name, X, Y, 1, 1, 1, 1){
	    font = Font; state = State; buttText = Text;
		drawThis = false; buttClick = false; buttPressed = false; 
		changeForm = false; reducePrecent = 100; enlargePrecent = 1;

		color = Color::Black;
		text = new mcText (&font); //создаем текст который будет отображаться на кнопке
		text -> changeSize (SIZE_TEXT); //размер текста
		text -> add (buttText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
	}

	void draw (){ }

	virtual void checkCursor () = 0;

	virtual void updateText (char *Pass) = 0;

	virtual void reduceButton (){ //уменьшение кнопки
		if (changeForm){
			shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
			shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);
			text -> clear ();
			text -> changeSize (SIZE_TEXT * (int) reducePrecent / 100); //размер текста
			text -> add (buttText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * reducePrecent / 100 / 3); //распологаем текст по кнопке

			reducePrecent -= speedChangeSt * time; //когда прекратится изменение формы
			if (reducePrecent < speedChangeSt * time){
				reducePrecent = 100;
				changeForm = false;
			}
		}
	}

	virtual void enlargeButton (){ //увелечение кнопки
		if (changeForm){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);
			text -> clear ();
			text -> changeSize (SIZE_TEXT * (int) enlargePrecent / 100); //размер текста
			text -> add (buttText, color);
			if (name == "Edit")
				text -> clear ();
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * enlargePrecent / 100 / 3); //распологаем текст по кнопке

			enlargePrecent += speedChangeSt * time; //когда прекратится изменение формы
			if (enlargePrecent > 100 - speedChangeSt * time){
				enlargePrecent = 1;
				changeForm = false;

				shape.setSize (Vector2f ((float) w, (float) h));
				shape.setOrigin ((float) w / 2, (float) h / 2);
				shape.setPosition ((float) x, (float) y);
				text -> clear ();
				text -> changeSize (SIZE_TEXT); //размер текста
				text -> add (buttText, color);
				if (name == "Edit")
					text -> clear ();
				text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
			}
		}
	}

	virtual void clearButton (){ //функция очищения кнопки, нужна что б начать кнопки увеличивать (вторая фаза)
		shape.setSize (Vector2f (1, 1));
		text -> clear ();
	}
};