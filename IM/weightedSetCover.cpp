#include"weightedSetCover.h"
unsigned WSC::PigeonGreedy(InfGraph& g, Keywords& words, Query& Q, vector<int>& seeds){
	vector<QueueEle> elelist;
	for(int i=0; i<Q.keywords.size(); i++){
		cout<<Q.keywords[i]<<" ";
	}
	cout<<endl;
	initial(g, words, Q, elelist);
	//cout<<"initialed"<<endl;
	vector<double> r, APs;
	bitset<TAG_BIT_LENGTH> querytags((1<<Q.keywords.size()) -1);
	vector<bool> selected(g.size(), false);
	IRIEUpdate(g, r, APs, seeds);
	vector<bitset<TAG_BIT_LENGTH>> tmp;
	for(unsigned i=0; i<Q.K; i++){
		cout<<querytags.to_string()<<endl;
		int bound = (querytags.count()+Q.K-i-1)/(Q.K-i);
		cout<<"bound is "<<bound<<endl;
		double maxInf = -1;
		int maxInfNode = -1;
		int maxNodeLoc = -1;
		for(unsigned j=0; j<elelist.size(); j++){
			if(selected[elelist[j].nodeID])continue;
			bitset<TAG_BIT_LENGTH> tmpbits = elelist[j].tagState & querytags;
			if(tmpbits.count() >= bound && (!selected[elelist[j].nodeID])){
				//cout<<"r[elelist[j].nodeID] = "<<r[elelist[j].nodeID]<<endl;
				//cout<<"elelist[j].nodeID = "<<elelist[j].nodeID<<endl;
				//system("pause");
				if(r[elelist[j].nodeID] > maxInf){
					maxInf = r[elelist[j].nodeID];
					maxInfNode = elelist[j].nodeID;
					maxNodeLoc = j;
				}
			}
		}
		if(maxInfNode == -1){
			seeds.clear();
			break;
		}
		cout<<"maxInf "<<maxInf<<endl;
		//cout<<querytags.to_string()<<endl;
		//cout<<elelist[maxNodeLoc].tagState.to_string()<<endl;
		querytags = querytags & (~elelist[maxNodeLoc].tagState);
		//cout<<querytags.to_string()<<endl;
		//system("pause");
		seeds.push_back(maxInfNode);
		tmp.push_back(elelist[maxNodeLoc].tagState);
		selected[maxInfNode] = true;
		IRIEUpdate(g, r, APs, seeds);
	}
	return querytags.count();
}
unsigned WSC::WSCSelect(InfGraph& g, Keywords& words, Query& Q, vector<int>& seeds, double combination_alpha){
	vector<QueueEle> elelist;
	initial(g, words, Q, elelist);
	cout<<"initialed "<<elelist.size()<<endl;
	vector<double> r, APs;
	bitset<TAG_BIT_LENGTH> querytags((1<<Q.keywords.size()) -1);
	r.resize(g.size(), 1);
	APs.resize(g.size(), 0);
	IRIEUpdate(g, r, APs, seeds);
	double maxInflu = -1;
	unsigned maxCoverNum = 0;
	for(unsigned i=0; i<elelist.size(); i++){
		if(r[elelist[i].nodeID] > maxInflu){
			maxInflu = r[elelist[i].nodeID];
		}
		if(elelist[i].tagState.count() > maxCoverNum){
			maxCoverNum = elelist[i].tagState.count();
		}
	}
	while(seeds.size() < Q.K){
		double maxScore = -1;
		int maxNodeLoc = -1;
		for(unsigned i=0; i<elelist.size(); i++){
			double score;
			if(querytags.count() == 0){
				score = combination_alpha + (1-combination_alpha) * r[elelist[i].nodeID] / maxInflu;
			}
			else{
				score = 1.0 * ( combination_alpha) * elelist[i].tagState.count() / maxCoverNum + (1-combination_alpha) * r[elelist[i].nodeID] / maxInflu;
			}
			//cout<<score<<endl;
			//system("pause");
			if(score > maxScore){
				maxScore = score;
				maxNodeLoc = i;
			}
		}
		cout<<maxScore<<" "<<maxNodeLoc<<" "<<seeds.size()<<" "<<Q.K<<endl;
		if(maxNodeLoc == -1){
			cout<<"error"<<endl;
			exit(0);
		}
		seeds.push_back(elelist[maxNodeLoc].nodeID);
		//cout<<maxScore<<endl;
		//cout<<querytags.to_string()<<endl;
		//cout<<elelist[maxNodeLoc].tagState.to_string()<<endl;
		querytags = querytags & (~elelist[maxNodeLoc].tagState);
		//cout<<querytags.to_string()<<endl;
		//system("pause");
		IRIEUpdate(g, r, APs, seeds);
		maxScore = -1;
		maxCoverNum = 0;
		for(unsigned i=0; i<elelist.size(); ++i){
			elelist[i].tagState &= querytags;
			if(r[elelist[i].nodeID] > maxInflu){
				maxInflu = r[elelist[i].nodeID];
			}
			if(elelist[i].tagState.count() > maxCoverNum){
				maxCoverNum = elelist[i].tagState.count();
			}
		}
		if(querytags.count() != 0 && maxCoverNum == 0){
			seeds.clear();
			break;
		}
	}
	if(querytags.count() != 0){
		seeds.clear();
	}

	//cout<<"querytags.count() = "<<querytags.count()<<endl;
	//cout<<seeds.size()<<endl;
	return querytags.count();
}


void WSC::initial(InfGraph& g, Keywords& words, Query& Q, vector<QueueEle>& queue){
	//!!!
	set<int> qwords(Q.keywords.begin(), Q.keywords.end());
	vector<double> r, APs;
	r.resize(g.size(), 1);
	APs.resize(g.size(), 0);
	vector<int> seed;
	IRIEUpdate(g, r, APs, seed); 
	for(unsigned i = 0; i < g.size(); ++i){
		set<int>::iterator it1, it2;
		it1 = qwords.begin();
		it2 = words[i].begin();
		unsigned counter = 0;
		bitset<TAG_BIT_LENGTH> tagStatus(0);
		while(it1 != qwords.end() && it2 != words[i].end()){
			if(*it1 == *it2){
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
		if(!tagStatus.any())continue;
		WSC::QueueEle e;
		e.tagState = tagStatus;
		e.nodeID = i;
		e.step = 0;
		queue.push_back(e);
	}
}
	