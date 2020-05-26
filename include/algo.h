/*************************************************************************
    > File Name: algo.h
    > Author: anryyang
    > Mail: anryyang@gmail.com 
    > Created Time: Thu 28 Sep 2017 02:27:17 PM
 ************************************************************************/

#include<vector>
#include <unordered_map>
#include <map>
#include "graph.h"
#include "sampler.h"

extern myMap<int> mapN;
extern myMap<int> mapM;

uint taylorDegree(double t, double epsilon);
void chkprSetting(Config& config, const Graph& graph);
void chkpr(int src, std::vector<double>& mapHK, Config& config, const Graph& graph);

void mcSetting(Config& config, const Graph& graph);
void mc(int src, vector<double>& mapHK, PoissonProb& poisson, Config& config, const Graph& graph);

void teaplusSetting(Config& config, const Graph& graph);
void teaplus(int src, vector<double>& mapHK, PoissonProb& poisson, Config& config, const Graph& graph);

void teaSetting(Config& config, const Graph& graph);
void tea(int src, vector<double>& mapHK, PoissonProb& poisson, Config& config, const Graph& graph);

void pi(int src, vector<double>& mapHK, PoissonProb& poisson, Config& config, const Graph& graph);