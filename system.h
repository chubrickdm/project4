#pragma once
#include <SFML/Graphics.hpp> //�������� ���������� ��� ������ � �������� � ������������
#include <SFML/Audio.hpp> //��� ������ �� ������ � �������
#include <Windows.h> //����� ��� � ������ ���������� ������ ������
using namespace std;
using namespace sf;


enum TypeState {unknown = -1, menu = 0, player, admin}; //���� ����
enum SubtypeState {launcher = 0, mode, settings, exitt, selectLVL, audioSetting, controlSeting, playerLVL, //0-7 //������� ����
	               editLVL,  openLVL, saveLVL, deleteLVL, listLVL, // 8-12
	               play, pause, startLVL, completeLVL,  //13-16
	               wholeType, allState, loadingLVL}; //17-19
enum StatePlayer {rectangle, triangle, circle};
enum CreateWall {rectangleW, triangleW, circleW, wall, finishW, startW, saveW};


class System{ //�������� ����� ����, � ������� �������� ��� ����� ������
public:
	static Vector2i     mousePosWin; //���������� ���� �����. ����
	static Vector2f     posMouse; //���������� ���� �����. �����
	static RenderWindow *window; //����, � ������� ����������� ����
	static Event        event; //�������

	static float        timer; //������
	static float        time; //�����
	static int          FPS; //�������� ���
	static Clock        clock; //�����
	
	static float        volumeMusic; //���������� ������� ������
	static float        volumeSound; //��������� �����
	static Music        backgroundMusic; //������� ������
	static SoundBuffer  bufferClickButt; //����� ��� ����� ������� �� ������
	static Sound        sndClickButt; //���� ������� �� ������
	static SoundBuffer  bufferTeleport; //����� ��� ����� ��������� ������ � ����������
	static Sound        sndTeleport; //���� ��������� ������ � ����������
	
	static bool         F_lvlComplete; //���������� �������� �������
	static float        speedChangeSt; //�������� ��������� ������� ������ ��� ��������� ���������
	static float        speed; //�������� � ������� �������� ����� �� ������
	static int          keyCodePressed; //����� ������� ���� ������
	static int          key [3]; //������ � ������� �������� ������ ������, ������� ������ ������
	static int          whatButChange; //��� ������� �������, ����� ������ ������� ��� �������� (����� ����� ����������� ������� ��������� ������)
	static int          PassedLVL; //������� �������� �������
	static CreateWall   whichWall; //����� ����� ������� ������� � ������ ������
	static String       AdOrPlMode; //������ �������� ��� �������� ���� ���� (����� ��� �����)
	static Coordinate   Start; //���������� ������ (������ ����� ��������) 
	static Coordinate   Finish; //���������� ������ (���� ������ ������)
	
	
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
		GLOBAL_W = 4000; //2240 //������������ ���������� ������ � ������� ���� ������, ������
		GLOBAL_H = 3000; //1280 //������
		W_WIN = GetSystemMetrics (0); H_WIN = GetSystemMetrics (1); //����������
		//W_WIN = 1366; H_WIN = 768; //����������
		EDGE = 10; //������ ����� ������
		NUM_CELL_X = 64; //���������� ������ ������ �� ������
		NUM_CELL_Y = 32; //���������� ������ ������ �� ������
		W_BUTTON = (int) (W_WIN / 9.6); //������ ������
		H_BUTTON = H_WIN / 19; //������ ������
		INTERVAL = H_BUTTON / 10;
		SIZE_TEXT = (int) 30 * H_BUTTON / 44;

		while (1){
			if ((H_WIN - NUM_CELL_Y * EDGE) / 2 < H_BUTTON + 40 || (W_WIN - NUM_CELL_X * EDGE) / 2 < 60)
				break;
			EDGE++;
		}
		NUM_SQUARE = 8; //���������� ��������� ������� �� ������ ����� ������ �����
		SQUARE = EDGE * NUM_CELL_Y / NUM_SQUARE; //������ ������ ������ ��������

		GLOB_IND_W = (GLOBAL_W - NUM_CELL_X * EDGE) / 2; //������ �� ������, � �������� ���������� ������� ������� ����� �����
		GLOB_IND_H = (GLOBAL_H - NUM_CELL_Y * EDGE) / 2; //������ �� ������, � �������� ���������� ������� ������� ����� �����

		speed = (float) 3 * EDGE; //������� ����� ������� �� 1 �������
		speedChangeSt = 200; //�� ������� ��������� ���������� ������ �� 1 �������
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