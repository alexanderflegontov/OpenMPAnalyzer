#pragma once
#include <omp.h>
#include <iostream>

namespace Simpson
{

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

	////////////////////////////////////////////////////////////////////////////////////////////
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

		for (int j = 1; j < N; j += 2)
		{
			I3 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
			if (j != 1)
				I2 += Simpson_integration(F, a1, b1, N, a2 + (j - 1)*h2);
		}

		return sign * (h2 / 3) * (Simpson_integration(F, a1, b1, N, a2) + 2 * I2 + 4 * I3 + Simpson_integration(F, a1, b1, N, b2));
	}

	double Simpson2_OMP_parallel(double(*F)(double, double), double a1, double b1, double a2, double b2, int n)
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
		for (j = 1; j < N; j += 2)
		{
			I3 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
			if (j != 1)
				I2 += Simpson_integration(F, a1, b1, N, a2 + (j - 1)*h2);
		}

		return sign * (h2 / 3) * (Simpson_integration(F, a1, b1, N, a2) + 2 * I2 + 4 * I3 + Simpson_integration(F, a1, b1, N, b2));
	}


	double Simpson2_test0(double(*F)(double, double), double a1, double b1, double a2, double b2, int n)
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
		
		//tbb::tick_count ti1, ti2;
		//ti1 = tbb::tick_count::now();
		double t1 = 0., t2 = 0.;
		t1 = omp_get_wtime();

		double I2 = 0., I3 = 0.;

		for (int j = 1; j < N; ++j)
		{
			if (j % 2 == 1)
				I3 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
			else
				I2 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
		}
		t2 = omp_get_wtime();
		//ti2 = tbb::tick_count::now();
		double s = sign * (h2 / 3) * (Simpson_integration(F, a1, b1, N, a2) + 2 * I2 + 4 * I3 + Simpson_integration(F, a1, b1, N, b2));
		//return (ti2 - ti1).seconds() * 1000.;
		return (t2 - t1) * 1000.;
	}

	double Simpson2_test1(double(*F)(double, double), double a1, double b1, double a2, double b2, int n)
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
		
		//tbb::tick_count ti1, ti2;
		//ti1 = tbb::tick_count::now();
		double t1 = 0., t2 = 0.;
		t1 = omp_get_wtime();

		double I2 = 0., I3 = 0.;
		
		for (int j = 1; j < N; j += 2)
		{
			I3 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
			if (j != 1)
				I2 += Simpson_integration(F, a1, b1, N, a2 + (j - 1)*h2);
		}
		t2 = omp_get_wtime();
		//ti2 = tbb::tick_count::now();
		double s = sign * (h2 / 3) * (Simpson_integration(F, a1, b1, N, a2) + 2 * I2 + 4 * I3 + Simpson_integration(F, a1, b1, N, b2));
		//return (ti2 - ti1).seconds() * 1000.;
		return (t2 - t1) * 1000.;

	}

	double Simpson2_test2(double(*F)(double, double), double a1, double b1, double a2, double b2, int n)
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

		//tbb::tick_count ti1, ti2;
		//ti1 = tbb::tick_count::now();
		double t1 = 0., t2 = 0.;
		t1 = omp_get_wtime();

		double I2 = 0., I3 = 0.;
		I3 += Simpson_integration(F, a1, b1, N, a2 + h2);

		for (int j = 3; j < N; j += 2)
		{
			I3 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
			I2 += Simpson_integration(F, a1, b1, N, a2 + (j - 1)*h2);
		}
		t2 = omp_get_wtime();
		//ti2 = tbb::tick_count::now();

		double s = sign * (h2 / 3) * (Simpson_integration(F, a1, b1, N, a2) + 2 * I2 + 4 * I3 + Simpson_integration(F, a1, b1, N, b2));
		//return (ti2 - ti1).seconds() * 1000.;
		return (t2 - t1) * 1000.;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	double Simpson2_OMP_parallel_test(double(*F)(double, double), double a1, double b1, double a2, double b2, int n)
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

		//tbb::tick_count ti1, ti2;
		//ti1 = tbb::tick_count::now();

		double t1 = 0., t2 = 0.;
		t1 = omp_get_wtime();
		double I2 = 0., I3 = 0.;

		int j = 0;
#pragma omp parallel for shared(a1, b1, a2, N, h2) private(j) reduction(+:I3,I2)
		for (j = 1; j < N; j += 2)
		{
			I3 += Simpson_integration(F, a1, b1, N, a2 + j*h2);
			if (j != 1)
				I2 += Simpson_integration(F, a1, b1, N, a2 + (j - 1)*h2);
		}
		t2 = omp_get_wtime();
		//ti2 = tbb::tick_count::now();
		double s = sign * (h2 / 3) * (Simpson_integration(F, a1, b1, N, a2) + 2 * I2 + 4 * I3 + Simpson_integration(F, a1, b1, N, b2));
		//return (ti2 - ti1).seconds() * 1000.;
		return (t2 - t1) * 1000.;
	}


	double Simpson2_TBB_parallel_test(double(*F)(double, double), double a1, double b1, double a2, double b2, int n)
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

		//tbb::tick_count ti1, ti2;
		//ti1 = tbb::tick_count::now();
		double t1 = 0., t2 = 0.;
		t1 = omp_get_wtime();

		tbb::task_scheduler_init init(4);
		simpson_tbb_reduce stbbr(F, a1, b1, N, a2, h2);

		tbb::parallel_reduce(tbb::blocked_range<int>(1, N, 5), stbbr);
		init.terminate();
		t2 = omp_get_wtime();
		//ti2 = tbb::tick_count::now();

		double s = sign * (h2 / 3) * (Simpson_integration(F, a1, b1, N, a2) + 2 * stbbr.getI2() + 4 * stbbr.getI3() + Simpson_integration(F, a1, b1, N, b2));
		//return (ti2 - ti1).seconds() * 1000.;
		return (t2 - t1) * 1000.;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	inline double func2(double x, double y)
	{
		return x*y*y;
	}

	inline double func1(double x, double y)
	{
		return (x*x*y - x) / sqrt(x + y*y*x);
	}


	void Test_serial_code_simpson()
	{
		int num_of_experiments = 10;
		std::cout << "\n *********** Hello from a function: run sub_main() ****************\n";
		std::cout << " please, enter the number of experiments.. ";
		std::cin >> num_of_experiments;

		int n = 1000;
		double t1 = 0., t2 = 0.;
		double integration0 = 0., integration1 = 0., integration2 = 0.;

		t1 = omp_get_wtime();
		for (int i = 0; i < num_of_experiments; ++i)
		{
			integration0 += Simpson2_test0(&func2, 1, 2, 2, 3, n);
			integration0 += Simpson2_test0(&func1, 1, 2, 2, 3, n);

			integration1 += Simpson2_test1(&func2, 1, 2, 2, 3, n);
			integration1 += Simpson2_test1(&func1, 1, 2, 2, 3, n);

			integration2 += Simpson2_test2(&func2, 1, 2, 2, 3, n);
			integration2 += Simpson2_test2(&func1, 1, 2, 2, 3, n);
		}
		t2 = omp_get_wtime();
		integration0 /= 2*num_of_experiments;
		integration1 /= 2*num_of_experiments;
		integration2 /= 2*num_of_experiments;

		std::cout.precision(15);
		std::cout << "\n average time: ";
		std::cout << "\nSimpson0 integrate = " << integration0 << "ms";
		std::cout << "\nSimpson1 integrate = " << integration1 << "ms";
		std::cout << "\nSimpson2 integrate = " << integration2 << "ms";
		std::cout << "\n total time work = " << (t2 - t1) * 1000. << "ms";
		std::cout.precision(6);

		std::cout << "\n ***************************\n";
		return;
	}

	void Test_serial_vs_parallel()
	{
		int num_of_experiments = 10;
		std::cout << "\n *********** Hello from a function:  Test_serial_vs_parallel() ****************\n";
		std::cout << " please, enter the number of experiments.. ";
		std::cin >> num_of_experiments;

		int n = 1000;
		double t1 = 0., t2 = 0.;
		double integration0 = 0., integration1 = 0., integration2 = 0.;
		double par_integration0 = 0., par_integration1 = 0.;

		tbb::tick_count ti1, ti2;
		t1 = omp_get_wtime();
		ti1 = tbb::tick_count::now();
		for (int i = 0; i < num_of_experiments; ++i)
		{
			integration0 += Simpson2_test0(&func2, 1, 2, 2, 3, n);
			integration0 += Simpson2_test0(&func1, 1, 2, 2, 3, n);

			integration1 += Simpson2_test1(&func2, 1, 2, 2, 3, n);
			integration1 += Simpson2_test1(&func1, 1, 2, 2, 3, n);

			integration2 += Simpson2_test2(&func2, 1, 2, 2, 3, n);
			integration2 += Simpson2_test2(&func1, 1, 2, 2, 3, n);

			par_integration0 += Simpson2_OMP_parallel_test(&func2, 1, 2, 2, 3, n);
			par_integration0 += Simpson2_OMP_parallel_test(&func1, 1, 2, 2, 3, n);
			par_integration1 += Simpson2_TBB_parallel_test(&func2, 1, 2, 2, 3, n);
			par_integration1 += Simpson2_TBB_parallel_test(&func1, 1, 2, 2, 3, n);
		}
		t2 = omp_get_wtime();
		ti2 = tbb::tick_count::now();

		integration0 /= 2 * num_of_experiments;
		integration1 /= 2 * num_of_experiments;
		integration2 /= 2 * num_of_experiments;

		par_integration0 /= 2 * num_of_experiments;
		par_integration1 /= 2 * num_of_experiments;

		std::cout.precision(15);
		std::cout << "\n average time: ";
		std::cout << "\nSimpson0 integrate = " << integration0 << "ms";
		std::cout << "\nSimpson1 integrate = " << integration1 << "ms";
		std::cout << "\nSimpson2 integrate = " << integration2 << "ms";
		std::cout << "\n";
		std::cout << "\nSimpson_OMP integrate = " << par_integration0 << "ms";
		std::cout << "\nSimpson_TBB integrate = " << par_integration1 << "ms";
		std::cout << "\n";
		std::cout << "\nSpeedup  Simpson0 / OMP = " << integration0 / par_integration0;
		std::cout << "\nSpeedup  Simpson1 / OMP = " << integration1 / par_integration0;
		std::cout << "\nSpeedup  Simpson2 / OMP = " << integration2 / par_integration0;
		std::cout << "\n";
		std::cout << "\nSpeedup Simpson0 / TBB = " << integration0 / par_integration1;
		std::cout << "\nSpeedup Simpson1 / TBB = " << integration1 / par_integration1;
		std::cout << "\nSpeedup Simpson2 / TBB = " << integration2 / par_integration1;

		std::cout << "\n Omptimer total time work = " << (t2 - t1) * 1000. << "ms";
		std::cout << "\n TBBtimer total time work = " << (ti2 - ti1).seconds() * 1000. << "ms";
		std::cout.precision(6);

		std::cout << "\n ***************************\n";
		return;

	}

	void Test_ser_vs_par()
	{
		int num_of_experiments = 10;
		std::cout << "\n *********** Hello from a function:  Test_ser_vs_par() ****************\n";
		std::cout << " please, enter the number of experiments.. ";
		std::cin >> num_of_experiments;

		int n = 1000;
		double t1 = 0., t2 = 0.;
		double tt1 = 0., tt2 = 0.;
		double integration1 = 0.;
		double par_integration0 = 0., par_integration1 = 0.;

		tbb::tick_count ti1, ti2;
		t1 = omp_get_wtime();
		ti1 = tbb::tick_count::now();
		for (int i = 0; i < num_of_experiments; ++i)
		{
			tt1 = omp_get_wtime();
				Simpson2(&func2, 1, 2, 2, 3, n);
			tt2 = omp_get_wtime();
			integration1 += (tt2 - tt1)* 1000.;

				tt1 = omp_get_wtime();
					Simpson2_OMP_parallel(&func2, 1, 2, 2, 3, n);
				tt2 = omp_get_wtime();
				par_integration0 += (tt2 - tt1)* 1000.;

				tt1 = omp_get_wtime();
					Simpson2_TBB_parallel(&func2, 1, 2, 2, 3, n);
				tt2 = omp_get_wtime();
				par_integration1 += (tt2 - tt1)* 1000.;

		}
		t2 = omp_get_wtime();
		ti2 = tbb::tick_count::now();

		integration1 /= 1 * num_of_experiments;

		par_integration0 /= 1 * num_of_experiments;
		par_integration1 /= 1 * num_of_experiments;

		std::cout.precision(15);
		std::cout << "\n average time: ";
		std::cout << "\nSimpson0 integrate = " << integration1 << "ms";
		std::cout << "\n";
		std::cout << "\nSimpson_OMP integrate = " << par_integration0 << "ms";
		std::cout << "\nSimpson_TBB integrate = " << par_integration1 << "ms";
		std::cout << "\n";
		std::cout << "\nSpeedup  Simpson0 / OMP = " << integration1 / par_integration0;
		std::cout << "\nSpeedup Simpson0 / TBB = " << integration1 / par_integration1;

		std::cout << "\n Omptimer total time work = " << (t2 - t1) * 1000. << "ms";
		std::cout << "\n TBBtimer total time work = " << (ti2 - ti1).seconds() * 1000. << "ms";
		std::cout.precision(6);

		std::cout << "\n ***************************\n";
		return;
	}

}
