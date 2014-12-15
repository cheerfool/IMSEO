#include"MCSim.h"
int MCSim(const InfGraph& g, vector<int>& seeds){
	static mt19937 rng(static_cast<unsigned>(time(NULL)));
	vector<bool> activatedNodes(g.size(), false);
	queue<int> q;
	int activatedNodesCount = seeds.size();
	for(int i=0; i<seeds.size(); i++){
		activatedNodes[i] = true;
		q.push(seeds[i]);
	}
	while(!q.empty()){
		int node = q.front();
		q.pop();
		for(Neighbor::const_iterator it = g[node].neighbors.begin();
			it != g[node].neighbors.end();
			it++){
				if(activatedNodes[it->first]){
					continue;
				}
				bernoulli_distribution<double> bern_dist(it->second);
				variate_generator<mt19937 &, bernoulli_distribution<double> > bern_sampler(rng, bern_dist);

				if(bern_sampler()){
					activatedNodes[it->first] = true;
					activatedNodesCount ++;
					q.push(it->first);
				}
		}
	}
	return activatedNodesCount;
}
double MultiMCSim(vector<int>& seed, const InfGraph& g, int repeat_times){
	double influ = 0;
	for(int i=0; i<repeat_times; i++){
		influ += MCSim(g, seed);
	}
	return (double)(influ / repeat_times);
}

void initialGraph(InfGraph& g, vector<vector<bool>>& mark){
	//!!!
	mark.resize(g.size());
	for(unsigned i=0; i<g.size(); i++){
		mark[i].resize(g[i].neighbors.size());
	}
}
void generateGraph(InfGraph& g, vector<vector<bool>>& mark){
	//!!!
	static mt19937 rng(static_cast<unsigned>(time(NULL)));
	for(unsigned i=0; i<g.size(); i++){
		for(unsigned j=0; j<g[i].neighbors.size(); j++){
			bernoulli_distribution<double> bern_dist(g[i].neighbors[j].second);
			variate_generator<mt19937 &, bernoulli_distribution<double> > bern_sampler(rng, bern_dist);
			if(bern_sampler()){
				mark[i][j] = true;
			}
			else{
				mark[i][j] = false;
			}
		}
	}
}
int findNewActivatedNodes(InfGraph& g, const vector<vector<bool>>& mark, int id, const vector<bool>& influenced){
	//!!!
	if (influenced[id])return 0;
	vector<int> stack;
	vector<bool> visited(g.size(), false);
	stack.push_back(id);
	visited[id] = true;
	int nodeNum = 0;
	while (!stack.empty()){
		int nodeID = stack.back();
		stack.pop_back();
		nodeNum++;
		for (unsigned i = 0; i < g[nodeID].neighbors.size(); i++){
			if (!mark[nodeID][i])continue;
			int neighborID = g[nodeID].neighbors[i].first;
			if ((!influenced[neighborID]) && (!visited[neighborID])){
				stack.push_back(neighborID);
				visited[neighborID] = true;
			}
		}
	}
	return nodeNum;
}
int findActivatedNodes(InfGraph& g, vector<vector<bool>>& mark, vector<int>& seeds, vector<bool>& influenced){
	//!!!
	vector<int> stack(seeds.begin(), seeds.end());
	int nodeNum = 0;
	for(unsigned i=0; i<seeds.size(); i++){
		influenced[seeds[i]] = true;
	}
	while(!stack.empty()){
		int nodeID = stack.back();
		stack.pop_back();
		nodeNum++;
		for(unsigned i=0; i<g[nodeID].neighbors.size(); i++){
			if(!mark[nodeID][i])continue;
			int neighborID = g[nodeID].neighbors[i].first;
			if(!influenced[neighborID]){
				stack.push_back(neighborID);
				influenced[neighborID] = true;
			}
		}

	}
	return nodeNum;
}
double FastSimInfluence(InfGraph& g, vector<int>& seeds){
	vector<vector<bool>> mark;
	initialGraph(g, mark);
	double totalNum = 0;
	for(unsigned i=0; i<REPEAT_TIMES; i++){
		if(i%50 == 0)cout<<"\r"<<100.0*i/REPEAT_TIMES<<"%\t";
		generateGraph(g, mark);
		vector<bool> influenced(g.size(), false);
		totalNum += findActivatedNodes(g, mark, seeds, influenced);
	}
	return 1.0*totalNum/REPEAT_TIMES;
}
void FastSimInfluence(InfGraph& g, vector<int>& S0, vector<int>& ids, vector<double>& influIncrease){
	//!!!
	if(influIncrease.size() != g.size()){
		cout<<"error: influIncrease not initialed"<<endl;
		system("pause");
	}
	vector<vector<bool>> mark;
	vector<bool> influenced(g.size(), false);
	initialGraph(g, mark);
	for (unsigned i = 0; i < REPEAT_TIMES; i++){
		if(i%50 == 0)printf("\r%.2lf%%",1.0*i/REPEAT_TIMES*100);
		generateGraph(g, mark);
		vector<bool> influenced(g.size(), false);
		findActivatedNodes(g, mark, S0, influenced);
		for (unsigned j = 0; j < ids.size(); j++){
			//if(j%50 == 0)cout<<1.0*j/ids.size()*100<<"%"<<endl;
			influIncrease[ids[j]] += findNewActivatedNodes(g, mark, ids[j], influenced);
		}
	}
	for (unsigned i = 0; i < ids.size(); i++){
		influIncrease[ids[i]] /= REPEAT_TIMES;
	}
}
