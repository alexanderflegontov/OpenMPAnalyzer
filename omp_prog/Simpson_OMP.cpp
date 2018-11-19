#include <omp.h>
#include <iostream>
#include <stdlib.h> // abs
#include <math.h> // sqrt

#define Eps (0.000000000001)

inline bool Test(double sequent, double parallel)
{
	bool OK = true;
	if (abs(sequent - parallel) > Eps)
		OK = false;

	std::cout << "\n Check the results ...";
	if (OK != true)
		std::cout << "Warning!!! Something went wrong." << std::endl;
	else
		std::cout << "Successfully!!!" << std::endl;

	return OK;
}

inline void CheckResults(double sequentTimeWork, double parallTimeWork)
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


double Simpson_integration(double(*f)(double, double), double a, double b, int N, double fixX)
{
	double h = static_cast<double>(b - a) / N;

	double S4 = f(fixX, a + h), S2 = 0.;

	for (int i = 3; i < N; i += 2)
	{
		S4 += f(fixX, a + i*h);
		S2 += f(fixX, a + (i - 1)*h);
	}

	return (h / 3) * (f(fixX, a) + 4 * S4 + 2 * S2 + f(fixX, b));
}

double Simpson2(double(*F)(double, double), double a1, double b1, double a2, double b2, int n)
{
	if (n <= 0)
		return 0.;

	int sign = 1;
	
	if (abs(a1 - b1) < Eps || abs(a2 - b2) < Eps)
		return 0.;
	
	if (a1 > b1)
	{
		double tmp = b1;
		b1 = a1;
		a1 = b1;
		sign *= -1;
	}

	if (a2 > b2)
	{
		double tmp = b2;
		b2 = a2;
		a2 = b2;
		sign *= -1;
	}

	int N = 2 * n;

	double h2 = static_cast<double>(b2 - a2) / N;

	double I2 = 0., I3 = 0.;

	for (int j = 1; j < N; ++j)
	{
		if (j % 2 == 1)
			I3 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
		else
			I2 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
	}

	return sign * (h2 / 3) * (Simpson_integration(F, a1, b1, N, a2) + 2 * I2 + 4 * I3 + Simpson_integration(F, a1, b1, N, b2));
}

double Simpson2_parallel(double(*F)(double, double), double a1, double b1, double a2, double b2, int n)
{
	if (n <= 0)
		return 0.;

	int sign = 1;

	if (abs(a1 - b1) < Eps || abs(a2 - b2) < Eps)
		return 0.;

	if (a1 > b1)
	{
		double tmp = b1;
		b1 = a1;
		a1 = b1;
		sign *= -1;
	}

	if (a2 > b2)
	{
		double tmp = b2;
		b2 = a2;
		a2 = b2;
		sign *= -1;
	}

	int N = 2 * n;

	double h2 = static_cast<double>(b2 - a2) / N;

	double I2 = 0., I3 = 0.;

	int j = 0;
#pragma omp parallel for shared(a1, b1, a2, N, h2) private(j) reduction(+:I3,I2)
	for (j = 1; j < N; ++j)
	{
		if (j % 2 == 1)
			I3 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
		else
			I2 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
	}

	return sign * (h2 / 3) * (Simpson_integration(F, a1, b1, N, a2) + 2 * I2 + 4 * I3 + Simpson_integration(F, a1, b1, N, b2));
}


inline double func2(double x, double y)
{
	return x*y*y;
}

inline double func1(double x, double y)
{
	return (x*x*y-x)/sqrt(x+y*y*x);
}

int main(int argc, char** argv)
{
	int num_th = 2;
	if(argc > 1){
		num_th = atoi(argv[1]);	
	}

	int n = 1000;
	if(argc > 2){
		n = atoi(argv[2]);
	}

	std::cout << "num_th = " << num_th << std::endl;
	omp_set_num_threads(num_th);

	std::cout << "n = " << n << std::endl;

	double t1 = 0., t2 = 0., parallTimeWork = 0., sequentTimeWork = 0.;
	double Sequent_integration = 0., Parallel_integration = 0.;
	

	t1 = omp_get_wtime();
		Sequent_integration = Simpson2(&func1, 1, 2, 2, 3, n);
	t2 = omp_get_wtime();
	sequentTimeWork = (t2 - t1) * 1000.;

	std::cout.setf(std::ios::fixed);
	std::cout.precision(15);
	std::cout << "\n ******  Simpson Multi-Dimensional Integration ******";
	std::cout << "\nSimpson integrate = " << Sequent_integration << std::endl;
	std::cout.precision(6);
	std::cout << " time = " << sequentTimeWork << "ms\n ***************************";

	
	t1 = omp_get_wtime();
		Parallel_integration = Simpson2_parallel(&func1, 1, 2, 2, 3, n);
	t2 = omp_get_wtime();
	parallTimeWork = (t2 - t1) * 1000.;
	
	std::cout.precision(15);
	std::cout << "\nSimpson integrate = " << Parallel_integration;
	std::cout.precision(6);
	if (Test(Sequent_integration, Parallel_integration))
		std::cout << " time = " << parallTimeWork << "ms\n ***************************";
	
	CheckResults(sequentTimeWork, parallTimeWork);
	std::cout << "\n ***************************\n";

	//system("pause");
	return 0;
}
