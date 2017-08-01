#pragma once
#include <SFML/Graphics.hpp> //�������� ���������� ��� ������ � �������� � ������������
#include <SFML/Audio.hpp> //��� ������ �� ������ � �������
#include <Windows.h> //����� ��� � ������ ���������� ������ ������
using namespace std;
using namespace sf;


enum TypeState {unknown = -1, menu = 0, player, admin}; //���� ����
enum SubtypeState {launcher = 0, mode, settings, exitt, selectLVL, audioSetting, controlSeting, playerLVL, about, //0-8 //������� ����
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


class System{ //�������� ����� ����, � ������� �������� ��� ����� ������
public:
	static bool         F_musicOff; //����, ������� ���������� ��������� �� ������
	static bool         F_soundOff; //����, ������� ���������� ��������� �� �����
	static bool         F_lvlComplete; //���������� �������� �������
	static bool         F_showMessage; //����, ������� ���������� ����� �� ���������
	static bool         F_changeStates; //����, ������� ����������, �������� �� ��������� � ������ ������

	static float        speedChangeSt; //�������� ��������� ������� ������ ��� ��������� ���������
	static float        speed; //�������� � ������� �������� ����� �� ������
	static float        timer; //������
	static float        time; //�����
	static float        volumeMusic; //���������� ������� ������
	static float        volumeSound; //��������� �����

	static int          FPS; //�������� ���
	static int          keyCodePressed; //����� ������� ���� ������
	static int          key [3]; //������ � ������� �������� ������ ������, ������� ������ ������
	static int          whatButChange; //��� ������� �������, ����� ������ ������� ��� �������� (����� ����� ����������� ������� ��������� ������)
	static int          PassedLVL; //������� �������� �������
	static int*         levelArray; //������ � ������� �������� �����

	static RenderWindow *window; //����, � ������� ����������� ����
	static Event        event; //�������
	static Clock        clock; //�����
	static Sound        sndTeleport; //���� ��������� ������ � ����������
	static Sound        sndClickButt; //���� ������� �� ������
	static Music        backgroundMusic; //������� ������
	static SoundBuffer  bufferClickButt; //����� ��� ����� ������� �� ������
	static SoundBuffer  bufferTeleport; //����� ��� ����� ��������� ������ � ����������
	static String       AdOrPlMode; //������ �������� ��� �������� ���� ���� (����� ��� �����)
	static Vector2i     mousePosWin; //���������� ���� �����. ����
	static Vector2f     posMouse; //���������� ���� �����. �����
	static TileMap      map; //�����
	static CreateWall   whichWall; //����� ����� ������� ������� � ������ ������
	static Coordinate   Start; //���������� ������ (������ ����� ��������) 
	static Coordinate   Finish; //���������� ������ (���� ������ ������)
	

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
		W_WIN = GetSystemMetrics (0); H_WIN = GetSystemMetrics (1); //����������
		//W_WIN = 1366; H_WIN = 768; //����������
		//W_WIN = 1280; H_WIN = 1024; //����������
		EDGE = 5; //������ ����� ������
		NUM_CELL_X = 64; //���������� ������ ������ �� ������
		NUM_CELL_Y = 32; //���������� ������ ������ �� ������
		W_BUTTON = (int) (W_WIN / 8); //������ ������
		H_BUTTON = H_WIN / 19; //������ ������
		INTERVAL = H_BUTTON / 10;
		SIZE_TEXT = (int) 30 * H_BUTTON / 44;

		while (1){
			if ((H_WIN - NUM_CELL_Y * EDGE) / 2 < H_BUTTON * 2 || (W_WIN - NUM_CELL_X * EDGE) / 2 < 60)
				break;
			EDGE++;
		}

		NUM_SQUARE = 8; //���������� ��������� ������� �� ������ ����� ������ �����
		SQUARE = EDGE * NUM_CELL_Y / NUM_SQUARE; //������ ������ ������ ��������
		speed = (float) 3 * SQUARE; //������� ����� ������� �� 1 �������
		speedChangeSt = 200; //�� ������� ��������� ���������� ������ �� 1 �������
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