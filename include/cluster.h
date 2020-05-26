/*************************************************************************
    > File Name: sweep.h
    > Author: anryyang
    > Mail: anryyang@gmail.com 
    > Created Time: Thu 28 Sep 2017 02:27:30 PM
 ************************************************************************/

#include<iostream>
#include<vector>
#include<unordered_map>
#include "graph.h"

// given a heat kernel score vector, return a smallest-conductance cluster
double sweepCut(int seed, std::vector<double>&& mapHK, std::vector<std::pair<int, double>>& vecCluster, Config& config, const Graph& graph);
double targetSweepCut(int seed, std::unordered_map<int,double>& mapHK, std::vector<pair<int, double>>& vecCluster, Config& config, const Graph& graph);

void clusterToFile(std::vector< std::pair<int, std::vector<std::pair<int, double>>> >& vecClusters, string strFolder);

std::vector<int> loadSeed(std::string folder, std::string file_name, int count);

void targetCluster(Config& config, const Graph& graph);

void batchTargetCluster(Config& config, const Graph& graph);

void cluster(Config& config, const Graph& graph);
void batchCluster(Config& config, const Graph& graph);
// void batchRelativeCluster(Config& config, const Graph& graph);

vector<double> measureOne(int seed, vector<pair<int, double>>& vecCluster, const Graph& graph);
vector<double> measureAll(vector<pair<int,vector<pair<int, double>>>>& vecClusters, const Graph& graph);

void saveRanking(int seed, vector<double>& mapHK, Config& config, const Graph& graph);