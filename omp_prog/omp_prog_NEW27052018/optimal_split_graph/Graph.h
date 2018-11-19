#include <vector>
#include <iostream>
#include <algorithm>
#include "time.h"
#include "math.h"

using namespace std;

class Graph {
	int last_a, last_b, totalGain, half;
	int v_size, e_size;
	bool **edge;
	int *part, *gain;
	vector <pair<int, int> > def_a, def_b;
public:
	Graph(int size, int probty);
	~Graph();
	void print_matrix();
	void print_parts();
	void print_defs();
	void print_gains();
	void def_calc();
	void def_recalc();
	void def_balance();
	bool exchange_pair(int stage);
	int max_gain();
	int total_gain();
	void create_parts();
	int* create_result();
	int cut_size();
};
