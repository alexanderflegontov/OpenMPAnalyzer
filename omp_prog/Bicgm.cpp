
#ifndef BI_CONJUGATE_GRADIENT_METHOD
#define BI_CONJUGATE_GRADIENT_METHOD

#include <omp.h>
#include <vector>

#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <string.h>
#include <sstream>

#include <stddef.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <math.h>
#include <string.h> // memset
#include <iomanip>
//using namespase std;

//#define IN_TEST_SYSTEM 

#ifndef IN_TEST_SYSTEM

#include <iostream>
#include <sstream>
#include <iomanip>
#include <time.h>

#define DEFAULT_NUM_THREADS (4)
#define DEFAULT_N (10)
#define DEFAULT_CONT_IN_ROW (DEFAULT_N/(10))
#define DEFAULT_EPSILON (0.01)
#define DEFAULT_MAX_ITER (10)
#define DEFAULT_SWITCH_TEST (false)
#define DEFAULT_NUM_AVG (10);

#define IO_SPACE (7)
#define IO_DOUBLE_PRECISION (3)

#define START_DOUBLE (-5.0)
#define MAX_RAND_DOUBLE (10)
#define START_DOUBLE_DIAG (50)


typedef struct {
	int n; // Число строк в матрице
	int m; // Число столбцов в матрице
	int nz; // Число ненулевых элементов в разреженной матрице
	 std::vector<double> val; // Массив значений матрицы по строкам
	 std::vector<int> colIndex; // Массив номеров столбцов
	 std::vector<int> rowPtr; // Массив индексов начала строк
}CRSMatrix;


void ParseInputArgs(int argc, char* argv[]);
void CRS_GenerateSimmetricPositiveMatrix(CRSMatrix & matA, const int n, const int cntInRow);
void GenerateVector(double * b, const int n);
bool CalculateErr(const CRSMatrix & matA, const double * b, const double * x, double &err);
 std::string CRS_PrintMatrix(const CRSMatrix & A, const char* strPrint = "PrintMatrix");
 std::string PrintVector(const double * vect, const int n, const char* strPrint = "PrintVector");


double ScalarProduct_SEQ(const double * vect1, const double * vect2, const int n);
void MatrixMultVect_SEQ(const CRSMatrix & matr, const double * vect, double * vectR);
void SLE_Solver_CRS_BICG_SEQUENTIAL(CRSMatrix & A, double * b, double eps, int max_iter, double * x, int & iter);
void PrintResults(double sequentTimeWork, double parallTimeWork);
bool TestAlgorithm(CRSMatrix& matA, const bool inParallel);
void Experiment(CRSMatrix & matA);

#endif // IN_TEST_SYSTEM

double ScalarProduct(const double * vect1, const double * vect2, const int n);
void MatrixMultVect(const CRSMatrix & matr, const double * vect, double * vectR);
void Transpose(const int n, const int nz, const double * Acol, const double * ArowIndex, const double * Avalue, double * Atcol, double * AtrowIndex, double * Atvalue);
void SLE_Solver_CRS_BICG(CRSMatrix & A, double * b, double eps, int max_iter, double * x, int & iter);
void CRS_InitMatrix(CRSMatrix & matA, const int n, const int nz);
void CRS_FreeMatrix(CRSMatrix & A);


void CRS_InitMatrix(CRSMatrix & matA, const int n, const int nz) {
	matA.n = n;
	matA.m = n;
	matA.nz = nz;
	matA.val.resize(nz);
	matA.colIndex.resize(nz);
	matA.rowPtr.resize(n + 1);
}

void CRS_FreeMatrix(CRSMatrix & A) {

	 std::vector<double>().swap(A.val);
	 std::vector<int>().swap(A.colIndex);
	 std::vector<int>().swap(A.rowPtr);
}


#ifndef IN_TEST_SYSTEM
    int numThreads = DEFAULT_NUM_THREADS;
    int N = DEFAULT_N;
	bool Switch_test = DEFAULT_SWITCH_TEST;
	int numAVG = DEFAULT_NUM_AVG;
	int CONT_IN_ROW = DEFAULT_CONT_IN_ROW;
	double Epsilon = DEFAULT_EPSILON;
    int MAX_ITER = DEFAULT_MAX_ITER;
	
	double sequentTimeWork, parallTimeWork;

void ParseInputArgs(int argc, char* argv[]){

}

int compare(const void * a, const void * b)
{
	return (*(int*)a - *(int*)b);
}

void CRS_GenerateSimmetricPositiveMatrix(CRSMatrix & matA, const int n, const int cntInRow){
    CRS_InitMatrix(matA, n,cntInRow*n);
	int nz = cntInRow * n;
    int i,j,k;
	bool found;
    double tmp, newIndexCol;
    for(i = 0; i < n; i++){
		matA.colIndex[i*cntInRow] = i; // diagonal element
        for(j = 1; j < cntInRow; j++){
            do{
				newIndexCol = rand() % n;
                
                found = false;
                for(k = 0; k < j; k++){
                    if(newIndexCol == matA.colIndex[i*cntInRow+k]){
						found = true;
						break;
                    }
                }
            }while(found);
			matA.colIndex[i*cntInRow + j] = newIndexCol;
        }
        
		if(cntInRow > 30){
			 qsort(&matA.colIndex[i*cntInRow], cntInRow - 1, sizeof(int), compare);
		}else{
			// Bubble sorting
			for(j = 0; j < cntInRow-1; j++){
				for(k = 0; k < j; k++){
					int& elem1 = matA.colIndex[i*cntInRow+k];
					int& elem2 = matA.colIndex[i*cntInRow+k+1];
					if( elem1 > elem2 ){
						tmp = elem1;
						elem1 = elem2;
						elem2 = tmp;
					}
				}
			}
		}
    }
    
    for(i = 0; i < nz; i++){
        matA.val[i] = (rand()%MAX_RAND_DOUBLE) + START_DOUBLE; 
    }
    
    int c = 0;
    for(i = 0; i <= n; i++){
        matA.rowPtr[i] = c;
        c += cntInRow;
    }

	int indS, indE;
	for (i = 0; i < n; i++) {
		indS = matA.rowPtr[i];
		indE = matA.rowPtr[i + 1];
		for (k = indS; k < indE; k++) {
			if (i == matA.colIndex[k]) {
				matA.val[k] = (rand() % MAX_RAND_DOUBLE) + START_DOUBLE_DIAG;
				break;
			}
		}
	}
}

void GenerateVector(double * b, const int n){
    for(int i = 0; i < n; i++){
		b[i] = ((rand() % MAX_RAND_DOUBLE)/4.0);
    }
}

bool CalculateErr(const CRSMatrix & matA, const double * b, const double * x, double &err){
    double * vectR = new double[matA.n];
    MatrixMultVect(matA, x, vectR);
	double maxErr = 0.0;
    for(int i = 0; i < matA.n; i++){
		err =  abs(vectR[i] - b[i]);
        if(err >= Epsilon){            
            return false;
		}
		err = (err < maxErr) ? maxErr : err;
    }
    return true;
}

 std::string PrintVector(const double * vect, const int n, const char* strPrint){
     std::stringstream ss;

    return "";
}

 std::string CRS_PrintMatrix(const CRSMatrix & A, const char* strPrint){
return "";
}



#endif // IN_TEST_SYSTEM


double ScalarProduct(const double * vect1, const double * vect2, int n) {
	double sp = 0.0;
#pragma omp parallel for shared(vect1, vect2, n) reduction(+:sp)
	for (int i = 0; i < n; i++)
	{
		sp += vect1[i] * vect2[i];
	}
	return sp;
}


void MatrixMultVect(const CRSMatrix & matr, const double * vect, double * vectR) {
	int i, j, start, end;
	 memset(vectR, 0x00, sizeof(double)*matr.n);

#pragma omp parallel for shared(matr,vect,vectR) private(i,j,start,end)
	for (i = 0; i < matr.n; i++) {
		start = matr.rowPtr[i];
		end = matr.rowPtr[i + 1];
		for (j = start; j < end; j++) {
			vectR[i] += matr.val[j] * vect[matr.colIndex[j]];
		}
	}
}

void Transpose(const int n, const int nz, const int * Acol, const int * ArowIndex, const double * Avalue, int * Atcol, int * AtrowIndex, double * Atvalue) {
	 memset(&AtrowIndex[0], 0x00, sizeof(int)*(n + 1));

	int i, j, indS = 0, indE = 0;
	int accumulationRows = 0, cur = 0;

	int* AtrowIndexFrom1 = AtrowIndex + 1;
	for (i = 0; i < nz; ++i)
		++AtrowIndexFrom1[Acol[i]];

	for (i = 1; i <= n; ++i) {
		cur = AtrowIndex[i];
		AtrowIndex[i] = accumulationRows;
		accumulationRows += cur;
	}

	for (i = 0; i < n; ++i) {
		indS = ArowIndex[i];
		indE = ArowIndex[i + 1];
		for (j = indS; j < indE; ++j) {
			int& ind = AtrowIndex[Acol[j] + 1];
			Atvalue[ind] = Avalue[j];
			Atcol[ind] = i;
			++ind;
		}
	}
}

void SLE_Solver_CRS_BICG(CRSMatrix & A, double * b, double eps, int max_iter, double * x, int & iter) {
	CRSMatrix At;
	int n = A.n;

	double * R = new double[n];
	double * biR = new double[n];
	double * nR = new double[n];
	double * nbiR = new double[n];

	double * P = new double[n];
	double * biP = new double[n];
	double * nP = new double[n];
	double * nbiP = new double[n];


	double * tmpPtr;

	double * multAP = new double[n];
	double * multAtbiP = new double[n];

	double alpha, betta;
	double numerator, denominator;
	double check, norm;

	int i;
	/// choice of the starting point
	for (i = 0; i < n; i++) {
		x[i] = 1.0;
	}

	/// preparation of the method
	CRS_InitMatrix(At, n, A.nz);

	Transpose(n, A.nz, &A.colIndex[0], &A.rowPtr[0], &A.val[0], &At.colIndex[0], &At.rowPtr[0], &At.val[0]);

	norm = sqrt(ScalarProduct(b, b, n));

	MatrixMultVect(A, x, multAP);

#pragma omp parallel for shared(R, biR, P, biP, b, multAP, n) private(i)
	for (i = 0; i < n; i++) {
		R[i] = biR[i] = P[i] = biP[i] = b[i] - multAP[i];
	}

	/// realization of method
	for (iter = 0; iter < max_iter; ++iter)
	{
		MatrixMultVect(A, P, multAP);
		MatrixMultVect(At, biP, multAtbiP);

		numerator = ScalarProduct(biR, R, n);
		denominator = ScalarProduct(biP, multAP, n);
		alpha = numerator / denominator;

#pragma omp parallel for shared(nR, R, alpha, multAP, nbiR, biR, multAtbiP, n) private(i)
		for (i = 0; i < n; i++) {
			nR[i] = R[i] - alpha * multAP[i];
			nbiR[i] = biR[i] - alpha * multAtbiP[i];
		}

		denominator = numerator;
		numerator = ScalarProduct(nbiR, nR, n);
		betta = numerator / denominator;

#pragma omp parallel for shared(nP, nR, betta, P, nbiP, nbiR, biP, n) private(i)
		for (i = 0; i < n; i++) {
			nP[i] = nR[i] + betta * P[i];
			nbiP[i] = nbiR[i] + betta * biP[i];
		}

		check = sqrt(ScalarProduct(R, R, n)) / norm;

		if (check < eps) {
			break;
		}

#pragma omp parallel for shared(P, alpha, x, n) private(i)
		for (i = 0; i < n; i++) {
			x[i] += alpha * P[i];
		}

		tmpPtr = R; R = nR; nR = tmpPtr;
		tmpPtr = P; P = nP; nP = tmpPtr;
		tmpPtr = biR; biR = nbiR; nbiR = tmpPtr;
		tmpPtr = biP; biP = nbiP; nbiP = tmpPtr;
	}

	CRS_FreeMatrix(At); //
	delete[] R, biR, nR, nbiR, P, biP, nP, nbiP, tmpPtr, multAP, multAtbiP; //
}


#ifndef IN_TEST_SYSTEM

double ScalarProduct_SEQ(const double * vect1, const double * vect2, const int n) {
	double sp = 0.0;
//#pragma omp parallel for shared(vect1, vect2, n) reduction(+:sp)
	for (int i = 0; i < n; i++) {
		sp += vect1[i] * vect2[i];
	}
	return sp;
}


void MatrixMultVect_SEQ(const CRSMatrix & matr, const double * vect, double * vectR) {
	int i, j, start, end;
	 memset(vectR, 0x00, sizeof(double)*matr.n);

//#pragma omp parallel for shared(matr,vect,vectR) private(i,j,start,end)
	for (i = 0; i < matr.n; i++) {
		start = matr.rowPtr[i];
		end = matr.rowPtr[i + 1];
		for (j = start; j < end; j++) {
			vectR[i] += matr.val[j] * vect[matr.colIndex[j]];
		}
	}
}

void SLE_Solver_CRS_BICG_SEQUENTIAL(CRSMatrix & A, double * b, double eps, int max_iter, double * x, int & iter) {
	CRSMatrix At;
	int n = A.n;

	double * R = new double[n];
	double * biR = new double[n];
	double * nR = new double[n];
	double * nbiR = new double[n];

	double * P = new double[n];
	double * biP = new double[n];
	double * nP = new double[n];
	double * nbiP = new double[n];


	double * tmpPtr;

	double * multAP = new double[n];
	double * multAtbiP = new double[n];

	double alpha, betta;
	double numerator, denominator;
	double check, norm;


	int i;
	/// choice of the starting point 
	for (i = 0; i < n; i++) {
		x[i] = 1.0;
	}

	/// preparation of the method
	norm = sqrt(ScalarProduct_SEQ(b, b, n));

	MatrixMultVect_SEQ(A, x, multAP);

	for (i = 0; i < n; i++) {
		R[i] = biR[i] = P[i] = biP[i] = b[i] - multAP[i];
	}

	CRS_InitMatrix(At, n, A.nz);
	Transpose(n, A.nz, &A.colIndex[0], &A.rowPtr[0], &A.val[0], &At.colIndex[0], &At.rowPtr[0], &At.val[0]);

	/// realization of method
	for (iter = 0; iter < max_iter; iter++)
	{
		MatrixMultVect_SEQ(A, P, multAP);
		MatrixMultVect_SEQ(At, biP, multAtbiP);

		numerator = ScalarProduct_SEQ(biR, R, n);
		denominator = ScalarProduct_SEQ(biP, multAP, n);
		alpha = numerator / denominator;

		for (i = 0; i < n; i++) {
			nR[i] = R[i] - alpha * multAP[i];
		}

		for (i = 0; i < n; i++) {
			nbiR[i] = biR[i] - alpha * multAtbiP[i];
		}

		denominator = numerator;
		numerator = ScalarProduct_SEQ(nbiR, nR, n);
		betta = numerator / denominator;

		for (i = 0; i < n; i++) {
			nP[i] = nR[i] + betta * P[i];
		}

		for (i = 0; i < n; i++) {
			nbiP[i] = nbiR[i] + betta * biP[i];
		}

		check = sqrt(ScalarProduct_SEQ(R, R, n)) / norm;

		if (check < eps) {
			break;
		}

		for (i = 0; i < n; i++) {
			x[i] += alpha * P[i];
		}

		tmpPtr = R; R = nR; nR = tmpPtr;
		tmpPtr = P; P = nP; nP = tmpPtr;
		tmpPtr = biR; biR = nbiR; nbiR = tmpPtr;
		tmpPtr = biP; biP = nbiP; nbiP = tmpPtr;
	}

	CRS_FreeMatrix(At);
	delete[] R, biR, nR, nbiR, P, biP, nP, nbiP, tmpPtr, multAP, multAtbiP;
}


void PrintResults(double sequentTimeWork, double parallTimeWork)
{


}

bool TestAlgorithm(CRSMatrix& matA, const bool inParallel) {
	 std::stringstream ss;
	omp_set_num_threads(numThreads);


	double calc_err = 0.0;
	int outCountIter;
	 std::vector<double> b(N, 1.0);
	 std::vector<double> x(N);

	GenerateVector(&b[0], N);

	double t1 = 0.0, t2 = 0.0;
	if (inParallel) {
		t1 = omp_get_wtime();
		SLE_Solver_CRS_BICG(matA, &b[0], Epsilon, MAX_ITER, &x[0], outCountIter);
		t2 = omp_get_wtime();
		parallTimeWork = (t2 - t1) * 1000.;
		
	}else {
		omp_set_num_threads(1);
		t1 = omp_get_wtime();
		SLE_Solver_CRS_BICG_SEQUENTIAL(matA, &b[0], Epsilon, MAX_ITER, &x[0], outCountIter);
		t2 = omp_get_wtime();
		sequentTimeWork = (t2 - t1) * 1000.;
		
	}

	

	if (matA.n < 6) {
		ss << CRS_PrintMatrix(matA, "matA");
		//show_crs_matrix(matA, "matA");

		ss << PrintVector(&x[0], N, "x");
		ss << PrintVector(&b[0], N, "b");
	}

	bool isOk = CalculateErr(matA, &b[0], &x[0], calc_err);







	 std::vector<double>().swap(b);
	 std::vector<double>().swap(x);

	if(!Switch_test){

	}
	ss.clear();
	return isOk;
}

void Experiment(CRSMatrix & matA) {
	double AVGsequentTimeWork = 0.0, AVGparallTimeWork = 0.0;
	const int numberAVG = numAVG;
	double t1, t2;
	int numOK = 0;

	bool isOk = true;
	t1 = omp_get_wtime();
	for (int i = 0; i < numberAVG; i++) {
		isOk = isOk && TestAlgorithm(matA, false);
		isOk = isOk && TestAlgorithm(matA, true);
		if (isOk) {
			AVGsequentTimeWork += sequentTimeWork;
			AVGparallTimeWork += parallTimeWork;
			numOK++;
		}
	}
	t2 = omp_get_wtime();

	if (numOK != 0) {
		AVGsequentTimeWork /= numOK;
		AVGparallTimeWork /= numOK;
		PrintResults(AVGsequentTimeWork, AVGparallTimeWork);

	}

}


int main(int argc, char* argv[]){
    srand((unsigned)time(0));
    
    ParseInputArgs(argc, argv);
    
    CRSMatrix matA;
    CRS_GenerateSimmetricPositiveMatrix(matA, N, CONT_IN_ROW);
	
	if (Switch_test) {
		Experiment(matA);
	}else{
		sequentTimeWork = parallTimeWork = 0.0;
		bool isOk = true;
		isOk = isOk && TestAlgorithm(matA, false);
		isOk = isOk && TestAlgorithm(matA, true);
		if(isOk){
			PrintResults(sequentTimeWork, parallTimeWork);
		}
	}

	CRS_FreeMatrix(matA);
    system("pause");
    return 0;
}

#endif // IN_TEST_SYSTEM

#endif // BI_CONJUGATE_GRADIENT_METHOD


