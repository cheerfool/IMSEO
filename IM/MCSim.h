#ifndef MCSIM_H
#define MCSIM_H

#include"utility.h"

#include<vector>
#include<queue>
#include<time.h>
#include<boost/random.hpp>
#include<boost/random/mersenne_twister.hpp>
using std::vector;
using std::queue;
using namespace boost;

const int REPEAT_TIMES = 20000;

int MCSim(const InfGraph& g, vector<int>& seeds);

void initialGraph(InfGraph& g, vector<vector<bool>>& mark);

void generateGraph(InfGraph& g, vector<vector<bool>>& mark);

int findActivatedNodes(InfGraph& g, vector<vector<bool>>& mark, vector<int>& seeds, vector<bool>& influenced);

double MultiMCSim(vector<int>& seed, const InfGraph& g, int repeat_times = REPEAT_TIMES);

double FastSimInfluence(InfGraph& g, vector<int>& seeds);

void FastSimInfluence(InfGraph& g, vector<int>& S0, vector<int>& ids, vector<double>& influIncrease);
#endif