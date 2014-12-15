#include"utility.h"
#include"keywordCombine.h"
#include"Approx.h"

#include<bitset>
using std::bitset;
namespace WSC{
	typedef struct QueueEle{
		bitset<TAG_BIT_LENGTH> tagState;
		double score;
		unsigned nodeID;
		unsigned step;
	};

	class QECMP{
	public:
		bool operator()(const QueueEle& l, const QueueEle& r) const
		{
			return l.score < r.score;
		}
	};

	unsigned WSCSelect(InfGraph& g, Keywords& words, Query& Q, vector<int>& seeds, double combination_alpha);

	unsigned PigeonGreedy(InfGraph& g, Keywords& words, Query& Q, vector<int>& seeds);

	void initial(InfGraph& g, Keywords& words, Query& Q, vector<QueueEle>& queue);

	
}