//»спользу€ циклическую очередь алгоритм выхода из лабиринта
#include <iostream>
#include <fstream>
using namespace std;

struct Coordinate{
	int y; //строка row
	int x; //столбец col

	friend ostream& operator<<(ostream &os, const Coordinate &tmp){
		os << tmp.x << "  " << tmp.y << endl;
		return os;
	}
	
	/*friend istream& operator>>(istream &is, Cell &tmp){
		is >> tmp.row >> tmp.col;
		return is;
	}*/

	Coordinate& operator= (const Coordinate &tmp){
		x = tmp.x;
		y = tmp.y;
		return *this;
	}

	bool operator!=(const Coordinate &tmp){
		if ((y == tmp.y) && (x == tmp.x))
			return false;
		else
			return true;
	}
};

int NumAnsw = 0;
Coordinate Arr [1000];
int Direction [1000];
int NumMoves = 0;

class Queue{
private:
	Coordinate *C; 
	int size;
	int first, last; //индекс реального первого и последнего элемента 
	int n; //реальное количество элементов в очереди
public:
	Queue (){
		first = -1; last = -1; //очередь пуста€
		n = 0; size = 100;
		C = new Coordinate [size];
	}

	Queue (int tmpSize){
		first = -1; last = -1;
		n = 0; size = tmpSize;
		C = new Coordinate [size];
	}

	Queue (const Queue &tmp){
		if (C != NULL) delete [] C;
		first = tmp.first; last = tmp.last;
		n = tmp.n; size = tmp.size;
		C = new Coordinate [size];
		for (int i = 0; i < size; i++)
			C[i] = tmp.C[i];
	}

	virtual ~Queue (){
		delete [] C;
	}

//--------------------------------------------------------------

	void addLast (const Coordinate &tmp){
		if (n >= size) return;
		if (n == 0){
			first = 0; last = 0;
			C[last] = tmp; n++;
		}
		else{
			last = (last + 1) % size;
			C[last] = tmp; n++;
		}
	}

	Coordinate delFirst (){
		Coordinate tmp;
		if (n == 0){
			tmp.x = -1; 
			tmp.y = -1;
			return tmp;
		}
		if (n == 1){
			tmp = C[first]; n = 0;
			first = -1; last = -1;
		}
		else{
			tmp = C[first]; n--;
			first = (first + 1) % size;
		}
		return tmp;
	}
};

void outputSearch (bool **labyrinth, Coordinate &start, const Coordinate &finish, const Coordinate &size){
	NumAnsw = 0;
	NumMoves = 0;
	int **dir;
	dir = new int* [size.x];
	for (int i = 0; i < size.x; i++){
		dir [i] = new int [size.y]; 
		for (int j = 0; j < size.y; j++)
			dir [i][j] = 0;
	}
	dir [finish.x][finish.y] = 0;
	dir [start.x][start.y] = -3;
	///////////////////////////////////////////////////
	int answer = size.x * size.y; 
	Queue q (answer); answer = 0;
	Coordinate tmpc; //tmpc-
	q.addLast (start);
	tmpc = q.delFirst ();
	while (tmpc != finish){
	    if ((tmpc.y != 0) && (dir [tmpc.x][tmpc.y - 1] == 0) && (!labyrinth [tmpc.x][tmpc.y - 1])){
			dir [tmpc.x][--tmpc.y] = -1;
			q.addLast(tmpc); tmpc.y++;
    	}
    	if ((tmpc.y != size.y - 1) && (dir [tmpc.x][tmpc.y + 1] == 0) && (!labyrinth [tmpc.x][tmpc.y + 1])){
			dir [tmpc.x][++tmpc.y] = 1;
		    q.addLast(tmpc); tmpc.y--;
    	}
    	if ((tmpc.x != 0) && (dir[tmpc.x - 1][tmpc.y] == 0) && (!labyrinth [tmpc.x - 1][tmpc.y])){
			dir [--tmpc.x][tmpc.y] = -2;
			q.addLast(tmpc); tmpc.x++;
    	}
    	if ((tmpc.x != size.x - 1) && (dir [tmpc.x + 1][tmpc.y] == 0) && (!labyrinth [tmpc.x + 1][tmpc.y])){
			dir [++tmpc.x][tmpc.y] = 2;
			q.addLast(tmpc); tmpc.x--;
    	}
		tmpc = q.delFirst (); 
		if ((tmpc.x == -1) && (tmpc.y == -1)){
			answer = -1; tmpc = finish;
		}
	}
    
	if (answer != -1){
    	tmpc = finish;
	    while (tmpc != start){
			if (dir [tmpc.x][tmpc.y] == -1){ Arr [NumAnsw++] = tmpc; tmpc.y++; Direction [NumMoves++] = 1; }
	    	else
				if (dir [tmpc.x][tmpc.y] == 1){ Arr [NumAnsw++] = tmpc; tmpc.y--; Direction [NumMoves++] = 4; }
	    		else
					if (dir [tmpc.x][tmpc.y] == -2){ Arr [NumAnsw++] = tmpc; tmpc.x++; Direction [NumMoves++] = 3; }
		    		else
						if (dir [tmpc.x][tmpc.y] == 2){ Arr [NumAnsw++] = tmpc; tmpc.x--; Direction [NumMoves++] = 2; }
	    	answer++;
	    }
	}
	Arr [NumAnsw++] = start;
}