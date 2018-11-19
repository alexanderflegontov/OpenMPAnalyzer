#include <iostream>
#include <time.h>
#include <omp.h>
#include <fstream>
#include <stdlib.h> // abs, rand
#include <math.h> // sqrt, pow

using namespace std;

#define CHUNK_SIZE 10

int** CreateMatrix(int N)
{
	int **matrix = new int*[N];
	for (int i = 0; i < N; i++)
		matrix[i] = new int[N];

	return matrix;
}

void DeleteMatrix(int** matr, int N)
{
	for (int i = 0; i < N; i++)
		delete[] matr[i];
	delete[] matr;
}

void PrintMatrix(int** matrix, int N)
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			cout << matrix[i][j] << " ";
		cout << endl;
	}

	cout << endl;
}

void GenerateRandomMatrix(int** matrix1, int** matrix2, int N)
{
	//srand(time(0));

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			matrix1[i][j] = rand() % 10;
			matrix2[i][j] = rand() % 10;
		}
	}
}

void Simple_Mmult(int** A, int** B, int** C, int N){
	int sum;
	for (int i = 0; i < N; i++)
	for (int j = 0; j < N; j++){
		sum = 0;
		for (int k = 0; k < N; k++)
			sum += A[i][k] * B[k][j];
		C[i][j] = sum;
	}
}

void Add(int** A, int** B, int** C, int N)
{
	for (int i = 0; i < N; i++)
	for (int j = 0; j < N; j++)
		C[i][j] = A[i][j] + B[i][j];
}

void Add(int** A, int** B, int** C, int** D, int** E, int N)
{
	for (int i = 0; i < N; i++)
	for (int j = 0; j < N; j++)
		E[i][j] = A[i][j] + B[i][j] + C[i][j] + D[i][j];
}

void Sub(int** A, int** B, int** C, int N)
{
	for (int i = 0; i < N; i++)
	for (int j = 0; j < N; j++)
		C[i][j] = A[i][j] - B[i][j];
}

void Sub(int** A, int** B, int** C, int** D, int** E, int N)
{
	for (int i = 0; i < N; i++)
	for (int j = 0; j < N; j++)
		E[i][j] = A[i][j] + B[i][j] + C[i][j] - D[i][j];
}

bool IsEqualMatrix(int** matrix1, int** matrix2, int N)
{
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (matrix1[i][j] != matrix2[i][j])
				return false;
		}
	}
	return true;
}

void Str_alg(int** matrix1, int** matrix2, int** matrix3, int N, int threshold)
{
	if (N <= threshold)
		Simple_Mmult(matrix1, matrix2, matrix3, N);
	else
	{
		N = N / 2;

		int** A[4]; int** B[4]; int** C[4]; int** P[7];

		int** TMP1 = CreateMatrix(N); int** TMP2 = CreateMatrix(N); int** TMP3 = CreateMatrix(N); int** TMP4 = CreateMatrix(N); int** TMP5 = CreateMatrix(N);
		int** TMP6 = CreateMatrix(N); int** TMP7 = CreateMatrix(N); int** TMP8 = CreateMatrix(N); int** TMP9 = CreateMatrix(N); int** TMP10 = CreateMatrix(N);

		/*Выделяем память под вспомогательные матрицы*/
		for (int i = 0; i < 4; i++)
		{
			A[i] = CreateMatrix(N);
			B[i] = CreateMatrix(N);
			C[i] = CreateMatrix(N);
		}

		for (int i = 0; i < 7; i++)
			P[i] = CreateMatrix(N);

		for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			A[0][i][j] = matrix1[i][j];
			A[1][i][j] = matrix1[i][j + N];
			A[2][i][j] = matrix1[i + N][j];
			A[3][i][j] = matrix1[i + N][j + N];

			B[0][i][j] = matrix2[i][j];
			B[1][i][j] = matrix2[i][j + N];
			B[2][i][j] = matrix2[i + N][j];
			B[3][i][j] = matrix2[i + N][j + N];
		}

		Add(A[0], A[3], TMP1, N);
		Add(B[0], B[3], TMP2, N);
		Str_alg(TMP1, TMP2, P[0], N, threshold); // (A11 + A22)*(B11 + B22)

		Add(A[2], A[3], TMP3, N);
		Str_alg(TMP3, B[0], P[1], N, threshold); // (A21 + A22)*B11

		Sub(B[1], B[3], TMP4, N);
		Str_alg(A[0], TMP4, P[2], N, threshold); // A11*(B12 - B22)

		Sub(B[2], B[0], TMP5, N);
		Str_alg(A[3], TMP5, P[3], N, threshold); // A22*(B21 - B11)

		Add(A[0], A[1], TMP6, N);
		Str_alg(TMP6, B[3], P[4], N, threshold); // (A11 + A12)*B22

		Sub(A[2], A[0], TMP7, N);
		Add(B[0], B[1], TMP8, N);
		Str_alg(TMP7, TMP8, P[5], N, threshold); // (A21 - A11)*(B11 + B12)

		Sub(A[1], A[3], TMP9, N);
		Add(B[2], B[3], TMP10, N);
		Str_alg(TMP9, TMP10, P[6], N, threshold); // (A12 - A22)*(B21 + B22)

		Sub(P[0], P[3], P[6], P[4], C[0], N); // P1 + P4 - P5 + P7
		Add(P[2], P[4], C[1], N); // P3 + P5
		Add(P[1], P[3], C[2], N); // P2 + P4
		Sub(P[0], P[2], P[5], P[1], C[3], N); // P1 - P2 + P3 + P6

		for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			matrix3[i][j] = C[0][i][j];
			matrix3[i][j + N] = C[1][i][j];
			matrix3[i + N][j] = C[2][i][j];
			matrix3[i + N][j + N] = C[3][i][j];
		}

		for (int i = 0; i < 4; i++){
			DeleteMatrix(A[i], N);
			DeleteMatrix(B[i], N);
			DeleteMatrix(C[i], N);
		}

		for (int i = 0; i < 7; i++){
			DeleteMatrix(P[i], N);
		}

		DeleteMatrix(TMP1, N); DeleteMatrix(TMP2, N); DeleteMatrix(TMP3, N); DeleteMatrix(TMP4, N); DeleteMatrix(TMP5, N);
		DeleteMatrix(TMP6, N); DeleteMatrix(TMP7, N); DeleteMatrix(TMP8, N); DeleteMatrix(TMP9, N); DeleteMatrix(TMP10, N);
	}
}

void Str_alg_Pp(int** matrix1, int** matrix2, int** matrix3, int N, int threshold)
{
	if (N <= threshold)
		Simple_Mmult(matrix1, matrix2, matrix3, N);
	else
	{
		N = N / 2;

		int** A[4]; int** B[4]; int** C[4]; int** P[7];

		int** TMP1 = CreateMatrix(N); int** TMP2 = CreateMatrix(N); int** TMP3 = CreateMatrix(N); int** TMP4 = CreateMatrix(N); int** TMP5 = CreateMatrix(N);
		int** TMP6 = CreateMatrix(N); int** TMP7 = CreateMatrix(N); int** TMP8 = CreateMatrix(N); int** TMP9 = CreateMatrix(N); int** TMP10 = CreateMatrix(N);

		for (int i = 0; i < 4; i++)
		{
			A[i] = CreateMatrix(N);
			B[i] = CreateMatrix(N);
			C[i] = CreateMatrix(N);
		}

		int i, j;

		for (int i = 0; i < 7; i++)
			P[i] = CreateMatrix(N);

		#pragma omp parallel
		{
			#pragma omp for private(i,j) schedule(static, CHUNK_SIZE) 
			for (i = 0; i < N; i++)
			for (j = 0; j < N; j++)
			{
				A[0][i][j] = matrix1[i][j];
				A[1][i][j] = matrix1[i][j + N];
				A[2][i][j] = matrix1[i + N][j];
				A[3][i][j] = matrix1[i + N][j + N];

				B[0][i][j] = matrix2[i][j];
				B[1][i][j] = matrix2[i][j + N];
				B[2][i][j] = matrix2[i + N][j];
				B[3][i][j] = matrix2[i + N][j + N];
			}
			
			#pragma omp sections
			{
				#pragma omp section
				{
					Add(A[0], A[3], TMP1, N);
					Add(B[0], B[3], TMP2, N);
					Str_alg_Pp(TMP1, TMP2, P[0], N, threshold); // (A11 + A22)*(B11 + B22)
				}
				#pragma omp section
				{
					Add(A[2], A[3], TMP3, N);
					Str_alg_Pp(TMP3, B[0], P[1], N, threshold); // (A21 + A22)*B11
				}
				#pragma omp section
				{

					Sub(B[1], B[3], TMP4, N);
					Str_alg_Pp(A[0], TMP4, P[2], N, threshold); // A11*(B12 - B22)
				}
				#pragma omp section
				{
					Sub(B[2], B[0], TMP5, N);
					Str_alg_Pp(A[3], TMP5, P[3], N, threshold); // A22*(B21 - B11)
				}
				#pragma omp section
				{
					Add(A[0], A[1], TMP6, N);
					Str_alg_Pp(TMP6, B[3], P[4], N, threshold); // (A11 + A12)*B22
				}
				#pragma omp section
				{
					Sub(A[2], A[0], TMP7, N);
					Add(B[0], B[1], TMP8, N);
					Str_alg_Pp(TMP7, TMP8, P[5], N, threshold); // (A21 - A11)*(B11 + B12)
				}
				#pragma omp section
				{
					Sub(A[1], A[3], TMP9, N);
					Add(B[2], B[3], TMP10, N);
					Str_alg_Pp(TMP9, TMP10, P[6], N, threshold); // (A12 - A22)*(B21 + B22)
				}
			}

			#pragma omp sections
			{
				#pragma omp section
				Sub(P[0], P[3], P[6], P[4], C[0], N); // P1 + P4 - P5 + P7

				#pragma omp section
				Add(P[2], P[4], C[1], N); // P3 + P5

				#pragma omp section
				Add(P[1], P[3], C[2], N); // P2 + P4

				#pragma omp section
				Sub(P[0], P[2], P[5], P[1], C[3], N); // P1 - P2 + P3 + P6
			}

			#pragma omp for private(i,j) schedule(static, CHUNK_SIZE) 
			for (i = 0; i < N; i++)
			for (j = 0; j < N; j++)
			{
				matrix3[i][j] = C[0][i][j];
				matrix3[i][j + N] = C[1][i][j];
				matrix3[i + N][j] = C[2][i][j];
				matrix3[i + N][j + N] = C[3][i][j];
			}
		}

		for (int i = 0; i < 4; i++){
			DeleteMatrix(A[i], N);
			DeleteMatrix(B[i], N);
			DeleteMatrix(C[i], N);
		}

		for (int i = 0; i < 7; i++){
			DeleteMatrix(P[i], N);
		}

		DeleteMatrix(TMP1, N); DeleteMatrix(TMP2, N); DeleteMatrix(TMP3, N); DeleteMatrix(TMP4, N); DeleteMatrix(TMP5, N);
		DeleteMatrix(TMP6, N); DeleteMatrix(TMP7, N); DeleteMatrix(TMP8, N); DeleteMatrix(TMP9, N); DeleteMatrix(TMP10, N);
	}
}

int main(int argc, char** argv)
{
	int** matr_A = NULL;
	int** matr_B = NULL;
	int** matr_Rez_Str = NULL;
	int** matr_Rez_Str_PP = NULL;
	int** matr_Rez_Check = NULL;

	int  N, thr = 64;

	double StartStrAlg = 0;
	double TimeStrAlg = 0;
	double StartStrPpAlg = 0;	
	double TimeStrPpAlg = 0;
	int k, num_th;

	cout << "Enter the degree of two and num of threads:" << endl;
	//cin >> k >> num_th;
	k = 2;
	num_th = 2;
	if(argc > 2){
		k = atoi(argv[1]);
		num_th = atoi(argv[2]);	
	}else if(argc == 2){
		k = atoi(argv[1]);
	}

    

	cout << "degree of two: " << k << endl;
	cout << "num of threads:" << num_th << endl;

	N = (int)pow(2.0, k);
	omp_set_num_threads(num_th);

	cout << "Size of matrix: " << N << " x " << N << endl;

	/* Создание и заполнение матриц */
	matr_A = CreateMatrix(N);
	matr_B = CreateMatrix(N);
	matr_Rez_Str = CreateMatrix(N);
	matr_Rez_Str_PP = CreateMatrix(N);
	matr_Rez_Check = CreateMatrix(N);
	GenerateRandomMatrix(matr_A, matr_B, N);

	/* Последовательный алгоритм */
	StartStrAlg = omp_get_wtime();
	Str_alg(matr_A, matr_B, matr_Rez_Str, N, thr);
	TimeStrAlg = omp_get_wtime() - StartStrAlg;

	/* Вывод матриц, если размер матриц максимум 16x16*/
	if (k < 5)
	{
		cout << "Matrix A: " << endl;
		PrintMatrix(matr_A, N);
		cout << endl;
		cout << "Matrix B: " << endl;
		PrintMatrix(matr_B, N);
		cout << endl;
		cout << "Matrix C: " << endl;
		PrintMatrix(matr_Rez_Str, N);
		cout << endl;
	}

	/* Параллельный алгоритм */
	StartStrPpAlg = omp_get_wtime();
	Str_alg_Pp(matr_A, matr_B, matr_Rez_Str_PP, N, thr);
	TimeStrPpAlg = omp_get_wtime() - StartStrPpAlg;

	/* Проверка результата умножение матриц алгоритмом Штрассена и обычным алгоритмом, если размер матриц максимум 1024x1024*/
	if (k < 11)
	{
		Simple_Mmult(matr_A, matr_B, matr_Rez_Check, N);

		if (IsEqualMatrix(matr_Rez_Check, matr_Rez_Str, N))
			cout << "Check: matr_Rez_Check and matr_Rez_Str are equal";
		else
			cout << "Check: matr_Rez_Check and matr_Rez_Str are not equal";
		cout << endl;
	}

	/* Проверка результата умножение матриц последоватльнным алгоритмом Штрассена и параллельным*/
	if (IsEqualMatrix(matr_Rez_Str, matr_Rez_Str_PP, N))
		cout << "Check: Maxtrix matr_Rez_Str and matr_Rez_Str_P are equal";
	else
		cout << "Check: Maxtrix matr_Rez_Str and matr_Rez_Str_P are not equal";
	cout << endl;

	/* Вывод времени работы алгоритмов*/
	cout << "Strassen algorithm time = " << TimeStrAlg << endl;
	cout << "Strassen parallel algorithm time = " << TimeStrPpAlg << endl;
	cout << endl;

	DeleteMatrix(matr_Rez_Check, N);
	DeleteMatrix(matr_Rez_Str_PP, N);

	DeleteMatrix(matr_Rez_Str, N);
	DeleteMatrix(matr_B, N);
	DeleteMatrix(matr_A, N);

	return 0;
}
