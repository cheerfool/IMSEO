#include"expMethod.h"
ofstream query_log;


void ScoreGreedy(string graphfile, string keywordfile, string queryfile, string graphtype, string inftype){
	//string logfile = "SG_"+graphtype+"_"+inftype+"_"+graphfile+"_"+queryfile+".log";
	string logfile=queryfile+"_"+"SG_"+graphtype+"_"+inftype+".log";
    query_log.open(logfile);
	ofstream ofs(logfile + ".seed.log");
	InfGraph g;
	IRIE_min_stopping_criteria = 0.0001;
	IRIE_alpha = 0.4;
	IRIE_theta = 1.0 / 320;
	double theta = 0.001;
	double c_alpha = 0.6;
	if(graphtype == "Trained"){
		ConstructProbGraph(graphfile, g);
	}
	else{
		ConstructGraph(graphfile, g);
		if(graphtype == "Random"){
			AssignEdgeWeightRandom(g);
		}
		if(graphtype == "Degree"){
			AssignEdgeWeight(g);
		}
	}
	cout << "graph loaded" << endl;
	Keywords words;
	ReadKeywords(words, keywordfile);
	cout << "words loaded" << endl;
	cout << words.size() << " users" << endl;
	ifstream ifs_query(queryfile);
	int query_counter = 1;
	double total_time = 0;
	while (!ifs_query.eof()){
		int k, wordNum, w;
		set<int> QueryWords;
		ifs_query >> k >> wordNum;
		for (unsigned i = 0; i < wordNum; ++i){
			ifs_query >> w;
			QueryWords.insert(w);
		}
		Query Q;
		InitialQuery(Q, QueryWords, k);
		cout << "query initialized" << endl;
		if(graphtype == "TopicPP"){
			AssignTopicAwareEdgeWeight(g, words, Q);
		}
		vector<int> seeds;
		clock_t start = clock();
		unsigned left = 0;
		if(inftype == "IRIE"){
			left = WSC::WSCSelect(g, words, Q, seeds, c_alpha);
		}
		if(inftype == "MC"){
			left = ScoreGreedySim(g, words, Q, seeds, c_alpha);
		}
		clock_t end = clock();
		cout<<"finished"<<endl;
		double influ = 0;
		//influ = MultiMCSim(seeds, g);
		query_log<<"query "<<c_alpha<<" "<<influ<<" "<<query_counter<<" : ";
		//query_log<<"influence "<<influ<<endl;
		cout<<left<<" attributes in Q are not covered"<<endl;
		if(seeds.size() != 0){
			query_log<<"success"<<endl;
			cout<<"success"<<endl;
		}
		else{
			query_log<<"failed\t"<<left<<endl;
			cout<<"failed"<<endl;
		}
		query_counter++;
		ofs<<seeds.size()<<" ";
		for(unsigned i =0; i<seeds.size(); i++){
			ofs<<seeds[i]<<" ";
		}
		ofs<<endl;
		//query_log<<"influence is : "<<influence<<endl;
		query_log<<"time cost is : "<<(end-start)<<" ms"<<endl;
		total_time += (end-start);
		seeds.clear();		
		keyword_pattern_tried = 0;
		found = false;
		maxInflu = 0;
	}
	query_log<<"total time cost "<<total_time<<" ms"<<endl;
	query_log.close();
}

void PigeonGreedy(string graphfile, string keywordfile, string queryfile, string graphtype, string inftype){
	string logfile = queryfile+"_"+"PG_"+graphtype+"_"+inftype+".log";
	query_log.open(logfile);
	ofstream ofs(logfile + ".seed.log");
	InfGraph g;
	IRIE_min_stopping_criteria = 0.0001;
	IRIE_alpha = 0.4;
	IRIE_theta = 1.0 / 320;
	double theta = 0.001;
	double c_alpha = 0.6;
	if(graphtype == "Trained"){
		ConstructProbGraph(graphfile, g);
	}
	else{
		ConstructGraph(graphfile, g);
		if(graphtype == "Random"){
			AssignEdgeWeightRandom(g);
		}
		if(graphtype == "Degree"){
			AssignEdgeWeight(g);
		}
	}
	cout << "graph loaded" << endl;
	Keywords words;
	ReadKeywords(words, keywordfile);
	cout << "words loaded" << endl;
	cout << words.size() << " users" << endl;
	ifstream ifs_query(queryfile);
	int query_counter = 1;
	double total_time = 0;
	while (!ifs_query.eof()){
		int k, wordNum, w;
		set<int> QueryWords;
		ifs_query >> k >> wordNum;
		for (unsigned i = 0; i < wordNum; ++i){
			ifs_query >> w;
			QueryWords.insert(w);
		}
		Query Q;
		InitialQuery(Q, QueryWords, k);
		cout << "query initialed" << endl;
		if(graphtype == "TopicPP"){
			AssignTopicAwareEdgeWeight(g, words, Q);
		}
		vector<int> seeds;
		unsigned left = 0;
		clock_t start = clock();
		if(inftype == "IRIE"){
			left = WSC::PigeonGreedy(g, words, Q, seeds);
		}
		if(inftype == "MC"){
			left = PigeonGreedySim(g, words, Q, seeds);
		}
		clock_t end = clock();
		cout<<"finished"<<endl;
		double influ = 0;
		//influ = MultiMCSim(seeds, g);
		query_log<<"query "<<c_alpha<<" "<<influ<<" "<<query_counter<<" : ";
		//query_log<<"influence "<<influ<<endl;
		cout<<left<<" attributes in Q are not covered"<<endl;
		if(seeds.size() != 0){
			query_log<<"success"<<endl;
			cout<<"success"<<endl;
		}
		else{
			query_log<<"failed\t"<<left<<endl;
			cout<<"failed"<<endl;
		}
		query_counter++;
		ofs<<seeds.size()<<" ";
		for(unsigned i =0; i<seeds.size(); i++){
			ofs<<seeds[i]<<" ";
		}
		ofs<<endl;
		//query_log<<"influence is : "<<influence<<endl;
		query_log<<"time cost is : "<<(end-start)<<" ms"<<endl;
		total_time += (end-start);
		seeds.clear();		
		keyword_pattern_tried = 0;
		found = false;
		maxInflu = 0;
	}
	query_log<<"total time cost "<<total_time<<" ms"<<endl;
	query_log.close();
}

void PICSP(string graphfile, string keywordfile, string queryfile, string graphtype, string inftype){
	string logfile = queryfile+"_"+"PICSP_"+graphtype+"_"+inftype+".log";
	query_log.open(logfile);
	ofstream ofs(logfile + ".seed.log");
	InfGraph g;
	IRIE_min_stopping_criteria = 0.0001;
	IRIE_alpha = 0.4;
	IRIE_theta = 1.0 / 320;
	double theta = 0.001;
	double c_alpha = 0.6;
	if(graphtype == "Trained"){
		ConstructProbGraph(graphfile, g);
	}
	else{
		ConstructGraph(graphfile, g);
		if(graphtype == "Random"){
			AssignEdgeWeightRandom(g);
		}
		if(graphtype == "Degree"){
			AssignEdgeWeight(g);
		}
	}
	cout << "graph loaded" << endl;
	Keywords words;
	ReadKeywords(words, keywordfile);
	cout << "words loaded" << endl;
	cout << words.size() << " users" << endl;
	ifstream ifs_query(queryfile);
	int query_counter = 1;
	double total_time = 0;
	while (!ifs_query.eof()){
		int k, wordNum, w;
		set<int> QueryWords;
		ifs_query >> k >> wordNum;
		for (unsigned i = 0; i < wordNum; ++i){
			ifs_query >> w;
			QueryWords.insert(w);
		}
		Query Q;
		InitialQuery(Q, QueryWords, k);
		cout << "query initialed" << endl;
		if(graphtype == "TopicPP"){
			AssignTopicAwareEdgeWeight(g, words, Q);
		}
		vector<int> seeds;
		clock_t start = clock();
		APPROX::Result result;
		if(inftype == "IRIE"){
			result = approx1(g, Q, words);
		}
		if(inftype == "MC"){
			result = approx1Sim(g, Q, words);
		}
		clock_t end = clock();
		cout<<"finished"<<endl;


		double influ = result.influence;
		query_log << "query " << query_counter << " : " << result.invoke_time << " "<<result.influence<<" "<<influ<<endl;
		query_counter++;
		if (result.seeds.size() == 0)query_log << "not found" << endl;
		ofs << result.seeds.size() << " ";
		for (unsigned i = 0; i<result.seeds.size(); i++){
			ofs << result.seeds[i] << " ";
		}
		ofs << endl;
		//query_log<<"influence is : "<<influence<<endl;
		query_log << "time cost is : " << (end - start) << " ms" << endl;
		keyword_pattern_tried = 0;
		total_time += (end - start);
		found = false;
		maxInflu = 0;
	}
	query_log<<"total time cost "<<total_time<<" ms"<<endl;
	query_log.close();
}

void PICS(string graphfile, string keywordfile, string queryfile, string graphtype, string inftype){
	string logfile = queryfile+"_"+"PICS_"+graphtype+"_"+inftype+".log";
	query_log.open(logfile);
	ofstream ofs(logfile + ".seed.log");
	InfGraph g;
	IRIE_min_stopping_criteria = 0.0001;
	IRIE_alpha = 0.4;
	IRIE_theta = 1.0 / 320;
	double theta = 0.001;
	double c_alpha = 0.6;
	if(graphtype == "Trained"){
		ConstructProbGraph(graphfile, g);
	}
	else{
		ConstructGraph(graphfile, g);
		if(graphtype == "Random"){
			AssignEdgeWeightRandom(g);
		}
		if(graphtype == "Degree"){
			AssignEdgeWeight(g);
		}
	}
	cout << "graph loaded" << endl;
	Keywords words;
	ReadKeywords(words, keywordfile);
	cout << "words loaded" << endl;
	cout << words.size() << " users" << endl;
	ifstream ifs_query(queryfile);
	int query_counter = 1;
	double total_time = 0;
	while (!ifs_query.eof()){
		int k, wordNum, w;
		set<int> QueryWords;
		ifs_query >> k >> wordNum;
		for (unsigned i = 0; i < wordNum; ++i){
			ifs_query >> w;
			QueryWords.insert(w);
		}
		Query Q;
		InitialQuery(Q, QueryWords, k);
		cout << "query initialed" << endl;
		if(graphtype == "TopicPP"){
			AssignTopicAwareEdgeWeight(g, words, Q);
		}
		vector<int> seeds;
		clock_t start = clock();
		APPROX::Result result;
		if(inftype == "IRIE"){
			 result = pics(g, Q, words);
		}
		clock_t end = clock();
		cout<<"finished"<<endl;


		double influ = result.influence;
		query_log << "query " << query_counter << " : " << result.invoke_time << " "<<result.influence<<" "<<influ<<endl;
		query_counter++;
		if (result.seeds.size() == 0)query_log << "not found" << endl;
		ofs << result.seeds.size() << " ";
		for (unsigned i = 0; i<result.seeds.size(); i++){
			ofs << result.seeds[i] << " ";
		}
		ofs << endl;
		//query_log<<"influence is : "<<influence<<endl;
		query_log << "time cost is : " << (end - start) << " ms" << endl;
		keyword_pattern_tried = 0;
		total_time += (end - start);
		found = false;
		maxInflu = 0;
	}
	query_log<<"total time cost "<<total_time<<" ms"<<endl;
	query_log.close();
}

void Influence(string graphfile, string keywordfile, string queryfile, string graphtype, string seedsfile){
	string logfile = seedsfile+".influence";
	query_log.open(logfile);
	ifstream ifs_seeds(seedsfile);
	InfGraph g;
	IRIE_min_stopping_criteria = 0.0001;
	IRIE_alpha = 0.4;
	IRIE_theta = 1.0 / 320;
	double theta = 0.001;
	double c_alpha = 0.6;
	if(graphtype == "Trained"){
		ConstructProbGraph(graphfile, g);
	}
	else{
		ConstructGraph(graphfile, g);
		if(graphtype == "Random"){
			AssignEdgeWeightRandom(g);
		}
		if(graphtype == "Degree"){
			AssignEdgeWeight(g);
		}
	}
	cout << "graph loaded" << endl;
	Keywords words;
	ReadKeywords(words, keywordfile);
	cout << "words loaded" << endl;
	cout << words.size() << " users" << endl;
	ifstream ifs_query(queryfile);
	int query_counter = 1;
	double total_time = 0;
	while (!ifs_query.eof()){
		int k, wordNum, w;
		set<int> QueryWords;
		ifs_query >> k >> wordNum;
		for (unsigned i = 0; i < wordNum; ++i){
			ifs_query >> w;
			QueryWords.insert(w);
		}
		Query Q;
		InitialQuery(Q, QueryWords, k);
		cout << "query initialed" << endl;
		if(graphtype == "TopicPP"){
			AssignTopicAwareEdgeWeight(g, words, Q);
		}
		vector<int> seeds;
		for(unsigned i=0; i < k; i++){
			int seed;
			ifs_seeds>>seed;
			seeds.push_back(seed);
		}
		double influence1 = MultiMCSim(seeds, g);
		query_log<<influence1<<endl;
	}
	query_log.close();
	ifs_seeds.close();
	ifs_query.close();
}