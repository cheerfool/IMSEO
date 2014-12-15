#include <string>
#include "expMethod.h"

int main(int argc, char** argv){

	//testKIMMSP("soc-Epinions-u", "user_tags-final", "new-multicover-query-5#5");



	//TraditionalISP("soc-Epinions-u", 10, 0.00001);
	//system("pause");
	string exp_type, graph_file, graph_type, inf_type, keyword_file, query_file;
    string dir="/Users/jkl/Courses/CPSC534/example/";
    string graph=dir+"networks";
    string attr=dir+"attributes";
    string query=dir+"q";
    string factors[6]={"SG",graph,"Degree","IRIE",attr,query};
	//test(argv[1]);
	//system("pause");
/*	if(argc < 7){
		cout<<"KIMExp exp_type graph_file keyword_file query_file"<<endl;
		cout<<"exp_type:"<<endl;
		cout<<"\tPICS"<<endl;
		cout<<"\tPICSP"<<endl;
		cout<<"\tSG"<<endl;
		cout<<"\tPG"<<endl;
		cout<<"\tinfluence"<<endl;
		cin>>exp_type;
		cout<<"graph file:"<<endl;
		cin>>graph_file;
		cout<<"graph type:"<<endl;
		cout<<"\tTopicPP"<<endl;
		cout<<"\tRandom"<<endl;
		cout<<"\tTrained"<<endl;
		cout<<"\tDegree"<<endl;
		cin>>graph_type;
		cout<<"inf type:"<<endl;
		cout<<"\tIRIE"<<endl;
		cout<<"\tMC"<<endl;
		cin>>inf_type;
		cout<<"keyword file"<<endl;
		cin>>keyword_file;
		cout<<"query file"<<endl;
		cin>>query_file;
	}
	else{
		exp_type.assign(argv[1]);
		graph_file.assign(argv[2]);
		graph_type.assign(argv[3]);
		inf_type.assign(argv[4]);
		keyword_file.assign(argv[5]);
		query_file.assign(argv[6]);
	}
 */
    exp_type.assign(factors[0]);
    graph_file.assign(factors[1]);
    graph_type.assign(factors[2]);
    inf_type.assign(factors[3]);
    keyword_file.assign(factors[4]);
    query_file.assign(factors[5]);
    
//    cout<<exp_type<<endl<<graph_file<<endl<<graph_type<<endl<<inf_type<<endl<<keyword_file<<endl<<query_file<<endl;

	if(exp_type == "SG"){
		ScoreGreedy(graph_file, keyword_file, query_file, graph_type, inf_type);
	}
	else if(exp_type == "PG"){
		PigeonGreedy(graph_file, keyword_file, query_file, graph_type, inf_type);
	}
	else if(exp_type == "PICS"){
		PICS(graph_file, keyword_file, query_file, graph_type, inf_type);
	}
	else if(exp_type == "PICSP"){
		PICSP(graph_file, keyword_file, query_file, graph_type, inf_type);
	}
	else if(exp_type == "influence"){
		Influence(graph_file, keyword_file, query_file, graph_type, inf_type);
	}

	//if(exp_type == "KIMNAIVE"){
	//	cout<<"KIMNAIVE"<<endl;
	//	testKIMNaive(graph_file, keyword_file, query_file);
	//}
	///*
	//else if(exp_type == "KIMNAIVEISP"){
	//	cout<<"KIMNAIVEISP"<<endl;
	//	testKIMNaiveISP(graph_file, keyword_file, query_file);
	//}*/
	//else if(exp_type == "KIMNAIVEIRIE"){
	//	cout<<"KIMNAIVEIRIE"<<endl;
	//	testKIMNaiveIRIE(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "TOPICPIGEON"){
	//	testTOPICPIGEON(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "TOPICPIGEONADJUST"){
	//	testTOPICPIGEONADJUST(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "NEWKIMIRIE"){
	//	cout<<"NEWKIMIRIE"<<endl;
	//	testKIMnewIRIE(graph_file, keyword_file, query_file);
	//}
	///*
	//else if(exp_type == "KIMNAIVEDEGREE"){
	//	cout<<"KIMNAIVEDEGREE"<<endl;
	//	testKIMNaiveDegree(graph_file, keyword_file, query_file);
	//}*/
	///*
	//else if(exp_type =="IRIE2"){
	//	cout<<"IRIE2"<<endl;
	//	testKIMIRIE2(graph_file, keyword_file, query_file);
	//}*/
	//else if(exp_type == "INFLUENCE"){
	//	cout<<"influence"<<endl;
	//	computeInfluence(graph_file, keyword_file);
	//}
	//else if(exp_type == "TOPICINFLUENCE"){
	//	string seed_file;
	//	seed_file.assign(argv[5]);
	//	computeInfluence(graph_file, keyword_file, query_file, seed_file);
	//}
	//else if(exp_type == "EASYSEED"){
	//	easyseed(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "SP"){
	//	testKIMSP(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "MSP"){
	//	testKIMMSP(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "TEMP"){
	//	testTEMP(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "TEMPADJUST"){
	//	testTEMPADJUST(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "WSC"){
	//	testSCNaive(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "TOPICWSC"){
	//	testSCTOPICNaive(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "TOPICWSCADJUST"){
	//	testSCTOPICNaiveADJUST(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "SPSIM"){
	//	testKIMSPSIM(graph_file, keyword_file, query_file);
	//}
	//else if (exp_type == "SCORESIM"){
	//	testScoreGreedySIM(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "PIGEONSIM"){
	//	testPigeonGreedySIM(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "TEMPSIM"){
	//	testTEMPSIM(graph_file, keyword_file, query_file);
	//}
	//else if (exp_type == "APPROXIRIE"){
	//	testIRIEApprox1(graph_file, keyword_file, query_file);
	//}
	//else if (exp_type == "TRAINPICSP"){
	//	testTRAINEDPICSP(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "TRAINWSC"){
	//	testTRAINWSC(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "TRAINPIGEON"){
	//	testTRAINPIGEON(graph_file, keyword_file, query_file);
	//}
	//else if(exp_type == "PICS"){
	//	testTOPICPICS(graph_file, keyword_file, query_file);
	//}
	/* 5-28
	else if(exp_type == "TESTTAG"){
		testTag(graph_file, keyword_file, query_file);
	}
	else if(exp_type == "TESTPROB"){
		testTrainingProb(graph_file, keyword_file);
	}*/
	//NaiveNeighborUpdate("soc-Epinions-u", 30, 30);
	//testKIMSelect();
	//testISPKIMSelect();
	//EasyNeighborUpdate("soc-Epinions-u", 30, 30);
	//AdvanceNeighborUpdate("soc-Epinions-u", 30, 30, 0.00001);
	//TraditionalISP("soc-Epinions-u", 30, 0.00001);
	//set<int> Qwords;
	//KIMBasic("soc-Epinions-u", "words", 10, Qwords);
	//KIMSelect("soc-Epinions-u", "words", 10, Qwords);
	//testUpdateConvex();
	//testNUKIMSelect();
	//system("pause");
	return 0;
}