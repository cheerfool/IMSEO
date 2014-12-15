#ifndef EXP_METHOD_H
#define EXP_METHOD_H
#include<time.h>
#include<sstream>
#include<string.h>

#include"keywordCombine.h"

#include"weightedSetCover.h"
#include"MCSim.h"
#include"scoreGreedy.h"
#include"Approx.h"


void ScoreGreedy(string graphfile, string keywordfile, string queryfile, string graphtype, string inftype);

void PigeonGreedy(string graphfile, string keywordfile, string queryfile, string graphtype, string inftype);

void PICSP(string graphfile, string keywordfile, string queryfile, string graphtype, string inftype);

void PICS(string graphfile, string keywordfile, string queryfile, string graphtype, string inftype);

void Influence(string graphfile, string keywordfile, string queryfile, string graphtype, string seedsfile);
#endif