#include"utility.h"

int update_repeat_times;
bool flag_for_debug = false;

//IRIE parameters
double alpha_theta;
double IRIE_alpha;
double IRIE_theta;
double IRIE_min_stopping_criteria;

void ConstructGraph(string filename, InfGraph& g){
	//read graph without edge weights
	ifstream infile(filename);
	int NodeNum, EdgeNum;
	infile>>NodeNum>>EdgeNum;
	g.resize(NodeNum);
	int src, tar;
	for(unsigned i = 0; i<NodeNum; ++i){
		g[i].influ = 1;
		g[i].oldInflu = 0;
		g[i].indegree = 0;
		g[i].para = 0;
		g[i].alpha = 1.0;
		g[i].valid = true;
	}
	for(unsigned i = 0; i < EdgeNum; ++i){
		infile>>src>>tar;
		g[src].neighbors.push_back(make_pair(tar, 0.0));
		g[tar].indegree ++;
	}
	infile.close();
    cout<<"Graph constructed.\n";
}
void ConstructProbGraph(string filename, InfGraph& g){
	//read graph with edge weights
	ifstream infile(filename);
	int NodeNum, EdgeNum;
	infile>>NodeNum>>EdgeNum;
	g.resize(NodeNum);
	int src, tar;
	double prob;
	for(unsigned i=0; i<NodeNum; ++i){
		g[i].influ = 1;
		g[i].oldInflu = 0;
		g[i].indegree = 0;
		g[i].para = 0;
		g[i].alpha = 1.0;
		g[i].valid = true;
	}
	for(unsigned i=0; i<EdgeNum; ++i){
		infile>>src>>tar>>prob;
		if(prob == 0)continue;
		g[src].neighbors.push_back(make_pair(tar, prob));
		g[tar].indegree ++;
	}
	infile.close();
    cout<<"Graph constructed.\n";
}


void AssignEdgeWeightRandom(InfGraph& g){
	//assign edge weights randomly from (0.1, 0.01, 0.001)
	double prob[3];
	prob[0] = 0.1;
	prob[1] = 0.01;
	prob[2] = 0.001;
	for(unsigned i=0; i<g.size(); ++i){
		for(unsigned j=0; j<g[i].neighbors.size(); j++){
			int r = rand()%3;
			g[i].neighbors[j].second = prob[r];
		}
	}
    cout<<"Weight assigned.\n";
}
void AdjustEdgeWeight(InfGraph& g){
	for(unsigned i=0; i<g.size(); ++i){
		for(Neighbor::iterator it = g[i].neighbors.begin();
			it != g[i].neighbors.end();
			++it){
				int tar = it->first;
				double a = sqrt((double)g[tar].indegree) + 0.8;
				if(a > 1)a=1;
				it->second = it->second * a;
		}
	}
	
}
void AssignEdgeWeight(InfGraph& g){
	//assign edge weights according to the in-degree
	for(unsigned i = 0; i < g.size(); ++i){
		for(Neighbor::iterator it = g[i].neighbors.begin();
			it != g[i].neighbors.end();
			it++){
				int tar = it->first;
				it->second = 1.0 / g[tar].indegree;
		}
	}
    cout<<"Weight assigned.\n";
}
void EstimateAPs(int node, InfGraph& g, vector<double>& APs){
	//IRIE substep
	vector<bool> flag(g.size(), false);
	priority_queue<pair<double, int>> queue;
	queue.push(make_pair(1, node));
	while(!queue.empty()){
		pair<double, int> element = queue.top();
		queue.pop();
		if(flag[element.second] || element.first < IRIE_theta){
			continue;
		}
		flag[element.second] = true;
		APs[element.second] += element.first;
		if(APs[element.second] > 1)APs[element.second] = 1;
		for(unsigned i = 0; i < g[element.second].neighbors.size(); ++i){
			pair<int, double> neighbor = g[element.second].neighbors[i];
			if(flag[neighbor.first]){
				continue;
			}
			queue.push(make_pair(element.first * neighbor.second, neighbor.first));
		}
	}
}

void IRIEUpdate(InfGraph& g, vector<double>& r, vector<double>& APs, vector<int>& S){
	//IRIE
	APs.clear();
	APs.resize(g.size(), 0);
	r.clear();
	r.resize(g.size(), 1);
	for(unsigned i = 0; i < S.size(); ++i){
		int s = S[i];
		EstimateAPs(s, g, APs);
	}
	for(unsigned i = 0; i < S.size(); ++i){
		APs[S[i]] = 1;
	}
	vector<double> newR(g.size());
	for(int iteration_counter = 0; iteration_counter < 5; iteration_counter++){
		double maxIncrease = 0;
		for(unsigned i = 0; i < g.size(); ++i){
			double sum=0;
			for(unsigned j = 0; j < g[i].neighbors.size(); ++j){
				sum += g[i].neighbors[j].second * r[g[i].neighbors[j].first];
			}
			newR[i] = (1 - APs[i])*(1 + IRIE_alpha * sum);
		}
		for(unsigned i = 0; i < g.size(); ++i){
			if(newR[i] - r[i] > maxIncrease)maxIncrease = newR[i] - r[i];
			r[i] = newR[i];
			
		}
		if(maxIncrease < IRIE_min_stopping_criteria){
			break;
		}
	}

}



void trainProbs3(string filename, InfGraph& g){
	vector<vector<pair<int, int>>> actionLog(g.size());
	vector<double> credits(g.size(), 0);
	fstream ifs(filename, ios::in);
	int edgeNum=0;
	for(int i=0; i<g.size(); i++){
		edgeNum += g[i].neighbors.size();
	}
	map<pair<int, int>, vector<int>> SP;
	map<pair<int, int>, double> Ps;
	int user, movie, date;
	while(!ifs.eof()){
		pair<int, int> act;
		ifs>>user>>act.first>>act.second;
		actionLog[user].push_back(act);
	}
	for(int i=0; i<actionLog.size(); i++){
		sort(actionLog[i].begin(), actionLog[i].end());
		for(int j=1; j<actionLog[i].size(); j++){
			if(actionLog[i][j].first < actionLog[i][j-1].first){
				cout<<"wrong order"<<endl;
				exit(0);
			}
		}
	}
	
	//phase 1: initial
	for(int i=0; i<g.size(); i++){
		for(int j=0; j<g[i].neighbors.size(); j++){
			double prob = 0.1 + (rand()%1000) * 0.2 / 1000;
			g[i].neighbors[j].second = prob;
		}
	}
	vector<vector<pair<int, int>>> S(g.size());
	for(int i=0; i<g.size(); i++){
		for(int j=0; j<g[i].neighbors.size(); j++){
			int nei = g[i].neighbors[j].first;
			S[i].push_back(make_pair(0, 0));
			int i1 = 0, i2 = 0;
			while(i1 < actionLog[i].size() && i2 < actionLog[nei].size()){
				if(actionLog[i][i1].first == actionLog[nei][i2].first){
					if(actionLog[i][i1].second < actionLog[nei][i2].second){
						SP[make_pair(i, nei)].push_back(actionLog[i][i1].first);
						S[i][j].first += 1;
					}
					else{
						S[i][j].second += 1;
					}
					i1 ++;
					i2 ++;
					continue;
				}
				if(actionLog[i][i1].first > actionLog[nei][i2].first){
					i2 ++;
					continue;
				}
				if(actionLog[i][i1].first < actionLog[nei][i2].first){
					i1 ++;
					continue;
				}

			}
		}
	}
	int valid_edge = 0;
	for(int i=0; i<g.size(); i++){
		for(int j=0; j<g[i].neighbors.size(); j++){
			if(S[i][j].first + S[i][j].second != 0){
				valid_edge ++;
			}
		}
	}
	cout<<"valid edge num is "<<valid_edge<<" from "<<edgeNum<<" edges"<<endl;
	int iter = 0;
	while(iter < 10){
		cout<<"iteration: "<<iter<<endl;
		iter += 1;
		//compute p
		double average = 0;
		for(int i=0; i<g.size(); i++){
			for(int j=0; j<g[i].neighbors.size(); j++){
				average += g[i].neighbors[j].second;
			}
		}
		cout<<"average weight is "<<average/edgeNum<<endl;
		for(int node = 0; node < g.size(); node ++){
			if(((node+1)*100/g.size())%5 == 0)printf("\r%d%%", (node+1)*100/g.size());
			for(int actionID = 0; actionID < actionLog[node].size(); actionID ++){
				double ps = 1;
				bool empty = true;
				int movieID = actionLog[node][actionID].first;
				int date = actionLog[node][actionID].second;
				for(int nei = 0; nei < g[node].neighbors.size(); nei++){
					int neighbor = g[node].neighbors[nei].first;
					for(int nei_action = 0; nei_action < actionLog[neighbor].size(); nei_action++){
						if(actionLog[neighbor][nei_action].first == movieID
							&& actionLog[neighbor][nei_action].second < date){
								double reverse_prob = -1;
								for(int i = 0; i<g[neighbor].neighbors.size(); i++){
									if(g[neighbor].neighbors[i].first == node){
										reverse_prob = g[neighbor].neighbors[i].second;
										break;
									}
								}
								if(reverse_prob < 0){
									cout<<"reverse_prob "<<reverse_prob<<endl;
									exit(0);
								}
								
								ps = ps*(1-reverse_prob);
								if(node == 4161){
									cout<<"reverse_prob "<<reverse_prob<<endl;
									cout<<"ps "<<ps<<endl;
								}
								empty = false;
								break;
						}
					}
				}
				if(!empty){
					Ps[make_pair(node, movieID)]=1.0/(1-ps);
					if(node == 4161){
						cout<<"Ps["<<node<<","<<movieID<<"] "<<1.0/(1-ps)<<endl;
					}
				}
			}
		}
		for(int node = 0; node < g.size(); node++){
			for(int j=0; j<g[node].neighbors.size(); j++){
				int nei = g[node].neighbors[j].first;
				if(S[node][j].first + S[node][j].second == 0){
				
				}
				else if(S[node][j].first > 0){
					pair<int, int> edge = make_pair(node, nei);
					if(SP.find(edge) == SP.end()){
						cout<<edge.first<<" "<<edge.second<<" not in SP"<<endl;
						exit(0);
					}
					double PT = 0;
					for(int k=0; k<SP[edge].size(); k++){
						int movieID = SP[edge][k];
						if(Ps.find(make_pair(nei, movieID)) == Ps.end()){
							cout<<"user "<<nei<<" rating "<<movieID<<" is not in the record"<<endl;
							exit(0);
						}
						PT += Ps[make_pair(nei, movieID)];

					}
					double new_weight = 1.0/(S[node][j].first+S[node][j].second)*g[node].neighbors[j].second*PT;
					if(new_weight > 1){
						new_weight = 1;
						/*cout<<new_weight<<endl;
						cout<<nei<<" movies: ";
						for(int k=0; k<SP[edge].size(); k++){
							cout<<"{"<<SP[edge][k]<<": "<<Ps[make_pair(nei, SP[edge][k])]<<"} ";
						}
						cout<<endl;
						cout<<PT<<" "<<S[node][j].first+S[node][j].second<<" "<<g[node].neighbors[j].second<<endl;
						exit(0);*/
					}
					g[node].neighbors[j].second = new_weight;
				}
				
			}
		}
	}
	ofstream ofs_graph("trained_graphs", ios::out);
	for(int i=0; i<g.size(); i++){
		for(int j=0; j<g[i].neighbors.size(); j++){
			int nei = g[i].neighbors[j].first;
			ofs_graph<<i<<" "<<nei<<" "<<g[i].neighbors[j].second<<" ";
			if(S[i][j].first > 0)
				ofs_graph<<1<<endl;
			else
				ofs_graph<<0<<endl;
		}
	}
	ofs_graph.close();

}
void trainProbs2(string filename, InfGraph& g){
	//vector<double> ps(g.size(), 1);
	vector<double> p(g.size(), 0);
	vector<vector<pair<int, int>>> actionLog(g.size());
	vector<double> credits(g.size(), 0);
	fstream ifs(filename, ios::in);
	int edgeNum=0;
	for(int i=0; i<g.size(); i++){
		edgeNum += g[i].neighbors.size();
	}
	int user, movie, date;
	while(!ifs.eof()){
		pair<int, int> act;
		ifs>>user>>act.first>>act.second;
		actionLog[user].push_back(act);
	}
	for(int i=0; i<actionLog.size(); i++){
		sort(actionLog[i].begin(), actionLog[i].end());
		for(int j=1; j<actionLog[i].size(); j++){
			if(actionLog[i][j].first < actionLog[i][j-1].first){
				cout<<"wrong order"<<endl;
				exit(0);
			}
		}
	}
	
	//phase 1: initial
	for(int i=0; i<g.size(); i++){
		for(int j=0; j<g[i].neighbors.size(); j++){
			double prob = 0.1 + (rand()%1000) * 0.2 / 1000;
			g[i].neighbors[j].second = prob;
		}
	}
	vector<vector<pair<int, int>>> S(g.size());
	for(int i=0; i<g.size(); i++){
		for(int j=0; j<g[i].neighbors.size(); j++){
			int nei = g[i].neighbors[j].first;
			S[i].push_back(make_pair(0, 0));
			int i1 = 0, i2 = 0;
			while(i1 < actionLog[i].size() && i2 < actionLog[nei].size()){
				if(actionLog[i][i1].first == actionLog[nei][i2].first){
					if(actionLog[i][i1].second > actionLog[nei][i2].second){
						S[i][j].first += 1;
					}
					if(actionLog[i][i1].second < actionLog[nei][i2].second){
						S[i][j].second += 1;
					}
					i1 ++;
					i2 ++;
					continue;
				}
				if(actionLog[i][i1].first > actionLog[nei][i2].first){
					i2 ++;
					continue;
				}
				if(actionLog[i][i1].first < actionLog[nei][i2].first){
					i1 ++;
					continue;
				}

			}
		}
	}
	int valid_edge = 0;
	for(int i=0; i<g.size(); i++){
		for(int j=0; j<g[i].neighbors.size(); j++){
			if(S[i][j].first + S[i][j].second != 0){
				valid_edge ++;
			}
		}
	}
	cout<<"valid edge num is "<<valid_edge<<" from "<<edgeNum<<" edges"<<endl;
	int iter = 0;
	while(iter < 20){
		cout<<"iteration: "<<iter<<endl;
		iter += 1;
		//compute p
		double average = 0;
		for(int i=0; i<g.size(); i++){
			for(int j=0; j<g[i].neighbors.size(); j++){
				average += g[i].neighbors[j].second;
			}
		}
		cout<<"average weight is "<<average/edgeNum<<endl;
		for(int node=0; node<g.size(); node++){
			if(node%1000 == 0)printf("\r%4.2lf%%", 100.0*(node+1)/g.size());
			for(int actionID = 0; actionID < actionLog[node].size(); actionID ++){
				double ps = 1;
				bool empty = true;
				int movieID = actionLog[node][actionID].first;
				int date = actionLog[node][actionID].second;
				for(int nei = 0; nei < g[node].neighbors.size(); nei++){
					int neighbor = g[node].neighbors[nei].first;
					for(int nei_action = 0; nei_action < actionLog[neighbor].size(); nei_action++){
						if(actionLog[neighbor][nei_action].first == movieID
							&& actionLog[neighbor][nei_action].second < date){
								double reverse_prob = -1;
								for(int i = 0; i<g[neighbor].neighbors.size(); i++){
									if(g[neighbor].neighbors[i].first == node){
										reverse_prob = g[neighbor].neighbors[i].second;
										break;
									}
								}
								if(reverse_prob < 0){
									cout<<"reverse_prob "<<reverse_prob<<endl;
									exit(0);
								}
								ps = ps*(1-reverse_prob);
								empty = false;
								break;
						}
					}
				}
				if(empty)
					p[node] = 0;
				else
					p[node] += 1.0/(1-ps);
			}
			
		}
		cout<<endl;
		/*ofstream pout("p", ios::out);
		for(int i=0; i<g.size(); i++){
			pout<<p[i]<<endl;
		}
		pout.close();*/
		//update edge probs
		for(int node = 0; node < g.size(); node++){
			for(int j=0; j<g[node].neighbors.size(); j++){
				int nei = g[node].neighbors[j].first;
				if(S[node][j].first + S[node][j].second == 0){
				
				}
				else{
					double new_weight = 1.0/(S[node][j].first+S[node][j].second)*g[node].neighbors[j].second*p[nei];
					if(new_weight > 1){
						cout<<p[nei]<<" "<<S[node][j].first+S[node][j].second<<" "<<g[node].neighbors[j].second<<endl;
						exit(0);
					}
					g[node].neighbors[j].second = new_weight;
				}
				
			}
		}

	}
	double f_average = 0;
	int edgeCount = 0;
	for(int i=0; i<g.size(); i++){
		for(int j=0; j<g[i].neighbors.size(); j++){
			f_average += g[i].neighbors[j].second;
		}
	}
	cout<<"average edge weight is "<<f_average/edgeCount<<endl;
	ofstream ofs("trained", ios::out);
	vector<int> map(g.size(), -1);
	vector<int> valid_nodes;
	int node_counter = 0;
	//ofs<<g.size()<<" "<<edgeNum<<endl;
	for(int i=0; i<g.size(); i++){
		for(int j=0; j<g[i].neighbors.size(); j++){
			if(S[i][j].first + S[i][j].second > 0){
				if(map[i] < 0){
					map[i] = node_counter;
					valid_nodes.push_back(i);
					node_counter += 1;
				}
				if(map[g[i].neighbors[j].first] < 0){
					map[g[i].neighbors[j].first] = node_counter;
					valid_nodes.push_back(g[i].neighbors[j].first);
					node_counter += 1;
				} 
				ofs<<map[i]<<" "<<map[g[i].neighbors[j].first]<<" "<<g[i].neighbors[j].second<<endl;
			}
		}
	}
	ofs.close();
	ofstream ofs_attr("valid_ratings", ios::out);
	for(int i=0; i<valid_nodes.size(); i++){
		for(int j=0; j<actionLog[valid_nodes[i]].size(); j++){
			ofs_attr<<map[valid_nodes[i]]<<" "<<actionLog[valid_nodes[i]][j].first<<" "<<actionLog[valid_nodes[i]][j].second<<endl;
		}
	}
	ofstream ofs_graph("valid_graphs", ios::out);
	for(int i=0; i<valid_nodes.size(); i++){
		for(int j=0; j<g[valid_nodes[i]].neighbors.size(); j++){
			int nei = g[valid_nodes[i]].neighbors[j].first;
		}
	}
	ofs_attr.close();
}

void trainProbs(string filename, InfGraph& g){
	vector<vector<Action>> actionLog(g.size());
	vector<double> credits(g.size(), 0);
	fstream ifs(filename, ios::in);
	int edgeNum=0;
	for(int i=0; i<g.size(); i++){
		edgeNum += g[i].neighbors.size();
	}
	int user, movie, date;
	while(!ifs.eof()){
		Action act;
		ifs>>user>>act.movie_id>>act.date;
		actionLog[user].push_back(act);
	}
	ofstream attr_ofs("attr", ios::out);
	attr_ofs<<g.size()<<endl;
	for(int i=0; i<actionLog.size(); i++){
		attr_ofs<<actionLog[i].size();
		for(int j=0; j<actionLog[i].size(); j++){
			attr_ofs<<" "<<actionLog[i][j].movie_id;
		}
		attr_ofs<<endl;
	}
	attr_ofs.close();
	cout<<"action log loaded"<<endl;
	ifs.close();
	int edgesWithProbs = 0;
	for(int i=0; i<g.size(); i++){
		cout<<"\r"<<100.0*i/g.size()<<"%\t\t\t";
		double total_credit = 0;
		set<int> s;
		for(int j=0; j<actionLog[i].size(); j++){
			int movieID = actionLog[i][j].movie_id;
			int date = actionLog[i][j].date;
			int credit = 0;
			for(int k=0; k<g[i].neighbors.size(); k++){
				int nei = g[i].neighbors[k].first;
				for(int l=0; l<actionLog[nei].size(); l++){
					if(actionLog[nei][l].movie_id == movieID && actionLog[nei][l].date < date){
						credit += 1;
						s.insert(nei);
						break;
					}
				}
			}
			if(credit != 0){
				total_credit += 1.0/credit;
			}
		}
		if(actionLog[i].size() != 0){
			total_credit /= actionLog[i].size();
		}
		edgesWithProbs += s.size();
		credits[i] = total_credit;
		bool found = false;
		for(set<int>::iterator it = s.begin(); it != s.end(); ++it){
			for(int j=0; j<g[*it].neighbors.size(); j++){
				if(g[*it].neighbors[j].first == i){
					g[*it].neighbors[j].second = total_credit;
					found = true;
					break;
				}
			}
			if(!found){
				cout<<"not found "<<i<<" "<<*it<<endl;
				exit(0);
			}
		}
	}
	cout<<edgesWithProbs<<" edges should have probs"<<endl;
	ofstream ofs("trained", ios::out);
	ofs<<g.size()<<" "<<edgeNum<<endl;
	for(int i=0; i<g.size(); i++){
		for(int j=0; j<g[i].neighbors.size(); j++){
			int nei = g[i].neighbors[j].first;
			ofs<<i<<" "<<nei<<" "<<g[i].neighbors[j].second<<endl;
		}
	}
	ofs.close();

}