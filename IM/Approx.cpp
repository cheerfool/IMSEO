#include"Approx.h"
bool out;
void initial(InfGraph& g, Keywords& words, Query& Q){
	//!!!
	set<int> qwords(Q.keywords.begin(), Q.keywords.end());
	set<int> sets;
	for(unsigned i = 0; i < g.size(); ++i){
		set<int>::iterator it1, it2;
		it1 = qwords.begin();
		it2 = words[i].begin();
		unsigned counter = 0;
		bitset<BITLENGTH> tagStatus(0);
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
		g[i].bits = tagStatus;
		sets.insert(g[i].bits.to_ulong());
	}	
}

double selectSeedsWithConstraintsSIM(InfGraph& g, Keywords& words, Query& Q, vector<int>& s, double influ, APPROX::Result& result, vector<vector<double>*>& old_influs){
	//!!!
	if(s.size() == Q.K)return -1;
	bitset<BITLENGTH> covered_attributes;
	for(unsigned i=0; i<s.size(); i++){
		covered_attributes |= g[s[i]].bits;
	}
	APPROX::NodeHouse nhouse;
	nhouse.influ1 = influ;
	nhouse.seeds1.assign(s.begin(), s.end());
	constructNodeHouseSim(g, Q, nhouse, s, old_influs);
	APPROX::CoverNumList clist;
	constructCoverList(nhouse, clist);
	checkAllCombinationsSim(Q.K - s.size(), nhouse.queryTagNum, g, nhouse, clist, result);
	result.invoke_time += nhouse.invoke_times;
    
    return 1;
}

double selectSeedsWithConstraintsIRIE(InfGraph& g, Keywords& words, Query& Q, vector<int>& s, double influ, vector<double>& r, vector<double>& APs, APPROX::Result& result){
	//!!!
	APPROX::NodeHouse nhouse;
	nhouse.influ1 = influ;
	nhouse.r.assign(r.begin(), r.end());
	nhouse.APs.assign(APs.begin(), APs.end());
	constructNodeHouse(g, Q, nhouse, s, nhouse.r);
	cout<<"nodehouse constructed"<<endl;
	APPROX::CoverNumList clist;
	constructCoverList(nhouse, clist);
	cout<<"coverlist constructed"<<endl;
	checkAllCombinations(Q.K - s.size(), nhouse.queryTagNum, g, nhouse, clist, result);
	result.invoke_time += nhouse.invoke_times;
	return 1;
	
}

void constructNodeHouseSim(InfGraph& g, Query& Q, APPROX::NodeHouse& nhouse, vector<int>& s, vector<vector<double>*>& old_influ){
	//!!!
	bitset<BITLENGTH> covered_bits;
	int s_size = s.size();
	for(unsigned i=0; i<s.size(); i++){
		covered_bits |= g[s[i]].bits;
	}
	//nhouse.seeds1.assign(s.begin(), s.end());
	nhouse.tagSets.resize(1<<(Q.keywords.size()-covered_bits.count()));
	nhouse.queryTag = (1 << (Q.keywords.size() - covered_bits.count()))-1;
	nhouse.queryTagNum = (Q.keywords.size() - covered_bits.count());
	for (unsigned i = 0; i < g.size(); i++){
		if ((g[i].bits | covered_bits) == covered_bits)continue;
		bitset<BITLENGTH> tagStatus;
		int counter = 0;
		for (unsigned j = 0; j < Q.keywords.size(); j++){
			if (covered_bits[j] == 0){
				if (g[i].bits[j]){
					tagStatus.set(counter, true);
				}
				else{
					tagStatus.set(counter, false);
				}
				counter++;
			}
		}
		//cout<<"tagStatus.to_ulong() "<<tagStatus.to_ulong()<<endl;
		//system("pause");
		nhouse.tagSets[tagStatus.to_ulong()].nodes.push_back(make_pair(old_influ[i]->at(s_size), i));
		nhouse.tagSets[tagStatus.to_ulong()].tagStatus = tagStatus;
	}
	for (unsigned i = 0; i < nhouse.tagSets.size(); ++i){
		//cout<<"i is "<<i<<endl;
		sort(nhouse.tagSets[i].nodes.begin(), nhouse.tagSets[i].nodes.end(), std::greater<pair<double, int>>());
		if (nhouse.tagSets[i].nodes.size() > 0)
			nhouse.tagSets[i].maxInflu = nhouse.tagSets[i].nodes[0].first;
		else
			nhouse.tagSets[i].maxInflu = -1;
	}
	//cout<<"sorted"<<endl;
	//system("pause");
	nhouse.QK = Q.K;
}

void constructNodeHouse(InfGraph& g, Query& Q, APPROX::NodeHouse& nhouse){
	//!!!
	nhouse.invoke_times = 0;
	nhouse.tagSets.resize(1<<(Q.keywords.size()));
	nhouse.queryTag = (1<<(Q.keywords.size()))-1;
	nhouse.queryTagNum = Q.keywords.size();
	vector<double> r, APs;
	vector<int> s;
	IRIEUpdate(g, r,APs, s);
	for(unsigned i=0; i<g.size(); i++){
		if(g[i].bits.any()){
			nhouse.tagSets[g[i].bits.to_ulong()].nodes.push_back(make_pair(r[i], i));
			nhouse.tagSets[g[i].bits.to_ulong()].tagStatus = g[i].bits;
		}
	}
	for (unsigned i = 0; i < nhouse.tagSets.size(); ++i){
		//cout<<"i is "<<i<<endl;
		sort(nhouse.tagSets[i].nodes.begin(), nhouse.tagSets[i].nodes.end(), std::greater<pair<double, int>>());
		if (nhouse.tagSets[i].nodes.size() > 0)
			nhouse.tagSets[i].maxInflu = nhouse.tagSets[i].nodes[0].first;
		else
			nhouse.tagSets[i].maxInflu = -1;
	}
	nhouse.QK = Q.K;
}
void constructNodeHouse(InfGraph& g, Query& Q, APPROX::NodeHouse& nhouse, vector<int>& s, vector<double>& r){
	//!!!
	bitset<BITLENGTH> covered_bits;
	for (unsigned i = 0; i < s.size(); i++){
		covered_bits |= g[s[i]].bits;
	}
	//cout<<s.size()<<" nodes covered "<<covered_bits.to_string()<<" bits"<<endl;
	//system("pause");
	nhouse.invoke_times = 0;
	nhouse.seeds1.assign(s.begin(), s.end());
	nhouse.tagSets.resize(1<<(Q.keywords.size() - covered_bits.count()));
	//cout<<"nhouse.tagSets.size() "<<nhouse.tagSets.size()<<endl;
	nhouse.queryTag = (1 << (Q.keywords.size() - covered_bits.count())) - 1;
	nhouse.queryTagNum = (Q.keywords.size() - covered_bits.count());
	cout<<covered_bits.count()<<" tags covered by"<<s.size()<<" nodes"<<endl;
	//cout<<"start"<<endl;
	//system("pause");
	for (unsigned i = 0; i < g.size(); i++){
		if ((g[i].bits | covered_bits) == covered_bits)continue;
		bitset<BITLENGTH> tagStatus;
		int counter = 0;
		for (unsigned j = 0; j < Q.keywords.size(); j++){
			if (covered_bits[j] == 0){
				if (g[i].bits[j]){
					tagStatus.set(counter, true);
				}
				else{
					tagStatus.set(counter, false);
				}
				counter++;
			}
		}
		//cout<<"tagStatus.to_ulong() "<<tagStatus.to_ulong()<<endl;
		//system("pause");
		nhouse.tagSets[tagStatus.to_ulong()].nodes.push_back(make_pair(r[i], i));
		nhouse.tagSets[tagStatus.to_ulong()].tagStatus = tagStatus;
	}
	//cout<<"added"<<endl;
	//system("pause");
	for (unsigned i = 0; i < nhouse.tagSets.size(); ++i){
		//cout<<"i is "<<i<<endl;
		sort(nhouse.tagSets[i].nodes.begin(), nhouse.tagSets[i].nodes.end(), std::greater<pair<double, int>>());
		if (nhouse.tagSets[i].nodes.size() > 0)
			nhouse.tagSets[i].maxInflu = nhouse.tagSets[i].nodes[0].first;
		else
			nhouse.tagSets[i].maxInflu = -1;
	}
	//cout<<"sorted"<<endl;
	//system("pause");
	nhouse.QK = Q.K;
}


void constructCoverList(APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist){
	//!!!
	clist.resize(nhouse.queryTagNum + 1);
	for (unsigned i = 0; i < nhouse.tagSets.size(); ++i){
		if (nhouse.tagSets[i].nodes.size() > 0){
			int coverNum = nhouse.tagSets[i].tagStatus.count();
			for (unsigned j = 1; j <= coverNum; ++j){
				clist[j].push_back(&(nhouse.tagSets[i]));
			}
		}
	}
	for (unsigned i = 0; i < clist.size(); ++i){
		bool noexchange;;
		for (unsigned j = 0; j < clist[i].size(); ++j){
			noexchange = true;
			for (unsigned k = 0; k < clist[i].size() - 1; ++k){
				if (clist[i][k]->maxInflu < clist[i][k + 1]->maxInflu){
					APPROX::NodeHouseUnit* tmp = clist[i][k];
					clist[i][k] = clist[i][k + 1];
					clist[i][k + 1] = tmp;
					noexchange = false;
				}
			}
			if (noexchange)break;
		}
	}
	int nodeNum = 0;
	for(unsigned i=0; i<nhouse.tagSets.size(); i++){
		nodeNum += nhouse.tagSets[i].nodes.size();
	}
	//cout<<"nodeNum: "<<nodeNum<<endl;
	//system("pause");
}

void selectSeedsSim(InfGraph& g, vector<unsigned>& types, APPROX::NodeHouse& nhouse, APPROX::Cache& myCache, APPROX::Result& result){
	//!!!
	nhouse.invoke_times++;
	vector<int> tmpSeeds(nhouse.seeds1.begin(), nhouse.seeds1.end());
	vector<bool> selected(g.size(), false);
	double influ = nhouse.influ1;
	for(unsigned i=0; i<tmpSeeds.size(); i++){
		selected[tmpSeeds[i]] = true;
	}
	vector<int> ids;
	vector<double> influs(g.size(), 0);
	for(unsigned i =0; i<types.size(); i++){
		for(unsigned j =0; j < nhouse.tagSets[types[i]].nodes.size(); ++j){
			ids.push_back(nhouse.tagSets[types[i]].nodes[j].second);
			influs[nhouse.tagSets[types[i]].nodes[j].second] = nhouse.tagSets[types[i]].nodes[j].first;
		}
	}
	for(unsigned i=0; i<types.size(); i++){
		double maxInflu = -1;
		int maxInfID = -1;
		for(unsigned j=0; j<nhouse.tagSets[types[i]].nodes.size(); j++){
			int nodeID = nhouse.tagSets[types[i]].nodes[j].second;
			if((!selected[nodeID]) && influs[nodeID] > maxInflu){
				maxInflu = influs[nodeID];
				maxInfID = nodeID;
			}
		}
		if(maxInfID == -1){
			return;
		}
		tmpSeeds.push_back(maxInfID);
		influ += maxInflu;
		influs.clear();
		influs.resize(g.size(), 0);
		FastSimInfluence(g, tmpSeeds, ids, influs);
	}
	if(influ > result.influence){
		result.influence = influ;
		result.seeds.assign(tmpSeeds.begin(), tmpSeeds.end());
		cout<<" nhouse.influ1 is "<<nhouse.influ1<<endl;
		cout<<"influence "<<influ<<endl;
	}
}
void selectSeeds(InfGraph& g, vector<unsigned>& types, APPROX::NodeHouse& nhouse, APPROX::Cache& myCache, APPROX::Result& result){
	//!!!
	nhouse.invoke_times++;
	vector<double> r, APs;
	r.assign(nhouse.r.begin(), nhouse.r.end());
	APs.assign(nhouse.APs.begin(), nhouse.APs.end());
	vector<int> tmpSeeds;
	vector<bool> selected(g.size(), false);
	double influ = 0;
	tmpSeeds.assign(nhouse.seeds1.begin(), nhouse.seeds1.end());
	for (unsigned i = 0; i < tmpSeeds.size(); i++){
		selected[tmpSeeds[i]] = true;
	}
	for (unsigned i = 0; i < types.size(); i++){
		double maxInflu = -1;
		int maxInfNode = -1;
		for (unsigned j = 0; j < nhouse.tagSets[types[i]].nodes.size(); j++){
			int nodeID = nhouse.tagSets[types[i]].nodes[j].second;
			if ((!selected[nodeID]) && r[nodeID] > maxInflu){
				maxInflu = r[nodeID];
				maxInfNode = nodeID;
			}
		}
		if (maxInfNode == -1){
			return;
		}
		tmpSeeds.push_back(maxInfNode);
		influ += maxInflu;
		IRIEUpdate(g, r, APs, tmpSeeds);
	}
	if (influ + nhouse.influ1> result.influence){
		result.seeds.assign(tmpSeeds.begin(), tmpSeeds.end());
		result.influence = influ + nhouse.influ1;
		
		cout<<"influence "<<result.influence+nhouse.influ1<<endl;
	}
}

bool IsStartersValid(vector<int>& starters, APPROX::CoverNumList& clist){
	//!!!
	for (unsigned i = 0; i < starters.size(); i++){
		if (clist[starters[i]].size() == 0){
			return false;
		}
	}
	return true;
}
void checkAllCombinationsSim(int size, int queryTagNum, InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, APPROX::Result& result){
	//!!!
	cout<<"size is "<<size<<" queryTagNum is "<<queryTagNum<<endl;
	vector<int> starters(size);
	if (size == 1){
		starters[0] = queryTagNum;
		if (clist[starters[0]].size() > 0){
			combinationCheckSim(g, nhouse, clist, starters, result);
		}
	}
	else{
		for (unsigned i = 0; i < size; i++){
			starters[i] = 1;
		}
		starters[0] = queryTagNum - size + 1;
		bool end = false;
		while (!end){
			end = true;
			if (IsStartersValid(starters, clist)){
				combinationCheckSim(g, nhouse, clist, starters, result);
			}
			for (int i = size - 1; i >= 0; i--){
				if(starters[i] - starters.back() >= 2){
					starters[i] --;
					for(int j=i; j<size; j++){
						if(starters[j]+1 <=starters[i]){
							starters[j]++;
							break;
						}
					}
					end = false;
					break;
				}
				/*if (starters[i - 1] > starters[i] + 1){
					starters[i - 1] --;
					starters[i] ++;
					end = false;
					break;
				}*/
			}
			if(end)break;
			
		}
	}
}
void checkAllCombinations(int size, int queryTagNum, InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, APPROX::Result& result){
	//!!!
	cout<<"size is "<<size<<" queryTagNum is "<<queryTagNum<<endl;
	vector<int> starters(size);
	int balance = 0;
	if (size == 0 && queryTagNum == 0){
		if(nhouse.influ1 > result.influence){
			result.seeds.assign(nhouse.seeds1.begin(), nhouse.seeds1.end());
			result.influence = nhouse.influ1;
			cout<<"influence "<<result.influence<<endl;
			return;
		}
	}
	else if (size == 1){
		starters[0] = queryTagNum;
		if (clist[starters[0]].size() > 0){
			combinationCheckIRIE(g, nhouse, clist, starters, result);
		}
		return;
	}
	else{
		for (unsigned i = 0; i < size; i++){
			starters[i] = 1;
		}
		starters[0] = queryTagNum - size + 1;
		bool end = false;
		while (!end){
			balance = 0;
			end = false;
			for(unsigned m = 0; m<starters.size(); m++){
				cout<<starters[m]<<" ";
			}
			cout<<endl;
			if (IsStartersValid(starters, clist)){
				combinationCheckIRIE(g, nhouse, clist, starters, result);
			}
			int loc ;
			bool found = false;
			for (loc = size - 2; loc >= 0; loc--){
				if(starters[loc] - starters[size-1] >= 2){
					found = true;
					break;
				}
			}
			if(found){
				starters[loc]--;
				balance++;
				for(int i=size-1; i>loc; i--){
					
					balance += starters[i]-1;
					starters[i] = 1;
				}
				for(int j=loc+1; j<size; j++){
					if(balance + 1 >= starters[loc]){
						starters[j] = starters[loc];
						balance -= starters[loc]-1;
					}
					else{
						starters[j] = 1 + balance;
						balance = 0;
						break;
					}
				}
				if(balance > 0){
					end = true;
					break;
				}
			}
			else{
				end = true;
			}
			if(end)break;
			
		}
	}
}

void combinationCheckSim(InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, vector<int>& starters, APPROX::Result& result){
	//!!!
	for(unsigned i=0; i<starters.size(); i++){
		cout<<starters[i]<<" ";
	}
	out = false;
	cout<<endl;
	APPROX::Cache myCache;
	myCache.type_num = starters.size();
	vector<int> locs;
	int times = 1;
	for (unsigned i = 0; i<starters.size(); i++){
		if (i < starters.size() - 1 && starters[i] != starters[i + 1]){
			myCache.lists.push_back(starters[i]);
			myCache.times.push_back(times);
			times = 1;
		}
		if (i < starters.size() - 1 && starters[i] == starters[i + 1]){
			times++;
		}
		if (i == starters.size() - 1){
			myCache.lists.push_back(starters[i]);
			myCache.times.push_back(times);
		}
	}
	locs.resize(myCache.lists.size(), 0);
	/*for(unsigned i=0; i<myCache.lists.size(); i++){
		cout<<clist[myCache.lists[i]].size()<<" ";
	}
	bitset<BITLENGTH> fff(0);
	for(unsigned i=0; i<clist[myCache.lists[1]].size(); i++){
		if(i > clist[myCache.lists[0]].size())cout<<fff.to_string();
		else cout<<clist[myCache.lists[0]][i]->tagStatus.to_string();
		cout<<" ";
		cout<<clist[myCache.lists[1]][i]->tagStatus.to_string()<<endl;
	}
	system("pause");*/
	unsigned combine_size = starters.size();
	if (combine_size == 1){
		vector<unsigned> types;
		types.push_back(clist[starters[0]].front()->tagStatus.to_ulong());
		selectSeedsSim(g, types, nhouse, myCache, result);
		return;
	}
	bool start = false;
	bitset<BITLENGTH> reach_end;
	while (true){
		//cout<<reach_end.to_string()<<endl;
		//cout<<myCache.cacheNodes.size()<<endl;
		//cout<<"level "<<locs[0]<<" "<<clist[myCache.lists.back()].size()<<endl;
		if (start && myCache.cacheNodes.size() == 0)break;
		if (reach_end.count() == myCache.lists.size())break;
		start = true;
		vector<double> uppers(myCache.lists.size(), 0);
		/*for(unsigned i=0; i<locs.size(); i++){
		cout<<locs[i]<<" ";
		}
		cout<<endl;
		system("pause");*/
		double upper_bound = 0;
		for (unsigned i = 0; i<myCache.lists.size(); i++){
			int cover_num = myCache.lists[i];
			if (locs[i] >= clist[cover_num].size()){
				reach_end.set(i, true);
				uppers[i] = 0;
			}
			else{
				uppers[i] = clist[cover_num][locs[i]]->maxInflu;
				upper_bound += uppers[i] * myCache.times[i];
			}
		}
		//cout<<"upper_bound "<<upper_bound<<endl;
		//for(unsigned i=0; i<uppers.size(); i++){
		//	cout<<uppers[i]<<" ";
		//}
		//cout<<endl;
		//system("pause");
		for (unsigned i = 0; i<myCache.lists.size(); i++){
			if (reach_end[i]){
				for (list<APPROX::CacheUnit>::iterator it = myCache.cacheNodes.begin();
					it != myCache.cacheNodes.end();
					++it){
					if (it->extra_times[i] > 0){
						it = myCache.cacheNodes.erase(it);
					}
				}
				continue;
			}
			int cover_num = myCache.lists[i];
			//cout<<"upper_bound = "<<upper_bound<<" nhouse.maxInflu = "<<nhouse.maxInflu<<" global_bound = "<<global_bound<<endl;
			if ((!reach_end.any()) &&
				upper_bound+nhouse.influ1 > result.influence){
				APPROX::CacheUnit cu;
				cu.extra_times.assign(myCache.times.begin(), myCache.times.end());
				cu.extra_times[i]--;
				cu.upper.assign(uppers.begin(), uppers.end());
				//cu.xLow.assign(uppers.begin(), uppers.end());
				//cout<<"cu.xlow.size() "<<cu.xLow.size()<<endl;
				//cu.upperInflu = upper_bound;
				cu.upperBound = upper_bound;
				cu.tagCombinations.resize(myCache.lists.size());
				cu.tagCombinations[i].push_back(clist[cover_num][locs[i]]->tagStatus);
				cu.types.push_back(clist[cover_num][locs[i]]->tagStatus.to_ulong());
				myCache.cacheNodes.push_back(cu);
			}
			//cout<<"start check"<<endl;
			//system("pause");
			checkTagSetSim(g, nhouse, clist, myCache, clist[cover_num][locs[i]], i, myCache.lists[i], result);
			//cout<<"check finished"<<endl;
		}
		for (unsigned i = 0; i<locs.size(); i++){
			locs[i]++;
		}
		if(out)cout<<"line "<<locs[0]<<endl;
	}
}
void combinationCheckIRIE(InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, vector<int>& starters, APPROX::Result& result){
	//!!!
	for(unsigned i=0; i<starters.size(); i++){
		cout<<starters[i]<<" ";
	}
	out = false;
	cout<<endl;
	APPROX::Cache myCache;
	myCache.type_num = starters.size();
	vector<int> locs;
	int times = 1;
	for (unsigned i = 0; i<starters.size(); i++){
		if (i < starters.size() - 1 && starters[i] != starters[i + 1]){
			myCache.lists.push_back(starters[i]);
			myCache.times.push_back(times);
			times = 1;
		}
		if (i < starters.size() - 1 && starters[i] == starters[i + 1]){
			times++;
		}
		if (i == starters.size() - 1){
			myCache.lists.push_back(starters[i]);
			myCache.times.push_back(times);
		}
	}
	locs.resize(myCache.lists.size(), 0);
	/*for(unsigned i=0; i<myCache.lists.size(); i++){
		cout<<clist[myCache.lists[i]].size()<<" ";
	}
	bitset<BITLENGTH> fff(0);
	for(unsigned i=0; i<clist[myCache.lists[1]].size(); i++){
		if(i > clist[myCache.lists[0]].size())cout<<fff.to_string();
		else cout<<clist[myCache.lists[0]][i]->tagStatus.to_string();
		cout<<" ";
		cout<<clist[myCache.lists[1]][i]->tagStatus.to_string()<<endl;
	}
	system("pause");*/
	unsigned combine_size = starters.size();
	if (combine_size == 1){
		vector<unsigned> types;
		types.push_back(clist[starters[0]].front()->tagStatus.to_ulong());
		selectSeeds(g, types, nhouse, myCache, result);
		return;
	}
	bool start = false;
	bitset<BITLENGTH> reach_end;
	while (true){
		//cout<<reach_end.to_string()<<endl;
		//cout<<myCache.cacheNodes.size()<<endl;
		//cout<<"level "<<locs[0]<<" "<<clist[myCache.lists.back()].size()<<endl;
		if (start && myCache.cacheNodes.size() == 0)break;
		if (reach_end.count() == myCache.lists.size())break;
		start = true;
		vector<double> uppers(myCache.lists.size(), 0);
		/*for(unsigned i=0; i<locs.size(); i++){
		cout<<locs[i]<<" ";
		}
		cout<<endl;
		system("pause");*/
		double upper_bound = 0;
		for (unsigned i = 0; i<myCache.lists.size(); i++){
			int cover_num = myCache.lists[i];
			if (locs[i] >= clist[cover_num].size()){
				reach_end.set(i, true);
				uppers[i] = 0;
			}
			else{
				uppers[i] = clist[cover_num][locs[i]]->maxInflu;
				upper_bound += uppers[i] * myCache.times[i];
			}
		}
		//cout<<"upper_bound "<<upper_bound<<endl;
		//for(unsigned i=0; i<uppers.size(); i++){
		//	cout<<uppers[i]<<" ";
		//}
		//cout<<endl;
		//system("pause");
		for (unsigned i = 0; i<myCache.lists.size(); i++){
			if (reach_end[i]){
				for (list<APPROX::CacheUnit>::iterator it = myCache.cacheNodes.begin();
					it != myCache.cacheNodes.end();
					++it){
					if (it->extra_times[i] > 0){
						it = myCache.cacheNodes.erase(it);
					}
				}
				continue;
			}
			int cover_num = myCache.lists[i];
			//cout<<"upper_bound = "<<upper_bound<<" nhouse.maxInflu = "<<nhouse.maxInflu<<" global_bound = "<<global_bound<<endl;
			if ((!reach_end.any()) &&
				upper_bound + nhouse.influ1 > result.influence){
				APPROX::CacheUnit cu;
				cu.extra_times.assign(myCache.times.begin(), myCache.times.end());
				cu.extra_times[i]--;
				cu.upper.assign(uppers.begin(), uppers.end());
				//cu.xLow.assign(uppers.begin(), uppers.end());
				//cout<<"cu.xlow.size() "<<cu.xLow.size()<<endl;
				//cu.upperInflu = upper_bound;
				cu.upperBound = upper_bound;
				cu.tagCombinations.resize(myCache.lists.size());
				cu.tagCombinations[i].push_back(clist[cover_num][locs[i]]->tagStatus);
				cu.types.push_back(clist[cover_num][locs[i]]->tagStatus.to_ulong());
				myCache.cacheNodes.push_back(cu);
			}
			//cout<<"start check"<<endl;
			//system("pause");
			checkTagSet(g, nhouse, clist, myCache, clist[cover_num][locs[i]], i, myCache.lists[i], result);
			//cout<<"check finished"<<endl;
		}
		for (unsigned i = 0; i<locs.size(); i++){
			locs[i]++;
		}
		if(out)cout<<"line "<<locs[0]<<endl;
	}
}
bool IsValidTagBits(APPROX::CacheUnit& cacheUnit, bitset<BITLENGTH>& tagStatus, int columnNum, vector<int>& lists, int queryTagNum){
	//!!!
	bitset<BITLENGTH> tmp_bits;
	int old_tag_bits = 0;
	cacheUnit.tagCombinations[columnNum].push_back(tagStatus);
	cacheUnit.extra_times[columnNum]--;
	/*for(unsigned i=0; i<cacheUnit.extra_times.size(); i++){
		for(unsigned j=0; j<cacheUnit.tagCombinations[i].size(); j++){
			cout<<cacheUnit.tagCombinations[i][j].to_string()<<" ";
		}
	}
	cout<<endl;*/
	for (unsigned i = 0; i<cacheUnit.extra_times.size(); i++){
		for (unsigned j = 0; j<cacheUnit.tagCombinations[i].size(); j++){
			tmp_bits |= cacheUnit.tagCombinations[i][j];
			if (tmp_bits.count() - old_tag_bits != lists[i]){
				cacheUnit.extra_times[columnNum]++;
				cacheUnit.tagCombinations[columnNum].pop_back();
				//if(out)cout<<"reason 1"<<endl;
				return false;
			}
			old_tag_bits = tmp_bits.count();
		}
		if (cacheUnit.extra_times[i] != 0)break;
	}
	bitset<BITLENGTH> tmp_bits2;
	int totalNum = 0;
	int maxNum = queryTagNum;
	for (unsigned i = 0; i < cacheUnit.extra_times.size(); i++){
		for (unsigned j = 0; j < cacheUnit.tagCombinations[i].size(); j++){
			//cout<<cacheUnit.tagCombinations[i][j].to_string()<<" ";
			tmp_bits2 |= cacheUnit.tagCombinations[i][j];
			totalNum += lists[i];
			if(cacheUnit.tagCombinations[i][j].count() > lists[0]){
				cacheUnit.extra_times[columnNum]++;
				cacheUnit.tagCombinations[columnNum].pop_back();
				//if(out)cout<<"reason 2"<<endl;
				return false;
			}
		}

	}
	bitset<BITLENGTH> T, B;
	B = tmp_bits2;
	for(unsigned i=0; i<cacheUnit.extra_times.size(); i++){
		for(unsigned j=0; j<cacheUnit.tagCombinations[i].size(); j++){
			B = B & (~(cacheUnit.tagCombinations[i][j] & T));
			T = T | cacheUnit.tagCombinations[i][j];
		}
	}
	for(unsigned i=0; i<cacheUnit.extra_times.size(); i++){
		for(unsigned j=0; j<cacheUnit.tagCombinations[i].size(); j++){
			bitset<BITLENGTH> temp = B & cacheUnit.tagCombinations[i][j];
			if(!temp.any()){
				cacheUnit.extra_times[columnNum]++;
				cacheUnit.tagCombinations[columnNum].pop_back();
				//if(out)cout<<"reason 3"<<endl;
				return false;
			}
		}
	}
	for(int i=cacheUnit.extra_times.size() -1; i >= 0; i--){
		queryTagNum -= cacheUnit.extra_times[i] * lists[i];
		if(cacheUnit.tagCombinations[i].size() > 0)break;
	}
	//cout<<endl;
	if (tmp_bits2.count() < totalNum || tmp_bits2.count() > maxNum){
		cacheUnit.extra_times[columnNum]++;
		cacheUnit.tagCombinations[columnNum].pop_back();
		//if(out)cout<<"reason 4"<<endl;
		return false;
	}
	cacheUnit.extra_times[columnNum]++;
	cacheUnit.tagCombinations[columnNum].pop_back();
	return true;
}

void checkTagSetSim(InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, APPROX::Cache& myCache, APPROX::NodeHouseUnit* ptr_nhouse, unsigned columnNum, unsigned coverNum, APPROX::Result& result){
	//!!!
	unsigned combine_size = myCache.type_num;
	for (list<APPROX::CacheUnit>::iterator it = myCache.cacheNodes.begin();
		it != myCache.cacheNodes.end();
		++it){
		if (it->extra_times[columnNum] > 0){
			//cout<<"check valid"<<endl;
			if(out){
				cout<<"cover num "<<coverNum<<endl;
				cout<<"checking "<<ptr_nhouse->tagStatus.to_string()<<endl;
				for(unsigned i=0; i< it->tagCombinations.size(); i++){
					for(unsigned j=0; j<it->tagCombinations[i].size(); j++){
						cout<<it->tagCombinations[i][j].to_string()<<endl;
					}
				}
			}
			bool isValid = IsValidTagBits(*it, ptr_nhouse->tagStatus, columnNum, myCache.lists, nhouse.queryTagNum);
			if(out && isValid)cout<<"valid"<<endl;
			if(out && (!isValid))cout<<"not valid"<<endl;
			//if(out)system("pause");
			//cout<<"isValid "<<isValid<<endl;
			//system("pause");
			//cout<<"isValid "<<isValid<<endl;
			//system("pause");
			it->upperBound -= it->upper[columnNum] * it->extra_times[columnNum];
			//it->upperInflu -= it->xLow[columnNum] * it->extra_times[columnNum];
			it->upper[columnNum] = ptr_nhouse->maxInflu;
			//it->xLow[columnNum] = ptr_nhouse->flex_maxInflu;
			//cout<<"after update: it->xlow.size() "<<it->xLow.size()<<endl;
			it->upperBound += it->upper[columnNum] * it->extra_times[columnNum];
			//it->upperInflu += it->xLow[columnNum] * it->extra_times[columnNum];
			//cout<<"upper bound "<<it->upperBound<<endl;
			if(it->upperBound < result.influence - nhouse.influ1){
				it = myCache.cacheNodes.erase(it);
				continue;
			}
			//cout<<"part 1"<<endl;
			if (isValid){
				APPROX::CacheUnit cu;
				cu.extra_times.assign(it->extra_times.begin(), it->extra_times.end());
				//cout<<"columnNum "<<columnNum<<endl;
				cu.extra_times[columnNum]--;
				cu.types.assign(it->types.begin(), it->types.end());
				cu.types.push_back(ptr_nhouse->tagStatus.to_ulong());
				//cout<<"types"<<endl;
				//cout<<"initial cu: it->xLow.size() "<<it->xLow.size()<<endl;
				cu.upper.assign(it->upper.begin(), it->upper.end());
				cu.upperBound= it->upperBound;
				//cout<<"bound"<<endl;
				//system("pause");
				cu.tagCombinations.resize(myCache.lists.size());
				for (unsigned i = 0; i<it->tagCombinations.size(); i++){
					cu.tagCombinations[i].assign(it->tagCombinations[i].begin(), it->tagCombinations[i].end());
				}
				cu.tagCombinations[columnNum].push_back(ptr_nhouse->tagStatus);
				//cout<<"part 2"<<endl;
				if (cu.upperBound - nhouse.influ1> result.influence ){
					if (cu.types.size() == myCache.type_num){
						//for(unsigned i=0; i<cu.tagCombinations.size(); i++){
						//	for(unsigned j=0; j<cu.tagCombinations[i].size(); j++){
						//		cout<<cu.tagCombinations[i][j].to_string()<<endl;
						//	}
						//}
						//cout<<"upper bound "<<cu.upperInflu<<" myCache.maxInflu "<<myCache.maxInflu<<endl;
						selectSeedsSim(g, cu.types, nhouse, myCache, result);
						//cout<<"myCache.maxInflu "<<myCache.maxInflu<<endl;
					}
					else{
						myCache.cacheNodes.push_back(cu);
					}
				}
			}
		}
	}
}
void checkTagSet(InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::CoverNumList& clist, APPROX::Cache& myCache, APPROX::NodeHouseUnit* ptr_nhouse, unsigned columnNum, unsigned coverNum, APPROX::Result& result){
	//!!!
	unsigned combine_size = myCache.type_num;
	for (list<APPROX::CacheUnit>::iterator it = myCache.cacheNodes.begin();
		it != myCache.cacheNodes.end();
		++it){
		if (it->extra_times[columnNum] > 0){
			//cout<<"check valid"<<endl;
			if(out){
				cout<<"cover num "<<coverNum<<endl;
				cout<<"checking "<<ptr_nhouse->tagStatus.to_string()<<endl;
				for(unsigned i=0; i< it->tagCombinations.size(); i++){
					for(unsigned j=0; j<it->tagCombinations[i].size(); j++){
						cout<<it->tagCombinations[i][j].to_string()<<endl;
					}
				}
			}
			bool isValid = IsValidTagBits(*it, ptr_nhouse->tagStatus, columnNum, myCache.lists, nhouse.queryTagNum);
			if(out && isValid)cout<<"valid"<<endl;
			if(out && (!isValid))cout<<"not valid"<<endl;
			//if(out)system("pause");
			//cout<<"isValid "<<isValid<<endl;
			//system("pause");
			//cout<<"isValid "<<isValid<<endl;
			//system("pause");
			it->upperBound -= it->upper[columnNum] * it->extra_times[columnNum];
			//it->upperInflu -= it->xLow[columnNum] * it->extra_times[columnNum];
			it->upper[columnNum] = ptr_nhouse->maxInflu;
			//it->xLow[columnNum] = ptr_nhouse->flex_maxInflu;
			//cout<<"after update: it->xlow.size() "<<it->xLow.size()<<endl;
			it->upperBound += it->upper[columnNum] * it->extra_times[columnNum];
			//it->upperInflu += it->xLow[columnNum] * it->extra_times[columnNum];
			//cout<<"upper bound "<<it->upperBound<<endl;
			if (it->upperBound + nhouse.influ1 < result.influence){
				it = myCache.cacheNodes.erase(it);
				continue;
			}
			//cout<<"part 1"<<endl;
			if (isValid){
				APPROX::CacheUnit cu;
				cu.extra_times.assign(it->extra_times.begin(), it->extra_times.end());
				//cout<<"columnNum "<<columnNum<<endl;
				cu.extra_times[columnNum]--;
				cu.types.assign(it->types.begin(), it->types.end());
				cu.types.push_back(ptr_nhouse->tagStatus.to_ulong());
				//cout<<"types"<<endl;
				//cout<<"initial cu: it->xLow.size() "<<it->xLow.size()<<endl;
				cu.upper.assign(it->upper.begin(), it->upper.end());
				cu.upperBound= it->upperBound;
				//cout<<"bound"<<endl;
				//system("pause");
				cu.tagCombinations.resize(myCache.lists.size());
				for (unsigned i = 0; i<it->tagCombinations.size(); i++){
					cu.tagCombinations[i].assign(it->tagCombinations[i].begin(), it->tagCombinations[i].end());
				}
				cu.tagCombinations[columnNum].push_back(ptr_nhouse->tagStatus);
				//cout<<"part 2"<<endl;
				if (cu.upperBound + nhouse.influ1 > result.influence){
					if (cu.types.size() == myCache.type_num){
						//for(unsigned i=0; i<cu.tagCombinations.size(); i++){
						//	for(unsigned j=0; j<cu.tagCombinations[i].size(); j++){
						//		cout<<cu.tagCombinations[i][j].to_string()<<endl;
						//	}
						//}
						//cout<<"upper bound "<<cu.upperInflu<<" myCache.maxInflu "<<myCache.maxInflu<<endl;
						selectSeeds(g, cu.types, nhouse, myCache, result);
						//cout<<"myCache.maxInflu "<<myCache.maxInflu<<endl;
					}
					else{
						myCache.cacheNodes.push_back(cu);
					}
				}
			}
		}
	}
}

APPROX::Result approx1(InfGraph& g, Query& Q, Keywords& words){
	//!!!
	APPROX::Result result;
	result.invoke_time = 0;
	result.influOfTopMNodes.clear();
	result.influOfTopMNodes.resize(Q.K+1, 0);
	result.influence = 0;
	result.seeds.clear();
	initial(g, words, Q);
	vector<double> r, APs;
	vector<bool> selected(g.size(), false);
	vector<int> s;
	IRIEUpdate(g, r, APs, s);
	for(unsigned i=0; i<Q.K; i++){
		double maxInflu = -1;
		int maxNode = -1;
		for(unsigned j=0; j<g.size(); j++){
			if(	g[j].bits.any() &&
				!selected[j] && 
				r[j] > maxInflu){
				maxInflu = r[j];
				maxNode = j;
			}
		}
		if(maxNode == -1){
			return result;
		}
		s.push_back(maxNode);
		result.freeSet.push_back(maxNode);
		result.freeSetInflu.push_back(maxInflu);
		selected[maxNode] = true;
		IRIEUpdate(g, r, APs, s);
	}
	for(unsigned i=0; i<=Q.K && i <= Q.keywords.size(); i++){
		vector<int> freeSet;
		double influ = 0;
		bitset<BITLENGTH> tmpbits;
		for(unsigned j=0; j<Q.K - i; j++){
			freeSet.push_back(result.freeSet[j]);
			influ += result.freeSetInflu[j];
			tmpbits |= g[result.freeSet[j]].bits;
		}
		if(Q.keywords.size() - tmpbits.count() < i || 
			(i == 0 && Q.keywords.size() - tmpbits.count() > 0)){
			continue;
		} 
		IRIEUpdate(g, r, APs, freeSet);
		selectSeedsWithConstraintsIRIE(g, words, Q, freeSet, influ, r, APs, result);

	}
	return result;
}

APPROX::Result approx1Sim(InfGraph& g, Query& Q, Keywords& words){
	//!!!
	APPROX::Result result;
	result.influOfTopMNodes.clear();
	result.influOfTopMNodes.resize(Q.K+1, 0);
	result.influence = 0;
	result.seeds.clear();
	initial(g, words, Q);
	vector<int> seedWithNoConstraint;
	vector<double> influ(g.size(), 0);
	vector<int> ids;
	vector<vector<double>*> old_influs(g.size(), NULL);
	for(unsigned i=0; i<g.size(); i++){
		if(g[i].bits.any()){
			ids.push_back(i);
			old_influs[i] = new vector<double>();
		}
	}
	vector<int> s;
	for(unsigned i=0; i < Q.K; i++){
		vector<double> influ(g.size(), 0);
		FastSimInfluence(g, s, ids, influ);
		int maxInfluID = -1;
		double maxInflu = -1;
		for(unsigned j=0; j<ids.size(); j++){
			if(influ[ids[j]] > maxInflu){
				maxInflu = influ[ids[j]];
				maxInfluID = ids[j];
			}
			old_influs[ids[j]]->push_back(influ[ids[j]]);
		}
		if(maxInfluID == -1){
			result.seeds.clear();
			return result;
		}
		s.push_back(maxInfluID);
		result.freeSet.push_back(maxInfluID);
		result.freeSetInflu.push_back(maxInflu);
	}
	vector<int> freeSet;
	for(unsigned i=0; i<=Q.K && i <= Q.keywords.size(); i++){
		freeSet.clear();
		bitset<BITLENGTH> tmpbit;
		double influ = 0;
		for(unsigned j=0; j<Q.K - i; j++){
			freeSet.push_back(result.freeSet[j]);
			influ += result.freeSetInflu[j];
			tmpbit |= g[result.freeSet[j]].bits;
		}
		if(Q.keywords.size() - tmpbit.count() < i ||
			(i==0 && tmpbit.count() < Q.keywords.size())){
				continue;
		}
		selectSeedsWithConstraintsSIM(g, words, Q, freeSet, influ, result, old_influs);
	}	
	return result;
}
bool next_partition(vector<int>& partition, int partition_size){
	//!!!
	vector<int> m(partition.size()+1, 0);
	m[0] = 0;
	for(unsigned i=1; i<=m.size(); i++){
		m[i] = m[i-1] > partition[i-1]?m[i-1]:partition[i-1];
	}
	bool has_next = false;
	partition.back() += 1;
	/*for(unsigned i=0; i<partition.size(); i++){
		cout<<partition[i];
	}
	cout<<endl;*/
	for(int i=partition.size()-1; i>0; i--){
		/*for(unsigned i=0; i<partition.size(); i++){
			cout<<partition[i];
		}
		cout<<endl;
		cout<<"parition["<<i<<"] is "<<partition[i]<<endl;
		cout<<"m["<<i<<"] is "<<m[i]<<endl;
		cout<<"partition_size is "<<partition_size<<endl;*/
		if(partition[i] > m[i]+1  || partition[i] +1 > partition_size){
			partition[i] = 0;
			partition[i-1] += 1;
		}
		else{
			
			has_next = true;
			break;
		}
		
	}
	return has_next;
}
void enumerate_partition(InfGraph& g, APPROX::NodeHouse& nhouse, APPROX::Result& result, vector<vector<double>*>& old_influ){
	//!!!
	vector<int> partitions(nhouse.queryTagNum,0);
	int counter = 0;
	vector<double> r, APs;
	int partition_size = nhouse.QK < nhouse.queryTagNum ? nhouse.QK:nhouse.queryTagNum;
	cout<<"partition_size is "<<partition_size<<endl;
	cout<<"QK is "<<nhouse.QK<<endl;
	bool flag = true;
	while(flag){
		counter += 1;
		if(counter%100 == 0)cout<<counter<<endl;
		/*for(unsigned i=0; i<partitions.size(); i++){
			cout<<partitions[i]<<" ";
		}
		cout<<endl;*/
		vector<int> seeds;
		set<int> selected;
		double influ = 0;
		int part_num = 0;
		for(unsigned i=0; i<partitions.size(); i++){
			if(partitions[i] > part_num){
				part_num = partitions[i];
			}
		}
		part_num += 1;
		vector<bitset<BITLENGTH>> parts(part_num);
		for(unsigned i=0; i<partitions.size(); i++){
			parts[partitions[i]].set(i);
		}
		int p = 0;
		while(parts.size() + p < nhouse.QK){
			int node = result.freeSet[p];
			seeds.push_back(node);
			selected.insert(node);
			influ += result.freeSetInflu[p];
			for(unsigned i=0; i<parts.size(); i++){
				parts[i] = parts[i] & (~g[node].bits);
				if(!parts[i].any()){
					parts.erase(parts.begin() + i);
					i--;
				}
			}
			p += 1;
		}
		/*for(unsigned i=0; i<parts.size(); i++){
			cout<<parts[i].to_string()<<endl;
		}*/
     	double upper = 0;
		for(unsigned i=0; i<parts.size(); i++){
			double max = 0;
			for(unsigned j=0; j<nhouse.tagSets[parts[i].to_ulong()].nodes.size(); j++){
				int nID = nhouse.tagSets[parts[i].to_ulong()].nodes[j].second;
				if(old_influ[nID]->at(p) > max){
					max = old_influ[nID]->at(p);
				}
			}
			upper += max;
		}
		if(upper + influ > result.influence){
			for(unsigned i=0; i<parts.size(); i++){
				IRIEUpdate(g, r, APs, seeds);
				double maxInflu = -1;
				int maxInfNode = -1;
				for(unsigned k=0; k<nhouse.tagSets.size(); k++){
					bitset<BITLENGTH> tmp = nhouse.tagSets[k].tagStatus | parts[i];
					if(tmp.count() == nhouse.tagSets[k].tagStatus.count()){
						for(unsigned j=0; j<nhouse.tagSets[k].nodes.size(); j++){
							int nID = nhouse.tagSets[k].nodes[j].second;
							if(r[nID] > maxInflu && selected.find(nID) == selected.end()){
								maxInflu = r[nID];
								maxInfNode = nID;
							}
						}
					}
				}
				
				if(maxInfNode == -1){
					break;
				}
				seeds.push_back(maxInfNode);
				selected.insert(maxInfNode);
				influ += maxInflu;
			}
			if(seeds.size() == nhouse.QK && influ > result.influence){
				cout<<"p is "<<p<<endl;
				cout<<"influ1 is "<<result.freeSetInflu[p]<<endl;
				
				result.influence = influ;
				result.seeds.assign(seeds.begin(), seeds.end());
				cout<<"influence "<<result.influence<<endl;
			}
		}
		flag = next_partition(partitions, partition_size);
		//system("pause");
	}

}

APPROX::Result pics(InfGraph& g, Query& Q, Keywords& words){
	//!!!
	APPROX::Result result;
	result.influOfTopMNodes.clear();
	result.influOfTopMNodes.resize(Q.K+1, 0);
	result.influence = 0;
	result.seeds.clear();
	initial(g, words, Q);
	APPROX::NodeHouse nhouse;
	constructNodeHouse(g, Q, nhouse);
	vector<double> r, APs;
	vector<bool> selected(g.size(), false);
	vector<int> s;
	vector<vector<double>*> old_influ(g.size());
	for(unsigned i=0; i<g.size(); i++){
		if(g[i].bits.any()){
			old_influ[i] = new vector<double>();
		}
	}
	for(unsigned i=0; i<Q.K; i++){
		IRIEUpdate(g, r, APs, s);
		double maxInflu = -1;
		int maxNode = -1;
		for(unsigned j=0; j<g.size(); j++){
			if(g[j].bits.any()){
				old_influ[j]->push_back(r[j]);
			}
			if(	g[j].bits.any() &&
				!selected[j] && 
				r[j] > maxInflu){
				maxInflu = r[j];
				maxNode = j;
				
			}
		}
		if(maxNode == -1){
			return result;
		}
		s.push_back(maxNode);
		result.freeSet.push_back(maxNode);
		result.freeSetInflu.push_back(maxInflu);
		selected[maxNode] = true;
	}
	for(unsigned i=0; i<result.freeSet.size(); i++){
		cout<<"("<<result.freeSet[i]<<" "<<result.freeSetInflu[i]<<")"<<endl;
	}

	cout<<"start"<<endl;
	enumerate_partition(g, nhouse, result, old_influ);
	return result;
}