// labapp4.cpp: определяет точку входа для консольного приложения.
//
#include <stack>
#include <ctime>
#include <stdlib.h> // abs, rand, srand
#include <math.h> // sqrt, pow

#include <iostream>
#include <omp.h>

using namespace std;

struct Point
{
	int X, Y;
	bool compare (Point &b)
	{
		return X == b.X && Y == b.Y;
	}
};

void RANDOM_POINT(Point *PH, int k)
{
	srand(unsigned(time(NULL)));
	for (int i = 0; i < k; i++)
	{
		PH[i].X = rand() % (k);
		PH[i].Y = rand() % (k);
	}
	if (k <= 10)
	{
		cout << "Точки: ";
		for (int i = 0; i<k; i++)
			cout << '(' << PH[i].X << ',' << PH[i].Y << ')';
	}
	cout << endl;
}

double area_triangle(Point a, Point b, Point c)
{
	return 0.5 * (a.X * b.Y + b.X * c.Y + c.X * a.Y - a.Y * b.X - b.Y* c.X - c.Y * a.X);
}

//const double eps = 1e-8;
int min(int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}

int max(int a, int b)
{
	if (a > b)
		return a;
	else
		return b;
}

bool point_in_box(Point t, Point p1, Point p2)
{
	double eps = 1e-8;

	return  (abs(t.X - min(p1.X, p2.X)) <= eps || min(p1.X, p2.X) <= t.X) &&
		(abs(max(p1.X, p2.X) - t.X) <= eps || max(p1.X, p2.X) >= t.X) &&
		(abs(t.Y - min(p1.Y, p2.Y)) <= eps || min(p1.Y, p2.Y) <= t.Y) &&
		(abs(max(p1.Y, p2.Y) - t.Y) <= eps || max(p1.Y, p2.Y) >= t.Y);
}

int Jarvis(const Point * p, int n, double &time, Point *&Hull) //линейная часть
{
	stack <Point> ip;
	int first, q1, next1;
	double sign;

	double t_start, t_end;
	t_start = omp_get_wtime();
	
	// находим самую нижнюю из самых левых точек
	first = 0;
	for (int i = 0; i < n; i++)
	if (p[i].X < p[first].X || (p[i].X == p[first].X && p[i].Y < p[first].Y))
		first = i;
	
	q1 = first; // текущая точка

	do
	{
		// добавляем точки в оболочку
		ip.push(p[q1]);
		next1 = q1;

		// ищем следующую точку
		for (int i = n - 1; i >= 0; --i)
		if (p[i].X != p[q1].X || p[i].Y != p[q1].Y)
		{
			sign = area_triangle(p[q1], p[i], p[next1]);
			if (next1 == q1 || sign > 0 || (sign == 0 && point_in_box(p[next1], p[q1], p[i])))
				next1 = i;
		}
		q1 = next1;

	} while (q1 != first);

	t_end = omp_get_wtime();
	time = t_end - t_start;

	int hullsize = ip.size();
	cout << endl << "Number of points in MBO: " << hullsize << endl;

	Hull = new Point[hullsize];
	for (int i = 0; i < hullsize; i++)
	{
		Hull[i] = ip.top();
		ip.pop();
	}

	if (n < 10)
	{
		cout << "MBO: ";
		for (int i = 0; i<hullsize; i++)
		{
			cout << '(' << Hull[i].X << ',' << Hull[i].Y << ")  ";
		}
		cout << endl;
	}
	return hullsize;
}

int JarvisOpenMp(const Point * p, int n, double &time, Point *&Hull) //open mp
{
	stack <Point> ip;
	int first, q, next;
	double sign;

	double t_start, t_end;
	t_start = omp_get_wtime();
	// находим самую нижнюю из самых левых точек
	first = 0;

	omp_lock_t lock;
	omp_init_lock(&lock);
	int i;
#pragma omp parallel for shared(p, n, first) private(i)
	for (i = 0; i < n; i++)
	{
		//cout << "Кол-во потоков : " << omp_get_num_threads() << endl;
		if (p[i].X < p[first].X || (p[i].X == p[first].X && p[i].Y < p[first].Y))
		{
			omp_set_lock(&lock);
			if (p[i].X < p[first].X || (p[i].X == p[first].X && p[i].Y < p[first].Y))
				first = i;
			omp_unset_lock(&lock);
		}
	}
	q = first; // текущая точка
	// добавляем точки в оболочку
	do
	{
		ip.push(p[q]);
		next = q;
		// ищем следующую точку
#pragma omp parallel for shared(p, n, next) private(i, sign)
		for (i = n - 1; i >= 0; --i)
		{
			if (p[i].X != p[q].X || p[i].Y != p[q].Y)
			{
				sign = area_triangle(p[q], p[i], p[next]);

				//if (next == q || sign > 0 || (sign == 0))
				if (next == q || sign > 0 || (sign == 0 && point_in_box(p[next], p[q], p[i])))
				{
					omp_set_lock(&lock);
					sign = area_triangle(p[q], p[i], p[next]);
					if (next == q || sign > 0 || (sign == 0 && point_in_box(p[next], p[q], p[i])))
						next = i;
					omp_unset_lock(&lock);
				}

			}
		}
		q = next;
	} while (q != first);
	omp_destroy_lock(&lock);

	t_end = omp_get_wtime();
	time = t_end - t_start;
	
	int hullsize = ip.size();
	cout << endl << "The number of points in MBO : " << hullsize << endl;
	
	Hull = new Point[hullsize];
	for (int i = 0; i < hullsize; i++)
	{
		Hull[i] = ip.top();
		ip.pop();
	}
	
	if (n < 10)
	{
		cout << "МВО: ";
		for (int i = 0; i<hullsize; i++)
		{
			cout << '(' << Hull[i].X << ',' << Hull[i].Y << ")  ";
		}
		cout << endl;
	}
	return hullsize;
}


void Test(int GIFT_Size1, int GIFT_Size2, Point ConvexHull_GIFT1[], Point ConvexHull_GIFT2[])
{
	bool isError = false;
	if (GIFT_Size1 != GIFT_Size2)
	{
		cout << "Error!" << endl;
		isError = true;
	}
	else
	{
		for (int i = 0; i < GIFT_Size1; i++)
			if (!ConvexHull_GIFT1[i].compare(ConvexHull_GIFT2[i]))
			{
				cout << "Error! Points aren't equal !" << endl;
				isError = true;
				break;
			}
		
	}

	if (isError != true)
		cout << "correctly!" << endl;
}


int main(int argc, char *argv[])
{
	int ThreadNum = 2;
	if(argc > 1){
		ThreadNum = atoi(argv[1]);	
	}

	std::cout << "ThreadNum = " << ThreadNum << std::endl;

	if(!(ThreadNum>0)){
		std::cout <<"ThreadNum should be > 0 " << std::endl;
		return 1;
	}

	int  k = 10; // количество строк и столбцов., должно быть чётное!	
	if(argc > 2){
		k = atoi(argv[2]);	
	}
	std::cout << "You entered the number of points: " << k << std::endl;
	if(!(k>0)){
		std::cout <<"the number of points should be > 0 " << std::endl;
		return 2;
	}

	omp_set_num_threads(ThreadNum);


	//omp_set_num_threads(2);
	//setlocale(LC_ALL, "Russian");
	double GIFT_TIME1 = 0, GIFT_TIME2 = 0;
	int GIFT_Size1, GIFT_Size2;
	Point *ConvexHull_GIFT1 = NULL;
	Point *ConvexHull_GIFT2 = NULL;

	//int k = 0;
	//cout << "Enter the number of points: ";
	//cin >> k;
	cout << endl;
	Point *GIFT = new Point[k];

	RANDOM_POINT(GIFT, k);

	GIFT_Size1 = Jarvis(GIFT, k, GIFT_TIME1, ConvexHull_GIFT1);
	GIFT_Size2 = JarvisOpenMp(GIFT, k, GIFT_TIME2, ConvexHull_GIFT2);

	Test(GIFT_Size1, GIFT_Size2, ConvexHull_GIFT1, ConvexHull_GIFT2);

	cout << "Number of threads: " << omp_get_num_threads() << endl;
	cout << "Time of work linear algorithm: " << GIFT_TIME1* 1000.0 <<"ms"<< endl;
	cout << "Time of work parallel OpenMP algorithm: " << GIFT_TIME2* 1000.0<<"ms" << endl;
	cout << "Speed up " << GIFT_TIME1 / GIFT_TIME2 << endl;

	//system("pause");
	delete[] ConvexHull_GIFT1;
	delete[] ConvexHull_GIFT2;
	delete[] GIFT;
	return 0;
}


