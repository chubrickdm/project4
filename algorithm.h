//»спользу€ циклическую очередь (FIFO) написать алгоритм выхода из лабиринта
#include <iostream>
#include <fstream>
using namespace std;

struct Cell{
	int row; //строка
	int col; //столбец

	friend ostream& operator<<(ostream &os, const Cell &tmp){
		os << tmp.row << "  " << tmp.col << endl;
		return os;
	}
	
	/*friend istream& operator>>(istream &is, Cell &tmp){
		is >> tmp.row >> tmp.col;
		return is;
	}*/

	Cell& operator= (const Cell &tmp){
		row = tmp.row;
		col = tmp.col;
		return *this;
	}

	bool operator!=(const Cell &tmp){
		if ((row == tmp.row) && (col == tmp.col))
			return false;
		else
			return true;
	}
};

//struct QueueFull{ };
//struct QueueEmpty{ };

int NumAnsw = 0;
Cell Arr [100];

class Queue{
private:
	Cell *C; 
	int size;
	int first, last; //индекс реального первого и последнего элемента 
	int n; //реальное количество элементов в очереди
public:
	Queue (){
		first = -1; last = -1; //очередь пуста€
		n = 0; size = 100;
		C = new Cell [size];
	}

	Queue (int tmpSize){
		first = -1; last = -1;
		n = 0; size = tmpSize;
		C = new Cell [size];
	}

	Queue (const Queue &tmp){
		if (C != NULL) delete [] C;
		first = tmp.first; last = tmp.last;
		n = tmp.n; size = tmp.size;
		C = new Cell [size];
		for (int i = 0; i < size; i++)
			C[i] = tmp.C[i];
	}

	virtual ~Queue (){
		delete [] C;
	}

//--------------------------------------------------------------

	void addLast (const Cell &tmp){
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

	Cell delFirst (){
		Cell tmp;
		if (n == 0){
			tmp.col = -1; 
			tmp.row = -1;
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

void input (char **&labyrinth, int **&dir, Cell &start, Cell &finish, Cell &size){
	int n, m;
	ifstream in ("input.txt");
	in >> n; dir = new int* [n]; labyrinth = new char* [n];
	in >> m; size.row = n; size.col = m;
	for (int i = 0; i < n; i++){
		dir [i] = new int [m]; labyrinth [i] = new char [m];
	}
	in >> start.row >> start.col >> finish.row >> finish.col;
	start.row--; start.col--; finish.row--; finish.col--;
	for (int i = 0; i < n; i++){
		in >> labyrinth [i];
	    //cout << labyrinth[i] << endl;
		for (int j = 0; j < m; j++)
			dir [i][j] = 0;
	}
	dir [finish.row][finish.col] = 0;
	dir [start.row][start.col] = -3;
}

int basicMethod (char **&labyrinth, int **&dir, const Cell &start, const Cell &finish, const Cell &size){
	int answer = size.row * size.col; 
	Queue q (answer); answer = 0;
	Cell tmpc; //tmpc-элемент извлеченный из очереди; tmpc2-элемент положенный в очередь
	q.addLast (start);
	tmpc = q.delFirst ();
	while (tmpc != finish){
	    if ((tmpc.col != 0) && (dir [tmpc.row][tmpc.col - 1] == 0) && (labyrinth [tmpc.row][tmpc.col - 1] == '.')){
			dir [tmpc.row][--tmpc.col] = -1;
			q.addLast(tmpc); tmpc.col++;
    	}
    	if ((tmpc.col != size.col - 1) && (dir [tmpc.row][tmpc.col + 1] == 0) && (labyrinth [tmpc.row][tmpc.col + 1] == '.')){
			dir [tmpc.row][++tmpc.col] = 1;
		    q.addLast(tmpc); tmpc.col--;
    	}
    	if ((tmpc.row != 0) && (dir[tmpc.row - 1][tmpc.col] == 0) && (labyrinth [tmpc.row - 1][tmpc.col] == '.')){
			dir [--tmpc.row][tmpc.col] = -2;
			q.addLast(tmpc); tmpc.row++;
    	}
    	if ((tmpc.row != size.row - 1) && (dir [tmpc.row + 1][tmpc.col] == 0) && (labyrinth [tmpc.row + 1][tmpc.col] == '.')){
			dir [++tmpc.row][tmpc.col] = 2;
			q.addLast(tmpc); tmpc.row--;
    	}
		tmpc = q.delFirst (); 
		if ((tmpc.row == -1) && (tmpc.col == -1)){
			answer = -1; tmpc = finish;
		}
	}
    
	if (answer != -1){
    	tmpc = finish;
	    while (tmpc != start){
			if (dir [tmpc.row][tmpc.col] == -1){ Arr [NumAnsw++] = tmpc; tmpc.col++; }
	    	else
				if (dir [tmpc.row][tmpc.col] == 1){ Arr [NumAnsw++] = tmpc; tmpc.col--; }
	    		else
					if (dir [tmpc.row][tmpc.col] == -2){ Arr [NumAnsw++] = tmpc; tmpc.row++; }
		    		else
						if (dir [tmpc.row][tmpc.col] == 2){ Arr [NumAnsw++] = tmpc; tmpc.row--; }
			//cout << tmpc << endl;
	    	answer++;
	    }
	}
	Arr [NumAnsw++] = start;

	return answer;
}

int main (){
	char **labyrinth;
	int **dir, answer;
	Cell start, finish, size;
	input (labyrinth, dir, start, finish, size);
	answer = basicMethod (labyrinth, dir, start, finish, size);
	cout << answer;
	cout << endl;
	for (int i = 0; i < NumAnsw; i++)
		cout << Arr [i];
	system ("pause");
	return 0;
}