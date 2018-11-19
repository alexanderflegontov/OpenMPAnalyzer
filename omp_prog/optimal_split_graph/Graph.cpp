#include "Graph.h"

#define PART_A (1)
#define PART_B (2)

// Sorting of deficits according to the second element
struct sort_def {
	bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right) {
		return left.second > right.second;
	}
};

Graph::Graph(int size, int probty) {
	Graph::v_size = size;
	Graph::half = Graph::v_size / 2;
	Graph::e_size = 0;
	totalGain = 0;
	edge = new bool*[Graph::v_size];
	part = new int[Graph::v_size];
	gain = new int[Graph::half + 1];

	// Create matrix
	for (int i(0); i < Graph::v_size; i++) {
		edge[i] = new bool[Graph::v_size];
		for (int j(0); j < Graph::v_size; j++)
			edge[i][j] = false;
	}

	// Generate matrix
	int prob;
	for (int i(0); i < Graph::v_size; i++){
		for (int j(i + 1); j < Graph::v_size; j++) {
			prob = rand() % 100;
			if (prob < probty) {
				edge[i][j] = true;
				edge[j][i] = true;
				Graph::e_size++;
			}
		}
	}
}

Graph::~Graph() {
	delete[] part;
	delete[] gain;
	for (int i(0); i < Graph::v_size; i++)
		delete[] edge[i];
	delete edge;
}

void Graph::print_matrix() {
	cout << "Graph has " << e_size << " edges\n\n";
	for (int i(0); i < Graph::v_size; i++) {
		for (int j(0); j < Graph::v_size; j++)
			cout << edge[i][j] << ' ';
		cout << endl;
	}
	cout << endl;
}

// Print current parts of splitting
void Graph::print_parts() {
	for (int i(1); i < 3; i++){
		cout << i << " part:\n";
		for (int j(0); j < Graph::v_size; j++){
			if (part[j] == i)
				cout << j << ' ';
		}
		cout << endl;
	}
	cout << endl;
}

// Print couples of deficits
void Graph::print_defs() {
	if (!def_a.empty() && !def_b.empty()){
		for (vector<pair<int, int> >::iterator it = def_a.begin(); it != def_a.end(); ++it)
			cout << it->first << ":" << it->second << " ";
		cout << endl;
		for (vector<pair<int, int> >::iterator it = def_b.begin(); it != def_b.end(); ++it)
			cout << it->first << ":" << it->second << " ";
		cout << endl << endl;
	}
}

void Graph::print_gains() {
	for (int i(1); i < Graph::v_size / 2 + 1; i++)
		cout << gain[i] << " ";
	cout << endl;
}

void Graph::create_parts(){

	for (int i= 0; i < Graph::v_size; i++)
		part[i] = 0;

	int k = 0;
	do
	{
		for (int j(0); j < Graph::v_size; j++)
		{
			int ch = rand() % 100;
			if (ch > 50 && part[j] != PART_A)
			{
				part[j] = PART_A;
				k++;
			}
			if (k == Graph::half)
				break;
		}
	} while (k < Graph::half);

	for (int i(0); i < Graph::v_size; i++){
		if (part[i] != PART_A)
			part[i] = PART_B;
	}

}

// Calculate deficits
void Graph::def_calc() {
	for (int i(0); i <= Graph::half; i++)
		gain[i] = 0;

	def_a.clear();
	def_b.clear();

	int def;
	for (int i(0); i < Graph::v_size; i++) 
	{
		def = 0;
		for (int j(0); j < Graph::v_size; j++)
		{
			if (edge[i][j])
			{
				if (part[i] == part[j]) 
					def--; 
				else def++;
			}
		}
		if (part[i] == PART_A)
			def_a.push_back(std::make_pair(i, def));
		else def_b.push_back(std::make_pair(i, def));
	}
}

// Recalculate deficits after couple exchange
void Graph::def_recalc() {
	for (vector<pair<int, int> >::iterator it = def_a.begin(); it != def_a.end(); ++it)
		it->second += 2 * edge[it->first][last_a] - 2 * edge[it->first][last_b];
	for (vector<pair<int, int> >::iterator it = def_b.begin(); it != def_b.end(); ++it)
		it->second += 2 * edge[it->first][last_b] - 2 * edge[it->first][last_a];
}

// Ordering deficiencies after couple exchange
void Graph::def_balance() {
	std::sort(def_a.begin(), def_a.end(), sort_def());
	std::sort(def_b.begin(), def_b.end(), sort_def());
}

// Exchange of couple of vertexes
bool Graph::exchange_pair(int stage)
{
	int tgain;
	last_a = def_a.front().first;
	last_b = def_b.front().first;
	tgain = def_a.front().second + def_b.front().second - 2 * edge[last_a][last_b];
	if (tgain > 0){
		def_a.erase(def_a.begin());
		def_b.erase(def_b.begin());

		gain[stage] = gain[stage - 1] + tgain;
		totalGain += tgain;
		if (part[last_a] == PART_A){
			part[last_a] = PART_B;
			part[last_b] = PART_A;
		}
		else{
			part[last_a] = PART_A;
			part[last_b] = PART_B;
		}
		cout << "Vertexes " << last_a << " and " << last_b << " have been exchanged!" << endl;
		return true;
	}
	return false;
}


//Calculate total benefit after a series of couple exchanges
int Graph::total_gain(){
	return totalGain;
}

//Returns array with total gain and parts
int* Graph::create_result(){

	int* res = new int[Graph::v_size+1];
	res[Graph::v_size] = this->cut_size();

	for (int i(0); i < Graph::v_size; i++)
		res[i] = part[i];
	return res;
}

int Graph::cut_size(){
	int size = 0;
	for (int i(0); i < Graph::v_size; i++)
	for (int j(0); j < Graph::v_size; j++)
	{
		if (edge[i][j] && (part[i] != part[j]))
			size++;
	}
	return size / 2;
}