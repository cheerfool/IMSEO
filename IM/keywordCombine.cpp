#include"keywordCombine.h"

double maxInflu = 0;;
bool found = false;

int keyword_pattern_tried = 0;
void AssignTopicAwareEdgeWeight(InfGraph& g, Keywords& words, Query& Q){
	//!!!
	set<int> qwords(Q.keywords.begin(), Q.keywords.end());
	vector<bitset<BITLENGTH>> tags;
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
			else if (*it1 > *it2){
				it2++;
			}
			else{
				it1++;
				counter++;
			}
		}
		tags.push_back(tagStatus);
	}
	vector<double> inProbs(g.size(), 0);
	for (unsigned i = 0; i < g.size(); i++){
		for (unsigned j = 0; j < g[i].neighbors.size(); j++){
			int neighborID = g[i].neighbors[j].first;
			//double prob1 = g[i].neighbors[j].second* tags[i].count() / qwords.size() * tags[neighborID].count() / qwords.size();
			double prob1 = 1.0* tags[i].count() * tags[neighborID].count() / qwords.size()/qwords.size();
			//double prob1 = 0.5 * (1.0* tags[i].count() / qwords.size() + 1.0* tags[neighborID].count()/qwords.size());
			bitset<BITLENGTH> tmpbit = tags[i] & tags[neighborID];
			prob1 *= tmpbit.count() / qwords.size();
			//prob1 = pow(prob1, 1.0/3);
			//prob1 = sqrt(prob1);
			if (prob1 > small_probs){
				g[i].neighbors[j].second = prob1;
			}
			else{
				g[i].neighbors[j].second = small_probs;
			}
			inProbs[g[i].neighbors[j].first] += g[i].neighbors[j].second;
		}
	}
	for(unsigned i = 0; i<g.size(); i++){
		for(unsigned j=0; j<g[i].neighbors.size(); j++){
			g[i].neighbors[j].second = g[i].neighbors[j].second / inProbs[g[i].neighbors[j].first];
		}
	}
	set<int> sets;
	for(unsigned i=0; i<g.size(); i++){
		if(tags[i].any()){
			sets.insert(tags[i].to_ulong());
		}
	}

}

void InitialQuery(Query& Q, set<int>& keywords, int k){
	//!!!
	Q.keywords.assign(keywords.begin(), keywords.end());
	for(set<int>::iterator it = keywords.begin();
	it != keywords.end();
	++it){
		Q.covered[*it] = 0;
	}
	Q.K = k;
}
void ReadKeywords(Keywords& words, string filename){
	//!!!
	ifstream ifs(filename);
	int nodeNum, wordNum, w;
	ifs>>nodeNum;
	for(unsigned i = 0; i < nodeNum; ++i){
		ifs>>wordNum;
		set<int> Ws;
		for(unsigned j = 0; j < wordNum; ++j){
			ifs>>w;
			Ws.insert(w);
		}
		words.push_back(Ws);
	}
	ifs.close();
}
