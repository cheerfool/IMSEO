#ifndef SCORE_GREEDY_H
#define SCORE_GREEDY_H
#include"MCSim.h"
#include"keywordCombine.h"
namespace GREEDY{
	typedef struct QueueEle{
		bitset<BITLENGTH> tagState;
		double score;
		double influ;
		int nodeID;
		unsigned step;
	};
}

unsigned ScoreGreedySim(InfGraph& g, Keywords& words, Query& Q, vector<int>& seeds, double combination_alpha);

unsigned PigeonGreedySim(InfGraph& g, Keywords& words, Query& Q, vector<int>& seeds);
#endif