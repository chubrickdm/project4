#pragma once
#include <SFML/Graphics.hpp> //основная библиотека для работы с графикой и изображением
#include <SFML/Audio.hpp> //для работы со звуком и музыкой
#include <Windows.h> //нужна что б узнать разрешение экрана игрока
using namespace std;
using namespace sf;


enum TypeState {unknown = -1, menu = 0, player, admin}; //типы игры
enum SubtypeState {launcher = 0, mode, settings, exitt, selectLVL, audioSetting, controlSeting, playerLVL, //0-7 //подтипы игры
	               editLVL,  openLVL, saveLVL, deleteLVL, listLVL, // 8-12
	               play, pause, startLVL, completeLVL,  //13-16
	               wholeType, allState, loadingLVL}; //17-19
enum StatePlayer {rectangle, triangle, circle};
enum CreateWall {rectangleW, triangleW, circleW, wall, finishW, startW, saveW};


class System{ //основной класс игры, в котором хранится все самое выжное
public:
	static Vector2i     mousePosWin; //координаты мыши относ. окна
	static Vector2f     posMouse; //координаты мыши относ. карты
	static RenderWindow *window; //окно, в котором запускается игра
	static Event        event; //событие

	static float        timer; //таймер
	static float        time; //время
	static int          FPS; //значение ФПС
	static Clock        clock; //время
	
	static float        volumeMusic; //громокость фоновой музыки
	static float        volumeSound; //громкость звука
	static Music        backgroundMusic; //фоновая музыка
	static SoundBuffer  bufferClickButt; //буфер для звука нажатия на кнопки
	static Sound        sndClickButt; //звук нажатия на кнопку
	static SoundBuffer  bufferTeleport; //буфер для звука телепорта игрока к сохранению
	static Sound        sndTeleport; //звук телепорта игрока к сохранению
	
	static bool         F_lvlComplete; //показывает завершен уровень
	static float        speedChangeSt; //скорость изменения размера кнопок при изменении состояния
	static float        speed; //скорость с которой движется игрок по уровню
	static int          keyCodePressed; //какая клавиша была нажата
	static int          key [3]; //массив в котором хранятся номера клавиш, которые меняют фигуру
	static int          whatButChange; //при нажатии клавиши, какая кнопка изменит своё значение (когда игрок настраивает клавиши изменения фигуры)
	static int          PassedLVL; //сколько пройдено уровней
	static CreateWall   whichWall; //какая стена выбрана админом в данный момент
	static String       AdOrPlMode; //строка хранящая имя текущего мода игры (игрок или админ)
	static Coordinate   Start; //координаты старта (откуда игрок стартует) 
	static Coordinate   Finish; //координаты финиша (куда должен придти)
	
	
	int GLOBAL_W;
	int GLOBAL_H;
	int W_WIN;
	int INTERVAL;
	int H_WIN;
	int EDGE;
	int NUM_CELL_X; 
	int NUM_CELL_Y;
	int W_BUTTON;
	int H_BUTTON;
	int GLOB_IND_W; 
	int GLOB_IND_H;
	int SIZE_TEXT;
	int NUM_SQUARE;
	int SQUARE;
public:
	System (){
		GLOBAL_W = 4000; //2240 //максимальное разрешение экрана в котором игра пойдет, ширина
		GLOBAL_H = 3000; //1280 //высота
		W_WIN = GetSystemMetrics (0); H_WIN = GetSystemMetrics (1); //разрешение
		//W_WIN = 1366; H_WIN = 768; //разрешение
		EDGE = 10; //размер одной клетки
		NUM_CELL_X = 64; //количество клеток уровня по ширине
		NUM_CELL_Y = 32; //количество клеток уровня по высоте
		W_BUTTON = (int) (W_WIN / 9.6); //ширина кнопки
		H_BUTTON = H_WIN / 19; //высота кнопки
		INTERVAL = H_BUTTON / 10;
		SIZE_TEXT = (int) 30 * H_BUTTON / 44;

		while (1){
			if ((H_WIN - NUM_CELL_Y * EDGE) / 2 < H_BUTTON + 40 || (W_WIN - NUM_CELL_X * EDGE) / 2 < 60)
				break;
			EDGE++;
		}
		NUM_SQUARE = 8; //количество квадратов видемых на экране когда играет игрок
		SQUARE = EDGE * NUM_CELL_Y / NUM_SQUARE; //размер одного такого квадрата

		GLOB_IND_W = (GLOBAL_W - NUM_CELL_X * EDGE) / 2; //отступ по ширине, с которого начинается область которую видит игрок
		GLOB_IND_H = (GLOBAL_H - NUM_CELL_Y * EDGE) / 2; //отступ по высоте, с которого начинается область которую видит игрок

		speed = (float) 3 * EDGE; //сколько игрок пройдет за 1 секунду
		speedChangeSt = 200; //на сколько процентов уменьшится кнопка за 1 секунду
	}

	TypeState findType (SubtypeState tmpS){
		if (tmpS > -1 && tmpS <= 7)       return menu;
		else if (tmpS > 7 && tmpS <= 12)  return admin;
		else if (tmpS > 12 && tmpS <= 16) return player;
		else                              return unknown;
	}
protected:
	void changeBool (bool &tmpB){
		if (tmpB) tmpB = false;
		else      tmpB = true;
	}

	void updateVolumeMusic (float tmpV){
		volumeMusic = tmpV;
		backgroundMusic.setVolume (tmpV);
	}
};