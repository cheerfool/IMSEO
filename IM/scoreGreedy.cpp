#include"scoreGreedy.h"

void initial(InfGraph& g, Keywords& words, Query& Q, vector<GREEDY::QueueEle>& queue){
	set<int> qwords(Q.keywords.begin(), Q.keywords.end());
	for (unsigned i = 0; i < g.size(); i++){
		set<int>::iterator it1, it2;
		it1 = qwords.begin();
		it2 = words[i].begin();
		unsigned counter = 0;
		bitset<BITLENGTH> tagStatus(0);
		while (it1 != qwords.end() && it2 != words[i].end()){
			if (*it1 == *it2){
				tagStatus[counter] = 1;
				it1++;
				it2++;
				counter++;
			}
			else if(*it1 > *it2){
				it2++;
			}
			else{
				it1++;
				counter++;
			}			
		}
		if (!tagStatus.any())continue;
		GREEDY::QueueEle e;
		e.tagState = tagStatus;
		e.nodeID = i;
		e.step = 0;
		queue.push_back(e);
	}
}
unsigned ScoreGreedySim(InfGraph& g, Keywords& words, Query& Q, vector<int>& seeds, double combination_alpha){
	vector<GREEDY::QueueEle> elelist;
	initial(g, words, Q, elelist);
	cout<<"initialed"<<endl;
	bitset<BITLENGTH> querytags((1 << Q.keywords.size()) - 1);
	vector<bool> selected(elelist.size(), false);

	for (unsigned i = 0; i < Q.K; i++){
		cout<<"round "<<i<<endl;
		vector<int> ids;
		for(unsigned j = 0; j < elelist.size(); j++){
			int nodeID = elelist[j].nodeID;
			if(!selected[j]){
				ids.push_back(nodeID);
			}
		}
		vector<double> influIncrease(g.size(), 0);
		FastSimInfluence(g, seeds, ids, influIncrease);
		double maxInflu = influIncrease[ids[0]];
		int maxCoverNum = 0;
		for(unsigned j=0; j<elelist.size(); j++){
			int nodeID = elelist[j].nodeID;
			elelist[j].tagState &= querytags;
			if(elelist[j].tagState.count() > maxCoverNum){
				maxCoverNum = elelist[j].tagState.count();
			}
			elelist[j].influ = influIncrease[nodeID];
			if(maxInflu < influIncrease[nodeID]){
				maxInflu = influIncrease[nodeID];
			}
		}
		cout<<"maxCoverNum is "<<maxCoverNum<<" maxInflu is "<<maxInflu<<endl;
		int maxScoreLoc = -1;
		double maxScore = 0;
		for(unsigned j=0; j<elelist.size(); j++){
			if(selected[j])continue;
			double score;
			if(querytags.count() == 0){
				score =  combination_alpha+(1-combination_alpha)*elelist[j].influ / maxInflu;
			}
			else{
				score = combination_alpha * elelist[j].tagState.count() / maxCoverNum + (1-combination_alpha )* elelist[j].influ / maxInflu;
			}
			if(score > maxScore){
				maxScore = score;
				maxScoreLoc = j;
			}
		}
		cout<<"maxScore is "<<maxScore<<" tagCount is "<<elelist[maxScoreLoc].tagState.count()<<endl;
		if(maxScoreLoc == -1){
			seeds.clear();
			return querytags.count();
		}
		seeds.push_back(elelist[maxScoreLoc].nodeID);
		selected[maxScoreLoc] = true;
		querytags = querytags & (~elelist[maxScoreLoc].tagState);
	}
	if (querytags.count() != 0){
		seeds.clear();
	}
	return querytags.count();
}

unsigned PigeonGreedySim(InfGraph& g, Keywords& words, Query& Q, vector<int>& seeds){
	vector<GREEDY::QueueEle> elelist;
	initial(g, words, Q, elelist);
	vector<bool> selected(elelist.size(), false);
	bitset<BITLENGTH> querytags((1 << Q.keywords.size()) - 1);

	for(unsigned i = 0; i < Q.K; i++){
		
		int bound = (querytags.count() + Q.K - i - 1) / (Q.K - i);
		cout<<"round "<<i<<" bound is "<<bound<<endl;
		vector<int> ids;
		for(unsigned j=0; j<elelist.size(); j++){
			if(selected[j])continue;
			ids.push_back(elelist[j].nodeID);
		}
		vector<double> influIncrease(g.size(), 0);
		FastSimInfluence(g, seeds, ids, influIncrease);
		for(unsigned j=0; j<elelist.size(); j++){
			elelist[j].influ = influIncrease[elelist[j].nodeID];
		}
		double maxInflu = 0;
		int maxInfluNode = -1;
		int maxInfluNodeLoc = -1;
		for(unsigned j=0; j<elelist.size(); j++){
			if(selected[j])continue;
			bitset<BITLENGTH> tmpbits = elelist[j].tagState & querytags;
			if(tmpbits.count() >= bound){
				if(elelist[j].influ > maxInflu){
					maxInflu = elelist[j].influ;
					maxInfluNode = elelist[j].nodeID;
					maxInfluNodeLoc = j;
				}
			}
		}
		if(maxInfluNode == -1){
			seeds.clear();
			return querytags.count();
		}
		cout<<"maxInflu is "<<maxInflu<<" maxInfluNode is "<<maxInfluNode<<endl;
		bitset<BITLENGTH> tmp = elelist[maxInfluNodeLoc].tagState & querytags;
		cout<<"cover "<<tmp.count()<<" tags"<<endl;
		seeds.push_back(maxInfluNode);
		querytags = querytags & (~elelist[maxInfluNodeLoc].tagState);
		selected[maxInfluNodeLoc] = true;
	}
	return 0;
}