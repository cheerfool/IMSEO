#ifndef KEYWORD_COMBINE_H
#define KEYWORD_COMBINE_H
#include<set>
#include<algorithm>
#include<queue>
#include<bitset>
#include"utility.h"
#include"MCSim.h"
using namespace std;

extern int keyword_pattern_tried;
extern bool found;
extern double maxInflu;


const double small_probs = 0.001;
typedef struct KeywordGroup{
	set<int> keywords;
	vector<pair<double, int>> nodes;
	vector<bool> selected;
	double maxInflu;
}KeywordGroup;

typedef struct TagList{
	set<int> tags;
	vector<int> nodes;
	double maxInflu;
}TL;
/*
typedef struct QueueEle{
	bitset<30> tagStatus;
	unsigned nodeID;
	double influ;
}QueueEle;*/

typedef vector<set<int>> Keywords;

typedef struct Query{
	vector<int> keywords;
	map<int, int> covered;
	int K;
}Query;


void AssignTopicAwareEdgeWeight(InfGraph& g, Keywords& words, Query& Q);

void InitialQuery(Query& Q, set<int>& keywords, int k);

void ReadKeywords(Keywords& words, string filename);


#endif