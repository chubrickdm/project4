#pragma once
#include "searchWay.h"
#include "general classes.h"
#include <iostream>
using namespace std;

int binSearch (int left, int right, Wall *ArrWall [4000], Coordinate key){
		int midd = 0;
		while (1){
			midd = (left + right) / 2; 
		
			if (key.x < ArrWall [midd] -> x || (key.x == ArrWall [midd] -> x && key.y < ArrWall [midd] -> y)) //если искомое меньше значения в ячейке
				right = midd - 1; //смещаем правую границу поиска
			else if (key.x > ArrWall [midd] -> x || (key.x == ArrWall [midd] -> x && key.y > ArrWall [midd] -> y)) //если искомое больше значения в ячейке
				left = midd + 1; //смещаем левую границу поиска
			else      
				return midd; //иначе (значения равны)

			if (left > right) //если границы сомкнулись 
				return -1;
		}
	}