/*************************************************************************
    > File Name: sweep.cc
    > Author: anryyang
    > Mail: anryyang@gmail.com 
    > Created Time: Thu 28 Sep 2017 02:25:08 PM
 ************************************************************************/

#include<iostream>
#include<fstream>
#include <algorithm>
#include "cluster.h"
#include "algo.h"
#include "mtwist.h"

using namespace std;

int max_t = 700;

double sweepCut(int seed, vector<double>& mapHK, vector<pair<int, double>>& vecCluster, Config& config, const Graph& graph){
    //for(int v=0; v<mapHK.size(); v++){
    for(int v=0; v<mapHK.size(); v++){
        double rho = mapHK[v];
        if(rho>0){
            vecCluster.push_back(make_pair(v, rho/graph.getDeg(v)));
        }
    }

    // sort nodes by hk scores
    sort(vecCluster.begin(), vecCluster.end(), 
        [](pair<int, double> const& l, pair<int, double> const& r){return l.second > r.second;});

    // int j=0;
    // for(auto& p: vecCluster){
    //    cout << "NO." << j++ << " node: " << p.first << " hk:" << p.second << endl;
    // }

    double volS = 0.;
    double cutS = 0.;
    double conductance=1.0;
    int cutPoint=0;
    uint64 totalVol = graph.getM()*2;
    int i=0;
    unordered_map<int, bool> S;
    for(auto& p: vecCluster){
        //compute conductance
        int v = p.first;
        // double score = p.second;
        volS += graph.getDeg(v);

        for(int u: graph[v]){
            if(S.find(u)==S.end()) // this neighbor do not exist in S, a new cut found
                cutS++;
            else // this neighbor already exists in S, an old cut should be deleted
                cutS--;
        }
        S[v]=true;

        double cur_conductance = cutS/min(volS, totalVol-volS);
        if(cur_conductance<conductance){
            conductance=cur_conductance;
            // cout << "better conductance: " << conductance << " cut: "  << i << endl;
            cutPoint=i;
        }
        i++;

        // reach the maximum allowed cluster size
        // if(i>=config.clusterSize){
        //     break;
        // }
    }
    // cout << cutS << " " << volS << " " << totalVol << endl;

    vecCluster = vector<pair<int, double>>(vecCluster.begin(), vecCluster.begin()+cutPoint);
    return conductance;
}

vector<double> measureOne(int seed, vector<pair<int, double>>& vecCluster, const Graph& graph){
    // how many predicated neighbor are exact
    vector<int> cmtyIds = graph.getNoeCmtyIds(seed);
    unordered_map<int, bool> neighbors;
    for(int cmtyId: cmtyIds){
        vector<int> cmty = graph.getCmtyById(cmtyId);
        for(int v: cmty){
            neighbors[v]=true;
        }
    }

    int num_pred=0;
    for(auto& p: vecCluster){
        if(neighbors.find(p.first)!=neighbors.end()){
            num_pred++;
        }
    }

    double prec = 0;
    if(vecCluster.size()>0)
        prec = num_pred*1.0/vecCluster.size();

    double recall = 0;
    if(neighbors.size()>0)
        recall=num_pred*1.0/neighbors.size();

    vector<double> vecMeasure(3);
    vecMeasure[0]=prec;
    vecMeasure[1]=recall;

    if(prec==0||recall==0)
        vecMeasure[2]=0;
    else
        vecMeasure[2]=2.0*prec*recall/(prec+recall);

    return vecMeasure;
}

vector<double> measureAll(vector<pair<int,vector<pair<int, double>>>>& vecClusters, const Graph& graph){
    vector<double> vecMeasure(3);
    for(int i=0; i<vecClusters.size(); i++){
        vector<double> res = measureOne(vecClusters[i].first, vecClusters[i].second, graph);

        vecMeasure[0]+=res[0];
        vecMeasure[1]+=res[1];
        vecMeasure[2]+=res[2];
    }

    vecMeasure[0] /= vecClusters.size();
    vecMeasure[1] /= vecClusters.size();
    vecMeasure[2] /= vecClusters.size();
    return vecMeasure;
}

void saveRanking(int seed, vector<double>& mapHK, Config& config, const Graph& graph){
    std::ofstream file;
    file.open(config.getRankFile(), std::ios::out | std::ios::app);
    if (file.fail())
        throw std::ios_base::failure(std::strerror(errno));
    file << seed << ":";

    vector<pair<int ,double>> temp;
    for(int v=0; v<mapHK.size(); v++){
        double rho = mapHK[v]/graph.getDeg(v);
        if(rho>0){
            temp.push_back(MP(v, rho));
        }
    }
    sort(temp.begin(), temp.end(), [](pair<int, double> const& l, pair<int, double> const& r){return l.second > r.second;} ); 
    for(auto &p: temp){
        if(p.second > 0){
            file << p.first << "\t" << p.second;
        }
        file << ",";
    }
    file << endl;
}

vector<int> loadSeed(string folder, string file_name, int count){
    FILE *fin = fopen((folder + "/" + file_name).c_str(), "r");
    int s;
    vector<int> seeds;
    int i=0;
    while (fscanf(fin, "%d", &s) != EOF) {
        seeds.push_back(s);
        i++;
        if(i>=count)
            break;
    }
    fclose(fin);
    cout << "read seed: Done!" << endl;
    return seeds;
}

// given a target conductance, target volume, find a cluster by performing a sweep over a hkpr vector
void cluster(Config& config, const Graph& graph){
    vector<double> mapHK(graph.getN());
    mt_goodseed();
    xorshifinit();

    vector<int> seeds = loadSeed(graph.getGraphFolder(), config.strSeed, config.numSeed);
    if(seeds.size()==0){
        for(int i=0; i<config.numSeed; i++){
            int seed = rand()%graph.getN();
            seeds.push_back(seed);
        }
    }

    config.setDefault(graph.getN(), graph.getM());
    PoissonProb poisson;
    uint default_num=1.0e7;
    int dist_counter = 5;

    vector<double> vecPhis;
    vector<double> vecPushCoeff;

    if(config.strAlgo==CHKPR){
        chkprSetting(config, graph);
    }
    else if(config.strAlgo==MC){
        mcSetting(config, graph);
        poisson = PoissonProb(config.t);
    }
    else if(config.strAlgo==TEAPLUS){
        teaplusSetting(config, graph);
        poisson = PoissonProb(config.t);
    }
    else if(config.strAlgo==TEA){
        teaSetting(config, graph);
        poisson = PoissonProb(config.t);
    }
    else if(config.strAlgo==PI){
        config.display();
        poisson = PoissonProb(config.t);
    }

    //if( remove( config.getRankFile().c_str() ) != 0 )
    //    cout << "Error deleting file" << endl;
    //else
    //    cout << "File successfully deleted" << endl;

    int hk_counter=1;
    int sweep_counter=2;
    int i=0;
    double conductance = 0.;
    double avg_conductance = 0.;
    int avg_cluster_size = 0.;
    vector<pair<int,vector<pair<int, double>>>> vecClusters(seeds.size());
    for(int seed: seeds){
        cout << "NO." << i+1 << " seed:" << seed << endl;
        cout << "\tcomputing heat kernel..." << endl;
        std::fill(mapHK.begin(), mapHK.end(), 0);
        {
            Timer tm(hk_counter, "heat kernel time");
            if(config.strAlgo==CHKPR){
                chkpr(seed, mapHK, config, graph);
            }
            else if(config.strAlgo==MC){
                mc(seed, mapHK, poisson, config, graph);
            }
            else if(config.strAlgo==TEAPLUS){
                teaplus(seed, mapHK, poisson, config, graph);
            }
            else if(config.strAlgo==PI){
                pi(seed, mapHK, poisson, config, graph);
            }
            else if(config.strAlgo==TEA){
                tea(seed, mapHK, poisson, config, graph);
            }
        }

        //saveRanking(seed, mapHK, config, graph);
        
        cout << "\tsweep cutting..." << endl;
        vector<pair<int, double>> vecCluster;
        vecCluster.reserve(graph.getN());
        {
            Timer tm(sweep_counter, "sweep cut time");
            conductance = sweepCut(seed, mapHK, vecCluster, config, graph);
        }
        avg_conductance+=conductance;
        avg_cluster_size+=vecCluster.size();
        cout << "\tConductance: " << conductance << " Cluster-size: " << vecCluster.size() << endl;
        vecClusters[i] = make_pair(seed, vecCluster);
        i++;
        cout << "\tcurrent secs per query: " << Timer::used(hk_counter)/i << endl;
        cout << "\tcurrent avg. conductance: " << avg_conductance/i << endl;
        cout << "\tcurrent avg. cluster size: " << avg_cluster_size/i << endl;
    }

    disp_mem_usage();

    vector<double> vecMeasure = measureAll(vecClusters, graph);

    cout << "=========================================================" << endl;
    cout << Timer::used(hk_counter)*1000/config.numSeed << " milli-seconds per hkpr query" << endl;
    cout << Timer::used(sweep_counter)*1000/config.numSeed << " milli-seconds per sweep" << endl;
    cout << (Timer::used(sweep_counter)+Timer::used(hk_counter))*1000/config.numSeed << " milli-seconds per clustering" << endl;
    cout << avg_conductance/config.numSeed << " average conductance" << endl;
    cout << avg_cluster_size/config.numSeed << " average cluster size" << endl;
    cout << vecMeasure[0] << " average precision" << endl;
    cout << vecMeasure[1] << " average recall" << endl;
    cout << vecMeasure[2] << " average F1-measure" << endl;
    cout << "=========================================================" << endl;
}
