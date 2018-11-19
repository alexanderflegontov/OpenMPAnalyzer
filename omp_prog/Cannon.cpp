#include <omp.h>
#include <iostream>
#include <string>
#include <iomanip>

#include <stdlib.h> // abs, rand, srand
#include <math.h> // sqrt, pow


typedef int* Matrix;

int ThreadNum = 4; //  можно задавать только те из которых извлекается квадратный корень!!! тоесть 4, 9, 16 .. и т.д.

// проверка матрицы последовательного алг. с параллельным
bool Test(const Matrix seqC, const Matrix parC, int size)
{
	bool OK = true;
	

	for (int i = 0; i < size; i++)
	for (int j = 0; j < size; j++)
	{
		if (seqC[i*size + j] != parC[i*size + j])
		{
			OK = false;
			break;
		}
	}


	std::cout << "\n Check the results ...";
	if (OK != true)
		std::cout << "Warning!!! Something went wrong." << std::endl;
	else
		std::cout << "Successfully!!!" << std::endl;

	return OK;
}

// выводит каккой алгоритм быстрее и собстевнно во сколько.
void CheckResults(double sequentTimeWork, double parallTimeWork)
{
	std::cout << "\n Who is faster? ...";
	if (parallTimeWork < sequentTimeWork)
		std::cout << " Parallel algorithm" << std::endl;
	else
		std::cout << " Sequential algorithm" << std::endl;

	std::cout.precision(3);
	std::cout.setf(std::ios::fixed);
	std::cout << " Speedup: " << sequentTimeWork / parallTimeWork << std::endl;
}

using namespace std;
// выводит матрицу на экран
void PrintMatrix(std::string Str, const Matrix A, int size)
{
	if (size < 10)
	{

		std::cout << Str << std::endl;
		//std::cout << std::setw(4);
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				std::cout << A[i*size + j] << std::setw(4);
			}
			std::cout << std::endl;
		}
	}
}

// инициализирует матрицу рандомными числами
void Initialize(Matrix matrix, int size)
{
	for (int i = 0; i < size; i++)
	for (int j = 0; j < size; j++)
	{
		matrix[i*size + j] = rand()%10;
	}
}

// инициализирует матрицу нулями
void Initialize(Matrix matrix, int size, int value)
{
	for (int i = 0; i < size; i++)
	for (int j = 0; j < size; j++)
	{
		matrix[i*size + j] = value;
	}
}

// последовательный алгоритм умножения матриц
void MultMatrix(const Matrix A, const Matrix B, Matrix C, int size)
{
	for (int i = 0; i < size; ++i)
	for (int j = 0; j < size; ++j)
	for (int k = 0; k < size; ++k)
	{
		C[i*size + j] += A[i*size + k] * B[k*size + j];
	}
}

// параллельный алгоритм умножения матриц
void Parallel_Mult_Matrix(const Matrix A, const  Matrix B, Matrix C, int size)
{
	int GridSize = int(sqrt((double)ThreadNum));
	int BlockSize = size / GridSize;
	omp_set_num_threads(ThreadNum);

#pragma omp parallel
	{
		int ThreadID = omp_get_thread_num();
		int RowIndex = ThreadID / GridSize;
		int ColIndex = ThreadID % GridSize;

		for (int iter = 0; iter < GridSize; iter++) 
		{
			for (int i = RowIndex*BlockSize; i < (RowIndex + 1)*BlockSize; i++)
			for (int j = ColIndex*BlockSize; j < (ColIndex + 1)*BlockSize; j++)
			for (int k = iter*BlockSize; k < (iter + 1)*BlockSize; k++)
				C[i*size + j] += A[i*size + k] * B[k*size + j];
		}
	}
}


int main(int argc, char* argv[])
{
	double t1 = 0., t2 = 0., parallTimeWork = 0., sequentTimeWork = 0.;

	int n = 800; // количество строк и столбцов., должно быть чётное!
	if(argc>1){
		n = atoi(argv[1]);	
	}
	std::cout << "dimension: " << n << "x" << n << std::endl;

	Matrix A = new int[n*n];
	Matrix B = new int[n*n];
	Matrix seqC = new int[n*n];
	Matrix parC = new int[n*n];

	Initialize(A, n);
	Initialize(B, n);
	Initialize(seqC, n, 0);
	Initialize(parC, n, 0);

	PrintMatrix("Matrix seqA", A, n);
	PrintMatrix("Matrix seqB", B, n);
	PrintMatrix("Matrix seqC", seqC, n);
	std::cout << std::endl;

///
	std::cout << "\n ******  Mult Matrix ******\n\n";
	std::cout.precision(6);


	t1 = omp_get_wtime();
		MultMatrix(A, B, seqC, n);
	t2 = omp_get_wtime();
	sequentTimeWork = (t2 - t1) * 1000.;

	PrintMatrix("Matrix seqC", seqC, n);
	std::cout << " sequentTimeWork = " << sequentTimeWork << "ms\n\n";

/////omp
	std::cout << std::endl << " OMP ";
	Matrix parC1 = new int[n*n];
	Initialize(parC1, n, 0);

	t1 = omp_get_wtime();
		Parallel_Mult_Matrix(A, B, parC1, n);
	t2 = omp_get_wtime();

	parallTimeWork = (t2 - t1) * 1000.;
	PrintMatrix("Matrix parC1", parC1, n);
	if (Test(seqC, parC1, n))
		std::cout << " parallTimeWork = " << parallTimeWork << "ms\n";
///

	CheckResults(sequentTimeWork, parallTimeWork);
	std::cout << "\n\n";

	//system("pause");
	return 0;
}
