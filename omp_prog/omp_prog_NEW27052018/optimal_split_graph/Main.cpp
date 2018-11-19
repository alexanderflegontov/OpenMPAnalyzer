///////////////////////////////
////                       ////
////   Sequental version   ////
////                       ////
///////////////////////////////

#include <iostream>
#include "Graph.h"
#include "stdlib.h"
#include <omp.h>

void printRes(int*, int);

using namespace std;

int main(int argc, char** argv) {

	srand(time(NULL));
	int vNumb, probty, iterNumb;
/*
	cin.clear();
	cout << "Enter the even number of vertexes" << endl;
	while (!(std::cin >> vNumb) || (vNumb%2 != 0)){
		cin.clear();
		while (cin.get() != '\n');
		cout << "Enter the even number of elements" << endl;
	}
	cin.clear();
	cout << "Enter the chance of edge appearance" << endl;
	while (!(std::cin >> probty)){
		cin.clear();
		while (cin.get() != '\n');
		cout << "Enter the number of elements" << endl;
	}
	cin.clear();
	cout << "Enter the number of iterations" << endl;
	while (!(std::cin >> iterNumb)){
		cin.clear();
		while (cin.get() != '\n');
		cout << "Enter the number of iterations" << endl;
	}
*/
	int numTh = 2;
	vNumb = 2;
	probty = 50;
	iterNumb = 10; 
	if(argc > 1){
		numTh = atoi(argv[1]);	
	}
	if(argc > 2){
		vNumb = atoi(argv[2]);
	}
	if(argc > 3){
		probty = atoi(argv[3]);
	}
	if(argc > 4){
		iterNumb = atoi(argv[4]);
	}

cout << "Number of threads             = " << numTh << endl;
cout << "the even number of vertexes   = " << vNumb << endl;
cout << "the chance of edge appearances= " << probty << endl;
cout << "numIterations                 = " << iterNumb << endl;

	if(!(numTh>0)){
		std::cout <<"numTh should be > 0 " << std::endl;
		return 1;	
	}
	if(!(vNumb>0 && (vNumb%2 == 0))){
	std::cout <<"the even number of vertexes should be > 0 & (x)%2==0" << std::endl;
		return 2;	
	}
	if(!(probty>0)){
		std::cout <<"the number of elements should be > 0 " << std::endl;
		return 3;			
	}
	if(!(probty>0)){
		std::cout <<"the number of iterations should be > 0 " << std::endl;
		return 4;			
	}

	omp_set_num_threads(numTh);


	int** resArr = new int*[iterNumb];

	Graph G(vNumb, probty);
	//G.print_matrix();


	double t_start, t_end;
	t_start = omp_get_wtime();
	int j;
	// Main loop of exchanges
#pragma omp parallel for shared(vNumb, probty, G) private(j)
	for ( j = 0; j < iterNumb; ++j)
	{
		G.create_parts();
		//G.print_parts();//
		G.def_calc();
		G.def_balance();
		//G.print_parts();//
		//G.print_defs();//
		for (int i(1); i <= (vNumb+1) / 2; i++)
		{
			if (!G.exchange_pair(i))
				break;
			G.def_recalc();
			G.def_balance();
		}
		resArr[j] = G.create_result();
	}

	t_end = omp_get_wtime();
	double time = t_end - t_start;

/*
	for (int i(0); i < iterNumb; i++){
		cout << i << " iteration result:" << endl;
		printRes(resArr[i], vNumb);
	}
*/

	cout << "Number of threads: " << omp_get_num_threads() << endl;
	//cout << "Time of work linear algorithm: " << GIFT_TIME1 << " sec"<< endl;
	cout << "Time of work parallel OpenMP algorithm: " << time << " sec" <<endl;
	//cout << "Speed up " << GIFT_TIME1 / GIFT_TIME2 << endl;

	//system("pause");
	return 0;
}

void printRes(int* arr, int size){
	for (int i(1); i < 3; i++){
		cout << i << " part:\n";
		for (int j(0); j < size; j++){
			if (arr[j] == i)
				cout << j << ' ';
		}
		cout << endl;
	}
	cout << "Cut size = " << arr[size] << endl << endl;
}
