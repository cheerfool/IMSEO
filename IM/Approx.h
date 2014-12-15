#ifndef APPROX_H
#define APPROX_H
#include"keywordCombine.h"
#include"MCSim.h"
enum BITLENGTH{
	TAG_BIT_LENGTH = 20,
	TYPE_BIT_LENGTH = 20
};
namespace APPROX{
	typedef struct NodeHouseUnit{
		bitset<BITLENGTH> tagStatus;
		double maxInflu;
		vector<pair<double, int>> nodes;
	}NodeHouseUnit;
	
	typedef struct NodeHouse{
		vector<NodeHouseUnit> tagSets;
		vector<unsigned> seeds;
		vector<int> seeds1;
		double maxInflu;
		double influ1;
		int QK;
		unsigned queryTag;
		unsigned queryTagNum;
		int invoke_times;
		vector<double> r, APs;
	}NodeHouse;

	typedef struct para{
		int edgeWeightType;
		int influenceEvaluationType;
	}para;

	typedef struct CacheUnit{
		bitset<BITLENGTH> tagState;
		vector<int> extra_times;
		vector<vector<bitset<BITLENGTH>>> tagCombinations;
		vector<unsigned> types;
		double upperBound;
		vector<double> upper;
	}CacheUnit;
	typedef struct Cache{
		unsigned type_num;
		list<CacheUnit> cacheNodes;
		vector<int> lists;
		vector<int> times;

	}Cache;

	typedef struct Result{
		vector<int> seeds;
		vector<double> infOfNodes;
		double influence;
		vector<double> influOfTopMNodes;
		vector<int> freeSet;
		vector<double> freeSetInflu;
		int invoke_time;
	}Result;
	typedef vector<vector<NodeHouseUnit*>> CoverNumList;

}

void initial(InfGraph& g, Keywords& words, Query& Q);

void selectSeedsWithNoConstraintsIRIE(InfGraph& g, Keywords& words, Query& Q, int k, APPROX::Result& result);

double selectSeedsWithConstraintsIRIE(InfGraph& g, Keywords& words, Query& Q, vector<int>& s, double influ, vector<double>& r, vector<double>& APs, APPROX::Result& result);

double selectSeedsWithConstraintsSim(InfGraph& g, Keywords& words, Query& Q, vector<int>& s, double influ, APPROX::Result& result, vector<vector<double>*>& old_influs);

void constructNodeHouse(InfGraph& g, Query& Q, APPROX::NodeHouse& nhouse);

void constructNodeHouse(InfGraph& g, Query& Q, APPROX::NodeHouse& nhouse, vector<int>& s, vector<double>& r);

void constructNodeHouseSim(InfGraph& g, Query& Q, APPROX::NodeHouse& nhouse, vector<int>& s, vector<vector<double>*>& old_influ);

void constructCoverList(APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist);

void checkAllCombinations(int size, int queryTagNum, InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, APPROX::Result& result);

void checkAllCombinationsSim(int size ,int queryTagNum, InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, APPROX::Result& result);

void combinationCheckIRIE(InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, vector<int>& starters, APPROX::Result& result);

void combinationCheckSim(InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, vector<int>& starters, APPROX::Result& result);

void selectSeeds(InfGraph& g, vector<unsigned>& types, APPROX::NodeHouse& nhouse, APPROX::Cache& myCache, APPROX::Result& result);

void selectSeedsSim(InfGraph& g, vector<unsigned>& types, APPROX::NodeHouse& nhouse, APPROX::Cache& myCache, APPROX::Result& result);

void checkTagSetSim(InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, APPROX::Cache& myCache, APPROX::NodeHouseUnit* ptr_nhouse, unsigned columnNum, unsigned coverNum, APPROX::Result& result);

void checkTagSet(InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, APPROX::Cache& myCache, APPROX::NodeHouseUnit* ptr_nhouse, unsigned columnNum, unsigned coverNum, APPROX::Result& result);

APPROX::Result approx1(InfGraph& g, Query& Q, Keywords& words);

APPROX::Result approx1Sim(InfGraph& g, Query& Q, Keywords& words);

APPROX::Result pics(InfGraph& g, Query& Q, Keywords& words);

void enumerate_partition(InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::Result& result);
#endif