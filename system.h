#pragma once
#include <SFML/Graphics.hpp> //основная библиотека для работы с графикой и изображением
#include <SFML/Audio.hpp> //для работы со звуком и музыкой
#include <Windows.h> //нужна что б узнать разрешение экрана игрока
using namespace std;
using namespace sf;


enum TypeState {unknown = -1, menu = 0, player, admin}; //типы игры
enum SubtypeState {launcher = 0, mode, settings, exitt, selectLVL, audioSetting, controlSeting, playerLVL, about, //0-8 //подтипы игры
	               editLVL,  openLVL, saveLVL, deleteLVL, listLVL, // 9-13
	               play, pause, startLVL, completeLVL,  //14-17
	               wholeType, allState, loadingLVL}; //18-20
enum StatePlayer {rectangle, triangle, circle};
enum CreateWall {rectangleW, triangleW, circleW, wall, finishW, startW, saveW};


class TileMap : public Drawable, public Transformable{
private:
	VertexArray m_vertices;
	Texture m_tileset;
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
        states.transform *= getTransform ();
        states.texture = &m_tileset;
        target.draw (m_vertices, states);
    }
public:
    bool load(const string &tileset, Vector2u tileSize, const int *tiles, unsigned int width, unsigned int height, Vector2i size){
        if (!m_tileset.loadFromFile (tileset))
            return false;

        m_vertices.setPrimitiveType (Quads);
        m_vertices.resize (width * height * 4);

        for (unsigned int i = 0; i < width; ++i)
            for (unsigned int j = 0; j < height; ++j){
                // get the current tile number
                int tileNumber = tiles [i + j * width];

                // find its position in the tileset texture
                int tu = tileNumber / (m_tileset.getSize ().y / tileSize.y);
                int tv = tileNumber % (m_tileset.getSize ().y / tileSize.y);

                // get a pointer to the current tile's quad
                sf::Vertex* quad = &m_vertices [(i + j * width) * 4];

                // define its 4 corners
                quad [0].position = Vector2f ((float) (i * size.x),       (float) (j * size.y));
                quad [1].position = Vector2f ((float) ((i + 1) * size.x), (float) (j * size.y));
                quad [2].position = Vector2f ((float) ((i + 1) * size.x), (float) ((j + 1) * size.y));
                quad [3].position = Vector2f ((float) (i * size.x),       (float) ((j + 1) * size.y));

                // define its 4 texture coordinates
                quad [0].texCoords = Vector2f ((float) (tu * tileSize.x),       (float) (tv * tileSize.y));
                quad [1].texCoords = Vector2f ((float) ((tu + 1) * tileSize.x), (float) (tv * tileSize.y));
                quad [2].texCoords = Vector2f ((float) ((tu + 1) * tileSize.x), (float) ((tv + 1) * tileSize.y));
                quad [3].texCoords = Vector2f ((float) (tu * tileSize.x),       (float) ((tv + 1) * tileSize.y));
            }
        return true;
    }
};


class System{ //основной класс игры, в котором хранится все самое выжное
public:
	static bool         F_musicOff; //флаг, который показывает выключена ли музыка
	static bool         F_soundOff; //флаг, который показывает выключены ли звуки
	static bool         F_lvlComplete; //показывает завершен уровень
	static bool         F_showMessage; //флаг, который показывает видно ли сообщение
	static bool         F_changeStates; //флаг, который показывает, меняется ли состояние в данный момент

	static float        speedChangeSt; //скорость изменения размера кнопок при изменении состояния
	static float        speed; //скорость с которой движется игрок по уровню
	static float        timer; //таймер
	static float        time; //время
	static float        volumeMusic; //громокость фоновой музыки
	static float        volumeSound; //громкость звука

	static int          FPS; //значение ФПС
	static int          keyCodePressed; //какая клавиша была нажата
	static int          key [3]; //массив в котором хранятся номера клавиш, которые меняют фигуру
	static int          whatButChange; //при нажатии клавиши, какая кнопка изменит своё значение (когда игрок настраивает клавиши изменения фигуры)
	static int          PassedLVL; //сколько пройдено уровней
	static int*         levelArray; //массив в котором хранится карта

	static RenderWindow *window; //окно, в котором запускается игра
	static Event        event; //событие
	static Clock        clock; //время
	static Sound        sndTeleport; //звук телепорта игрока к сохранению
	static Sound        sndClickButt; //звук нажатия на кнопку
	static Music        backgroundMusic; //фоновая музыка
	static SoundBuffer  bufferClickButt; //буфер для звука нажатия на кнопки
	static SoundBuffer  bufferTeleport; //буфер для звука телепорта игрока к сохранению
	static String       AdOrPlMode; //строка хранящая имя текущего мода игры (игрок или админ)
	static Vector2i     mousePosWin; //координаты мыши относ. окна
	static Vector2f     posMouse; //координаты мыши относ. карты
	static TileMap      map; //карта
	static CreateWall   whichWall; //какая стена выбрана админом в данный момент
	static Coordinate   Start; //координаты старта (откуда игрок стартует) 
	static Coordinate   Finish; //координаты финиша (куда должен придти)
	

	int W_WIN;
	int INTERVAL;
	int H_WIN;
	int EDGE;
	int NUM_CELL_X; 
	int NUM_CELL_Y;
	int W_BUTTON;
	int H_BUTTON;
	int SIZE_TEXT;
	int NUM_SQUARE;
	int SQUARE;
public:
	System (){
		W_WIN = GetSystemMetrics (0); H_WIN = GetSystemMetrics (1); //разрешение
		//W_WIN = 1366; H_WIN = 768; //разрешение
		//W_WIN = 1280; H_WIN = 1024; //разрешение
		EDGE = 5; //размер одной клетки
		NUM_CELL_X = 64; //количество клеток уровня по ширине
		NUM_CELL_Y = 32; //количество клеток уровня по высоте
		W_BUTTON = (int) (W_WIN / 8); //ширина кнопки
		H_BUTTON = H_WIN / 19; //высота кнопки
		INTERVAL = H_BUTTON / 10;
		SIZE_TEXT = (int) 30 * H_BUTTON / 44;

		while (1){
			if ((H_WIN - NUM_CELL_Y * EDGE) / 2 < H_BUTTON * 2 || (W_WIN - NUM_CELL_X * EDGE) / 2 < 60)
				break;
			EDGE++;
		}

		NUM_SQUARE = 8; //количество квадратов видемых на экране когда играет игрок
		SQUARE = EDGE * NUM_CELL_Y / NUM_SQUARE; //размер одного такого квадрата
		speed = (float) 3 * SQUARE; //сколько игрок пройдет за 1 секунду
		speedChangeSt = 200; //на сколько процентов уменьшится кнопка за 1 секунду
	}

	TypeState findType (SubtypeState tmpS){
		if (tmpS > -1 && tmpS <= 8)       return menu;
		else if (tmpS > 8 && tmpS <= 13)  return admin;
		else if (tmpS > 13 && tmpS <= 17) return player;
		else                              return unknown;
	}
protected:
	void changeBool (bool &tmpB){
		if (tmpB) tmpB = false;
		else      tmpB = true;
	}

	void changeMusicVolume (float tmpV){
		backgroundMusic.setVolume (tmpV);
	}

	void changeSoundVolume (float tmpV){
		sndClickButt.setVolume (tmpV);
		sndTeleport.setVolume (tmpV);
	}
};