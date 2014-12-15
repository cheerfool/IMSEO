#ifndef UTILITY_H
#define UTILITY_H

#include<set>
#include<map>
#include<vector>
#include<bitset>
#include<list>
#include<queue>
#include<fstream>
#include<string>
#include<iostream>
#include<cmath>

using namespace std;

extern int update_repeat_times;

extern double alpha_theta;

extern bool flag_for_debug;

extern double IRIE_alpha;

extern double IRIE_theta;

extern double IRIE_min_stopping_criteria;

const int BITLENGTH = 16;

typedef vector<pair<int, double>> Neighbor;

enum PPType{
	TopicPP,
	Random,
	Trained,
	Degree
};
enum IFCType{
	IRIE,
	MCSimulation
};
typedef struct _para{
	int propagation_probability_type;
	int influence_computation_type;
}_para;

typedef struct Node{
	Neighbor neighbors;
	int indegree;
	double influ;
	double oldInflu;
	double para;
	double alpha;
	double IRIE_para;
	bool valid;
	bitset<BITLENGTH> bits;
}Node;

typedef struct Action{
	int movie_id;
	int date;
}Action;

typedef vector<Node> InfGraph;

typedef pair<int, double> Path;

void InitialGraph(InfGraph& g, InfGraph& rg);

void ConstructGraph(string filename, InfGraph& g);

void ConstructProbGraph(string filename, InfGraph& g);




void AssignEdgeWeight(InfGraph& g);

void AssignEdgeWeightRandom(InfGraph& g);

void AdjustEdgeWeight(InfGraph& g);


void IRIEUpdate(InfGraph& g, vector<double>& r, vector<double>& APs, vector<int>& S);

void EstimateAPs(int node, InfGraph& g, vector<double>& APs);


void trainProbs(string filename, InfGraph& g);

void trainProbs2(string filename, InfGraph& g);

void trainProbs3(string filename, InfGraph& g);

#endif