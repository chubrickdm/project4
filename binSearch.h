#pragma once
#include "searchWay.h"
#include "general classes.h"
#include <iostream>
using namespace std;

int binSearch (int left, int right, Wall *ArrWall [4000], Coordinate key){
		int midd = 0;
		while (1){
			midd = (left + right) / 2; 
		
			if (key.x < ArrWall [midd] -> x || (key.x == ArrWall [midd] -> x && key.y < ArrWall [midd] -> y)) //���� ������� ������ �������� � ������
				right = midd - 1; //������� ������ ������� ������
			else if (key.x > ArrWall [midd] -> x || (key.x == ArrWall [midd] -> x && key.y > ArrWall [midd] -> y)) //���� ������� ������ �������� � ������
				left = midd + 1; //������� ����� ������� ������
			else      
				return midd; //����� (�������� �����)

			if (left > right) //���� ������� ���������� 
				return -1;
		}
	}