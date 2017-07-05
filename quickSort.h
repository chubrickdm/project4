#pragma once
#include <iostream>
#include "general classes.h"
using namespace std;

void quickSort (int l, int r, Wall *ArrWall [4000]){
		int i = l, j = r;
		Wall *tmp;
		tmp = ArrWall [(i + j) / 2];

		while (i <= j){ //сортировка по двум признакам, по координатам
			while (ArrWall [i] -> x < tmp -> x || (ArrWall [i] -> x == tmp -> x && ArrWall [i] -> y < tmp -> y))
				i++;
			while (ArrWall [j] -> x > tmp -> x || (ArrWall [j] -> x == tmp -> x && ArrWall [j] -> y > tmp -> y))
				j--;
			if (i <= j){
				Wall *tmp2 = ArrWall [i];
				ArrWall [i] = ArrWall [j];
				ArrWall [j] = tmp2;
				i++; j--;
			}
		}

		//while (i <= j){ //сортировка по одному признаку
		//	while (ArrWall [i] -> x < tmp -> x)
		//		i++;
		//	while (ArrWall [j] -> x > tmp -> x)
		//		j--;
		//	if (i <= j){
		//		Wall *tmp2 = ArrWall [i];
		//		ArrWall [i] = ArrWall [j];
		//		ArrWall [j] = tmp2;
		//		i++; j--;
		//	}
		//}

		if (l < j) //рекурсивно вызываем
			quickSort (l, j, ArrWall);
		if (i < r)
			quickSort (i, r, ArrWall);
	}
