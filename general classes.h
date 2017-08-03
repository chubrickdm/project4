#pragma once
#include "body.h"

class Wall : public Body{ //класс стены
public:
	Wall (Texture *tmpTexture, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (tmpTexture, Name, X, Y, W, H, WTexture, HTexture){ //конструктор с именем
		int GLOB_IND_W = (W_WIN - NUM_CELL_X * EDGE) / 2; //отступ по ширине, с которого начинается уровень
		int GLOB_IND_H = (H_WIN - NUM_CELL_Y * EDGE) / 2; //отступ по высоте, с которого начинается уровень
		shape.setPosition ((float) x * EDGE + GLOB_IND_W, (float) y * EDGE + GLOB_IND_H);
		if (name == "Save")            shape.setTextureRect (IntRect (0, 0,            wTexture, hTexture)); //определяем текстуру для каждого вида стены
		else if (name == "Wall")       shape.setTextureRect (IntRect (0, hTexture,     wTexture, hTexture));
		else if (name == "Finish")     shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		else if (name == "Start")      shape.setTextureRect (IntRect (0, hTexture * 3, wTexture, hTexture));
		else if (name == "Circle")     shape.setTextureRect (IntRect (0, hTexture * 4, wTexture, hTexture));
		else if (name == "Rectangle")  shape.setTextureRect (IntRect (0, hTexture * 5, wTexture, hTexture));
		else if (name == "Triangle")   shape.setTextureRect (IntRect (0, hTexture * 6, wTexture, hTexture));
	}

	Wall& operator= (const Wall& tmpW){ //в функции класса Game есть функция createWall, там нужен оператор равно
		if (this != &tmpW){
			x = tmpW.x; y = tmpW.y;
			w = tmpW.w; h = tmpW.h;  
			name = tmpW.name; 
			wTexture = tmpW.wTexture; hTexture = tmpW.hTexture;
			int GLOB_IND_W = (W_WIN - NUM_CELL_X * EDGE) / 2; //отступ по ширине, с которого начинается уровень
			int GLOB_IND_H = (H_WIN - NUM_CELL_Y * EDGE) / 2; //отступ по высоте, с которого начинается уровень
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
	Background (Texture *tmpTexture, String Name, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (tmpTexture, Name, X, Y, W, H, WTexture, HTexture){ 
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	void changeLocation (int x2, int y2){ //функция изменения координат
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
	int dir; //направление, в котором движется игрок в данный момент
	StatePlayer stateWill; //хранит состояние в которое трансформируется игрок
public:
	bool F_move; //флаг, который показывает движется ли игрок
	bool F_teleportation; //флаг, который показывает игрок телепортируется ли
	bool F_reduce; //флаг, который показывает уменьшается ли игрок в данный момент
	bool F_enlarge; //флаг, который показывает идет ли эффект начала игры
	int currDirection; //текущее направление
	StatePlayer state; //состояние игрока соотвествует фигуре
private:
	void EFF_reduce (){ //уменьшение игрока (конец уровня)
		shape.setSize (Vector2f ((float) w * reducePrecent / 100, (float) h * reducePrecent / 100));
		shape.setOrigin ((float) w * reducePrecent / 100 / 2, (float) h * reducePrecent / 100 / 2);
		shape.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
		shape.setRotation (rotation);

		rotation += speedChangeSt * time * 1.5f;
		reducePrecent -= speedChangeSt * time / 3;
		if (reducePrecent < speedChangeSt * time / 3){ //когда прекратится изменение формы 
			reducePrecent = 100; F_lvlComplete = true; //тогда и уровень закончится
			rotation = 0; F_reduce = false;
		}
	}

	void EFF_transformation (){ //эффект изменения фигуры игрока
		if (F_secPhaseTransformation){ //увелечение игрока
			shape.setSize (Vector2f ((float) w * timer / 0.15f, (float) h * timer / 0.15f));
			shape.setOrigin ((float) w * timer / 0.15f / 2, (float) h * timer / 0.15f / 2);
			shape.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
		}
		else{ //уменьшение игрока
			shape.setSize (Vector2f ((float) w * (1 - timer / 0.15f), (float) h * (1 - timer / 0.15f)));
			shape.setOrigin ((float) w * (1 - timer / 0.15f) / 2, (float) h * (1 - timer / 0.15f) / 2);
			shape.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
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
			F_transformation = false; //он может вконце иметь не полный размер, поэтому приводим его к 100% размеру
			shape.setSize (Vector2f ((float) w, (float) h));
			shape.setOrigin ((float) w / 2, (float) h / 2);
			shape.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
		}
	}
public:
	Player (Texture *tmpTexture, int X, int Y, int W, int H, int WTexture, int HTexture) : Body (tmpTexture, "Player", X, Y, W, H, WTexture, HTexture){ //конструктор без имени
		F_move = false; F_reduce = false;
		F_teleportation = false; F_transformation = false; F_secPhaseTransformation = false;
		reducePrecent = 100; rotation = 0; enlargePrecent = 1; F_enlarge = false;

		currDirection = 0; state = rectangle; 
		shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		shape.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	void EFF_enlarge (){ //увелечение игрока (вначале игры)
		if (F_enlarge){
			shape.setSize (Vector2f ((float) w * enlargePrecent / 100, (float) h * enlargePrecent / 100));
			shape.setOrigin ((float) w * enlargePrecent / 100 / 2, (float) h * enlargePrecent / 100 / 2);
			shape.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
			shape.setRotation (rotation);

			rotation += (float) (speedChangeSt * time * 1.5);
			enlargePrecent += speedChangeSt * time / 3;
			if (enlargePrecent > 100 - speedChangeSt * time / 3){ //когда прекратится изменение формы
				enlargePrecent = 1; F_enlarge = false;
				shape.setSize (Vector2f ((float) w, (float) h));
				shape.setOrigin ((float) w / 2, (float) h / 2);
				shape.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
				shape.setRotation (0); rotation = 0;
			}
		}
	}

	void changeFigureKey (){ //изменение фигуры по нажатию клавиши
		if (!F_secPhaseTransformation){ //пока идет уменьшение игрока, можно поменять фигуру в которую игрок будет увеличиваться
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

		if (F_transformation){ //если трансформируется
			EFF_transformation (); timer += time;
		}
	}

	void changeFigureStatic (){ //изменение фигуры соотвествующему состоянию игрока
		if (state == rectangle)     shape.setTextureRect (IntRect (0, hTexture, wTexture, hTexture));
		else if (state == triangle) shape.setTextureRect (IntRect (0, hTexture * 2, wTexture, hTexture));
		else if (state == circle)   shape.setTextureRect (IntRect (0, 0, wTexture, hTexture));
	}

	void update (){
		if (!F_teleportation){
			if (currDirection < NumMoves && !F_move){ //в массиве хранятся направления, в которе должен двигаться игрок, что б достичь финиша
				if (Direction [currDirection] == 4)      { y--; dir = 4; } //х и у хранят координаты на уровне! т.е. х от 0 до 31, у от 0 до 63
				else if (Direction [currDirection] == 1) { y++; dir = 1; }
				else if (Direction [currDirection] == 2) { x--; dir = 2; }
				else if (Direction [currDirection] == 3) { x++; dir = 3; }
				currDirection++; F_move = true;
			}

			changeFigureKey ();

			if (F_move){ //начало координат в левом верхнем углу в SFML, а про движение игрока написано для глаз игрока
				if (dir == 3){ //движение карты влево, а будет выглядеть как игрок движется вправо
					map.move (-speed * time, 0); 
					if (map.getPosition ().x - (W_WIN / 2 - SQUARE) < 3 * SQUARE * time || map.getPosition ().x < W_WIN / 2 - SQUARE){
						map.setOrigin ((float) SQUARE * x + 3 * SQUARE / 2, (float) SQUARE * y + 3 * SQUARE / 2);
						map.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
						F_move = false;
					}
				}
				else if (dir == 2){ //движение карты вправо, а будет выглядеть как игрок движется на лево
					map.move (speed * time, 0); 
					if ((W_WIN / 2 + SQUARE - map.getPosition ().x < 3 * SQUARE * time) || map.getPosition ().x > W_WIN / 2 + SQUARE){
						map.setOrigin ((float) SQUARE * x + 3 * SQUARE / 2, (float) SQUARE * y + 3 * SQUARE / 2);
						map.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
						F_move = false;
					}
				}

				else if (dir == 1){ //движение игрока вниз
					map.move (0, -speed * time); 
					if ((map.getPosition ().y - (H_WIN / 2 - SQUARE) < 3 * SQUARE * time) || map.getPosition ().y < H_WIN / 2 - SQUARE){
						map.setOrigin ((float) SQUARE * x + 3 * SQUARE / 2, (float) SQUARE * y + 3 * SQUARE / 2);
						map.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
						F_move = false;
					}
				}
				else if (dir == 4){ //движение игрок вверх
					map.move (0, speed * time); 
					if ((H_WIN / 2 + SQUARE - map.getPosition ().y < 3 * SQUARE * time) || map.getPosition ().y > H_WIN / 2 + SQUARE){
						map.setOrigin ((float) SQUARE * x + 3 * SQUARE / 2, (float) SQUARE * y + 3 * SQUARE / 2);
						map.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
						F_move = false;
					} 
				}
			}
		}
		if (x == Finish.x && y == Finish.y && !F_move){ //есди мы достигли финиша
			EFF_reduce (); F_transformation = false; //начинаем уменьшение
			F_secPhaseTransformation = false; F_reduce = true;
		}
	}

	void changeLocation (int x2, int y2){ //функция нужна для перемещения игрока в нужную координату (нужно при открытии уровня игроком)
		shape.setSize (Vector2f ((float) w, (float) h));
		shape.setOrigin ((float) w / 2, (float) h / 2);
		shape.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
		shape.setRotation (0);
		enlargePrecent = 1; reducePrecent = 100;
		rotation = 360 - ((FPS * (100 / speedChangeSt) * 3) * (float) (speedChangeSt * time * 1.5)); //высчитываем угол на который надо повернуть игрока, что б он потом стал в нормальное положение

		x = x2; y = y2; 
		map.setOrigin ((float) SQUARE * x + 3 * SQUARE / 2, (float) SQUARE * y + 3 * SQUARE / 2);
		map.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
	}

	void EFF_teleportation (int x2, int y2){ //эффект телепортации игрока
		shape.setTextureRect (IntRect (0, hTexture * 3, wTexture, hTexture));
		shape.setSize (Vector2f ((float) w * (1 - timer / 0.3f), (float) h * (1 - timer / 0.3f)));
		shape.setOrigin ((float) w * (1 - timer / 0.3f) / 2, (float) h * (1 - timer / 0.3f) / 2);
		shape.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
		F_teleportation = true;
		if (timer > 0.3){
			timer = 0;
			x = x2; y = y2; 

			F_teleportation = false; F_transformation = false; F_secPhaseTransformation = false;
			shape.setSize (Vector2f ((float) w, (float) h)); //доводим его размер до 100%
			shape.setOrigin ((float) w / 2, (float) h / 2);
			shape.setPosition ((float) W_WIN / 2, (float) H_WIN / 2);
			changeFigureStatic ();
		}
	}

	void draw (){ 
		window -> draw (shape);
	}
};

class BodyButton : public Body{ //тело кнопок
protected:
	int sizeText; //размер текста на кнопке
	bool F_pressed; //флаг, который показывает нажата ли кнопка
	float reducePrecent; //процент уменьшения
	float enlargePrecent; //процент увелечения
	Color color; //цвет текста кнопки
	mcText *text; //текст который выводится на кнопке (объект класса, в котором есть удобные методы для вывода текста в окно)
	String buttText; //текст который будет отображаться на кнопке
private:
	Font font; //шрифт
public:
	bool F_draw; //флаг, который показывает рисуется ли эта кнопка
	bool F_click; //флаг, который показывает кликнули ли по кнопке (клик- это нажать и отпустить кнопку когда курсор мыши на кнопке)
	bool F_transformation; //флаг, который показывает изменилась ли форма
	int value; //значение кнопки (нужно для кнопок с цифрами для выбора уровня)
	TypeState type; //тип состояния к которому принадлежит кнопка
	SubtypeState subtype; //подтип состояния к которому принадлежит кнопка
public:
	BodyButton (Texture *tmpTexture, String Text, String Name, Font &Font, SubtypeState &Subtype, int X, int Y, int W, int H, int WTexture, int HTexture) : 
		    Body (tmpTexture, Name, X, Y, W, H, WTexture, HTexture){
	    font = Font; buttText = Text; sizeText = SIZE_TEXT;
		subtype = Subtype; type = findType (subtype); //нашли тип к которому относится кнопка
		F_draw = false; F_click = false; F_pressed = false; 
		F_transformation = false; reducePrecent = 100; enlargePrecent = 1;

		color = Color::Black; //по умолчанию текст черный
		text = new mcText (&font); //создаем текст который будет отображаться на кнопке
		text -> changeSize (sizeText); //размер текста
		text -> add (buttText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * sizeText / 3); //распологаем текст по кнопке
		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	BodyButton (Texture *tmpTexture, String Name, SubtypeState &Subtype, int X, int Y, int W, int H, int WTexture, int HTexture) : //для пикчер баттон, чек баттон и горизонтал скрол бар
		    Body (tmpTexture, Name, X, Y, W, H, WTexture, HTexture){
		subtype = Subtype; type = findType (subtype); 
		F_draw = false; F_click = false; F_pressed = false; 
		F_transformation = false; reducePrecent = 100; enlargePrecent = 1;
		sizeText = SIZE_TEXT;

		shape.setOrigin ((float) w / 2, (float) h / 2);
	}

	BodyButton (String Text, String Name, Font &Font, SubtypeState &Subtype, int X, int Y) : //для статика сделана перегрузка
		    Body (Name, X, Y, 1, 1, 1, 1){
	    font = Font; buttText = Text; sizeText = SIZE_TEXT;
		subtype = Subtype; type = findType (subtype);
		F_draw = false; F_click = false; F_pressed = false; 
		F_transformation = false; reducePrecent = 100; enlargePrecent = 1;

		color = Color::Black;
		text = new mcText (&font); //создаем текст который будет отображаться на кнопке
		text -> changeSize (sizeText); //размер текста
		text -> add (buttText, color);
		text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * sizeText / 3); //распологаем текст по кнопке
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
			text -> changeSize (sizeText * (int) reducePrecent / 100); //размер текста
			text -> add (buttText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * sizeText * reducePrecent / 100 / 3); //распологаем текст по кнопке

			reducePrecent -= speedChangeSt * time;
			if (reducePrecent < speedChangeSt * time){ //когда прекратится изменение формы
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
			text -> changeSize (sizeText * (int) enlargePrecent / 100); //размер текста
			text -> add (buttText, color);
			text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * sizeText * enlargePrecent / 100 / 3); //распологаем текст по кнопке

			enlargePrecent += speedChangeSt * time;
			if (enlargePrecent > 100 - speedChangeSt * time){ //когда прекратится изменение формы
				enlargePrecent = 1;
				F_transformation = false;

				shape.setSize (Vector2f ((float) w, (float) h)); //приводим к 100% размеру
				shape.setOrigin ((float) w / 2, (float) h / 2);
				shape.setPosition ((float) x, (float) y);
				text -> clear ();
				text -> changeSize (sizeText); //размер текста
				text -> add (buttText, color);
				text -> setPosition ((float) x - text -> w / 2, (float) y - 2 * sizeText / 3); //распологаем текст по кнопке
			}
		}
	}

	virtual void clear (){ //функция очищения кнопки, нужна что б начать кнопки увеличивать (вторая фаза)
		shape.setSize (Vector2f (1, 1));
		text -> clear ();
	}
};