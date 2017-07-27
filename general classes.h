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

	void changeLocation (int tmpX, int tmpY){
		shape.setPosition ((float) tmpX, (float) tmpY);
	}
};

class Background : public Body{ //класс фонового изображения
public:
	Background (Image &image, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, Name, X, Y, W, H, WTexture, HTexture){ 
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	void changeLocation (int x2, int y2){ //функция изменения координат черного заднего фона (центр фона находится где центр игрока)
		shape.setPosition ((float) x2, (float) y2);
	}

	void draw (){ 
		window -> draw (shape);
	}
};

class Player : public Body{ //класс игрока
private:
	bool F_transformation; //флаг, который показывается трансформируется ли игрок
	bool F_secPhaseTransformation; //флаг, который показывает вторая ли фаза трансформации игрока сейчас
	float reducePrecent; //процент уменьшения
	float enlargePrecent; //процент увелечения
	float rotation; //угол на который поворачивается игрок при эффекте конца уровня и начале игры
	int tmpX, tmpY; //переменные которые хранят место куда мы хотим попасть, нажав клавишу
	StatePlayer stateWill; //хранит состояние в которое трансформируется игрок
public:
	bool F_move; //флаг, который показывает движется ли игрок
	bool F_teleportation; //флаг, который показывает игрок телепортируется ли
	bool F_reduce;
	bool F_enlarge; //флаг, который идет ли эффект начала игры
	float xx, yy; //нужны, потому что скорость не целое число, и коорд игрока тоже не целое число, и мы оставили х и у для того что б запоминать координаты куда мы хотим двигаться
	int currDirection; //текущее направление
	StatePlayer state; //состояние игрока соотвествует фигуре
private:
	void EFF_reduce (){ //уменьшение игрока (конец уровня)
		shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
		shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
		shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
		shape.setRotation (rotation);

		rotation += speedChangeSt * time * 1.5f;
		reducePrecent -= speedChangeSt * time / 3; //когда прекратится изменение формы 
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
	Player (Image &image, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (image, "Player", X, Y, W, H, WTexture, HTexture){ //конструктор без имени
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

	void EFF_enlarge (){ //увелечение игрока (вначале игры)
		if (F_enlarge){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
			shape.setRotation (rotation);

			rotation += (float) (speedChangeSt * time * 1.5);
			enlargePrecent += speedChangeSt * time / 3; //когда прекратится изменение формы
			if (enlargePrecent > 100 - speedChangeSt * time / 3){
				enlargePrecent = 1; F_enlarge = false;
				shape.setSize (Vector2f ((float) w, (float) h));
				shape.setOrigin ((float) w / 2, (float) h / 2);
				shape.setPosition ((float) GLOBAL_W / 2, (float) GLOBAL_H / 2);
				shape.setRotation (0); rotation = 0;
			}
		}
	}

	void changeFigureKey (){ //изменение фигуры по нажатию клавиши
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

	void changeFigureStatic (){ //изменение фигуры соотвествующему состоянию игрока
		if (state == rectangle)     shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		else if (state == triangle) shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		else if (state == circle)   shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
	}

	void update (){
		if (x == Finish.x && y == Finish.y){ //есди мы достигли финиша, то будет показана кнопка, свидетельствующая об этом
			EFF_reduce (); F_transformation = false; 
			F_secPhaseTransformation = false; F_reduce = true;
		}
		else if (!F_teleportation){
			if (currDirection < NumMoves && !F_move){
				if (Direction [currDirection] == 4)        tmpY = y - EDGE; //запоминаем координаты куда мы должы придти
				else if (Direction [currDirection] == 1)   tmpY = y + EDGE; //запоминаем координаты куда мы должы придти
				else if (Direction [currDirection] == 2)   tmpX = x - EDGE; //запоминаем координаты куда мы должы придти 
				else if (Direction [currDirection] == 3)   tmpX = x + EDGE; //запоминаем координаты куда мы должы придти
				currDirection++; F_move = true;
				xx = (float) x; yy = (float) y;
			}

			changeFigureKey ();

			if (F_move){ //проверяем, нет ли стены на том месте куда мы хотим перейти
				//cout << xx << "-xx   " << yy << "-yy\n" << tmpX << "-tmpX   " << tmpY << "-tmpY\n" << speed * time << "-speed * time\n" << abs (xx - (float) tmpX) << "-xx - tmpX   " << abs (yy - (float) tmpY) << "-yy - tmpY\n" << endl;

				if (abs (xx - (float) tmpX) <= speed * time && abs (yy - (float) tmpY) <= speed * time){ //по разности понимаем когда игрок достиг следующей клетки, округляем координаты и дальше движемся
					F_move = false; 
					xx = (float) tmpX; yy = (float) tmpY;
					x = tmpX; y = tmpY;
				}
				else{ //само движение игрока
					if (x < tmpX)        xx += speed * time; //движение по горизонтали
					else if (x > tmpX)   xx -= speed * time; 
					else if (y < tmpY)   yy += speed * time; //движениепо вертикали
					else if (y > tmpY)   yy -= speed * time;  
					else{
						yy = (float) tmpY; xx = (float) tmpX; //мб ситуация когда у = tmpY, а yy - tmpY больше чем speed * time, тогда
						//происходит зависание игрока, пока не будет такое время, что speed * time будет больше
					}
					x = (int) xx; y = (int) yy;
				}
			}
		}
	}

	void changeLocation (int x2, int y2){ //функция нужна для перемещения игрока в нужную координату (нужно при открытии уровня игркоом)
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

	void EFF_teleportation (int x2, int y2){ //функция нужна для перемещения игрока в нужную координату (нужно при открытии уровня игркоом)
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

class BodyButton : public Body{ //тело кнопок
protected:
	Color color; //цвет текста кнопки
	mcText *text; //текст который выводится на кнопке
	String buttText; //текст который будет отображаться на кнопке
private:
	Font font; //шрифт
public:
	bool F_draw; //флаг, который показывает рисуется ли эта кнопка
	bool F_pressed; //флаг, который показывает нажата ли кнопка
	bool F_click; //флаг, который показывает кликнули ли по кнопке (клик- это нажать и отпустить кнопку когда курсор мыши на кнопке)
	bool F_transformation; //флаг показывающий изменилась ли форма
	float reducePrecent; //процент уменьшения
	float enlargePrecent; //процент увелечения
	int value; //значение кнопки
	TypeState type;
	SubtypeState subtype;
public:
	BodyButton (Image &image, String Text, String Name, Font &Font, SubtypeState &Subtype, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    Body (image, Name, X, Y, W, H, WTexture, HTexture){
	    font = Font; buttText = Text; 
		subtype = Subtype; type = findType (subtype); 
		F_draw = false; F_click = false; F_pressed = false; 
		F_transformation = false; reducePrecent = 100; enlargePrecent = 1;

		color = Color::Black; //по умолчанию текст черный
		text = new mcText (&font); //создаем текст который будет отображаться на кнопке
		text -> changeSize (SIZE_TEXT); //размер текста
		text -> add (buttText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	BodyButton (Image &image, String Name, SubtypeState &Subtype, int X, int Y, int W, int H, int WTexture, int HTexture) : //для пикчер баттон, чек баттон и горизонтал скрол бар
		    Body (image, Name, X, Y, W, H, WTexture, HTexture){
		subtype = Subtype; type = findType (subtype); 
		F_draw = false; F_click = false; F_pressed = false; 
		F_transformation = false; reducePrecent = 100; enlargePrecent = 1;

		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	BodyButton (String Text, String Name, Font &Font, SubtypeState &Subtype, int X, int Y) : //для статика сделана перегрузка
		    Body (Name, X, Y, 1, 1, 1, 1){
	    font = Font; buttText = Text;
		subtype = Subtype; type = findType (subtype);
		F_draw = false; F_click = false; F_pressed = false; 
		F_transformation = false; reducePrecent = 100; enlargePrecent = 1;

		color = Color::Black;
		text = new mcText (&font); //создаем текст который будет отображаться на кнопке
		text -> changeSize (SIZE_TEXT); //размер текста
		text -> add (buttText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
	}

	void draw (){ }

	virtual void checkCursor () = 0;

	virtual void updateText (char *Pass) = 0;

	virtual void EFF_reduce (){ //уменьшение кнопки
		if (F_transformation){
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
				F_transformation = false;
			}
		}
	}

	virtual void EFF_enlarge (){ //увелечение кнопки
		if (F_transformation){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) x, (float) y);
			text -> clear ();
			text -> changeSize (SIZE_TEXT * (int) enlargePrecent / 100); //размер текста
			text -> add (buttText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT * enlargePrecent / 100 / 3); //распологаем текст по кнопке

			enlargePrecent += speedChangeSt * time; //когда прекратится изменение формы
			if (enlargePrecent > 100 - speedChangeSt * time){
				enlargePrecent = 1;
				F_transformation = false;

				shape.setSize (Vector2f ((float) w, (float) h));
				shape.setOrigin ((float) w / 2, (float) h / 2);
				shape.setPosition ((float) x, (float) y);
				text -> clear ();
				text -> changeSize (SIZE_TEXT); //размер текста
				text -> add (buttText, color);
				text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * SIZE_TEXT / 3); //распологаем текст по кнопке
			}
		}
	}

	virtual void clear (){ //функция очищения кнопки, нужна что б начать кнопки увеличивать (вторая фаза)
		shape.setSize (Vector2f (1, 1));
		text -> clear ();
	}
};