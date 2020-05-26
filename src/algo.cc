/*************************************************************************
    > File Name: algo.cc
    > Author: anryyang
    > Mail: anryyang@gmail.com 
    > Created Time: Thu 28 Sep 2017 02:26:32 PM
 ************************************************************************/

#include "algo.h"
#include <math.h>
#include <iostream>
#include <queue>
#include <list>
#include <algorithm>
#include "mtwist.h"
#include <random>

using namespace std;

myMap<int> mapN;
myMap<int> mapM;

uint taylorDegree(double t, double epsilon){
    assert(t<=1000);
    double eps_exp_t = epsilon*exp(t);
    double error = exp(t)-1;
    double last = 1.;
    double k = 0.;
    while(error > eps_exp_t){
        k = k + 1.;
        last = (last*t)/k;
        error = error - last;
    }
    return std::max((int)k, (int)1);
}

// https://github.com/osimpson/hkpr
void chkprSetting(Config& config, const Graph& graph){
    config.maxLen = 9*log(1.0/config.epsilon)/log(log(1.0/config.epsilon));
    assert(config.maxLen>1);
    config.numWalk = 16.0*log(1./config.pfail)/pow(config.epsilon, 3);
    config.display();
}

void chkpr(int src, vector<double>& mapHK, Config& config, const Graph& graph){
    double incre = 1.0/config.numWalk;
    std::mt19937 mrand(std::time(NULL));
    std::poisson_distribution<int> dist(config.t);
    for(uint64 i=0; i<config.numWalk; i++){
        int k = dist(mrand);
        if(k<=config.maxLen){
            int v = graph.sampleTarget(src, k);
            mapHK[v] += incre;
        }
    }
}

void mcSetting(Config& config, const Graph& graph){
    config.numWalk = 2*(1+config.epsilon/3.0)*log(graph.getN()*1.0/config.pfail)/config.delta/pow(config.epsilon,2);
    config.display();
}


void mc(int src, vector<double>& mapHK, PoissonProb& poisson, Config& config, const Graph& graph){
    double incre = 1.0/config.numWalk;
    for(uint64 i=0; i<config.numWalk; i++){
        uint cur=src;
        uint k = 0;
        while(true){
            double kp = poisson.m_stay[k];
            if(mt_drand()<=kp){
                break;
            }
            else{
                uint j = mt_lrand()%graph.m_deg[cur];
                cur = graph[cur][j];
            }
            k++;
        }
        mapHK[cur] += incre;
    }
}

void pi(int src, vector<double>& mapHK, PoissonProb& poisson, Config& config, const Graph& graph){
    static unordered_map<uint64, double> mapR;
    queue<uint64> Q;
    static uint n_nodes = graph.getN();

    mapR[src] = 1.0;
    Q.push(src);

    Timer tm(98, "fwd push");
    while(!Q.empty()){
        uint64 uk = Q.front();
        uint u = uk%n_nodes;
        uint k = uk/n_nodes;
        if(k==config.maxLen){return;}
        Q.pop();
        double& residue = mapR[uk];
        double reserve = residue*poisson.m_stay[k];
        mapHK[u] += reserve;
        residue -= reserve;
        double update = residue/graph.m_deg[u];
        mapR.erase(uk);
        for(const auto& v: graph[u]){
            uint64 vk = (k+1)*n_nodes+v;
            double& rvk = mapR[vk];
            rvk += update;
            double rvkd = rvk/graph.m_deg[v];
            Q.push(vk);
        }
    }
}


void teaplusSetting(Config& config, const Graph& graph){
    assert(config.epsilon>0 && config.epsilon<1.0);
    config.maxLen = (int)(config.scale*log(1.0/(config.epsilon*config.delta))/log(2*graph.getM()*1.0/graph.getN())+0.5);
    config.rmax = config.epsilon*config.delta/config.maxLen;
    config.numWalk = 2.0*8*(1+config.epsilon/6.0)*log(1./config.pfail)/config.delta/pow(config.epsilon,2);
    config.numPush = config.numWalk*config.t/2.0;
    config.display();
}

void teaplus(int src, vector<double>& mapHK, PoissonProb& poisson, Config& config, const Graph& graph){
    static unordered_map<uint64, double> mapR;
    queue<uint64> Q;
    static uint n_nodes = graph.getN();

    uint64 curPush = 1;
    double curError=0;
    double curMaxR=0;
    double curMaxRninq=0;
    double nextMaxR=0;
    double nextMaxRninq=0;

    double epsilon_a = config.rmax*config.maxLen;

    mapR[src] = 1.0;
    Q.push(src);
    curMaxRninq = 0;
    curMaxR = mapR[src]/graph.m_deg[src];

    uint jumpK = config.maxLen;

    if(curPush < config.numPush){
        Timer tm(98, "fwd push");
        for(uint k=0; k<=config.maxLen; k++){
            if( (Q.empty() || curPush>=config.numPush || curError+curMaxR<=epsilon_a || k>=config.maxLen) && k>0 ){
                curError+=curMaxR;
                jumpK = k;
                break;
            }
            nextMaxR = 0;
            nextMaxRninq = 0;
            while(!Q.empty() && curPush<config.numPush){
                uint64 uk = Q.front();
                uint u = uk%n_nodes;
                uint l = uk/n_nodes;
                if(l!=k){break;}
                Q.pop();
                double& residue = mapR[uk];
                double reserve = residue*poisson.m_stay[k];
                mapHK[u] += reserve;
                residue -= reserve;
                double update = residue/graph.m_deg[u];
                curPush += graph.m_deg[u];
                mapR.erase(uk);
                for(const auto& v: graph[u]){
                    double threshold = graph.m_deg[v]*config.rmax;
                    uint64 vk = (k+1)*n_nodes+v;
                    double& rvk = mapR[vk];
                    rvk += update;
                    double rvkd = rvk/graph.m_deg[v];
                    if(rvk-update<threshold && rvk>=threshold){
                        Q.push(vk);
                    }
                    if(rvk<threshold && rvkd>nextMaxRninq){
                        nextMaxRninq = rvkd;
                    }
                    if(rvkd>nextMaxR){
                        nextMaxR = rvkd;
                    }
                }
            }
            curError += curMaxRninq;
            curMaxRninq = nextMaxRninq;
            curMaxR = nextMaxR;
        }
    }

    // cout << "push: " << curPush << "/" << config.numPush << " err:" << curError << " epsilon:" << epsilon_a << " k:" << jumpK << endl;

    if(curError>epsilon_a){
        uint64 rsize = 0;
        double gamma = 0;
        vector<double> vecResi(config.maxLen+1, 0);
        {
        Timer tm1(101, "residue");
        for(auto it = mapR.begin(); it != mapR.end(); ++it){
            uint u = it->first%n_nodes;
            uint k = it->first/n_nodes;
            double r = it->second;
            double rk = r*poisson.m_stay[k];
            double rkd = r-rk;
            it->second = rkd;
            mapHK[u] += rk;
            gamma += rkd;
            vecResi[k] += rkd;
        }
        }

        {
        Timer tm1(102, "gamma");
        double gamma_inv = 1.0/gamma;
        gamma = 0;
        for(auto it = mapR.begin(); it != mapR.end(); ){
            uint u = it->first%n_nodes;
            uint k = it->first/n_nodes;
            double r = it->second;
            double rx = r-vecResi[k]*gamma_inv*epsilon_a*graph.m_deg[u];
            if(rx<=0){
                // it = mapR.erase(it);
                it->second = 0;
                ++it;
            }
            else{
                it->second = rx;
                gamma += rx;
                ++it;
                rsize++;
            }
        }

        //cout << "rsum: " << gamma << endl;
        }

        static DiscreteAliasSampler sampler;
        {
            Timer tm(103, "sample");
            //cout<< rsize << endl;
            sampler.init(mapR, rsize, gamma, graph, config);
        }
        uint64 numTrial = gamma*config.numWalk;
        double incre = 1.0/config.numWalk;

        // cout << "curErr:" << curError << " Err:" << epsilon_a << " walks:" << numTrial << endl;
        Timer tm(100, "randwalk");
        for(uint64 i=0; i<numTrial; i++){
            uint64 x = sampler.sample();
            uint k = sampler.veckN[x].first;
            uint s = sampler.veckN[x].second;
            uint l=k;
            uint cur = s;
            while(true){
                double kp = poisson.m_stay[k];
                if(l!=k && mt_drand()<=kp){
                    break;
                }
                else{
                    // uint j = mt_lrand()%graph.m_deg[cur];
                    uint j = xorshift32()%graph.m_deg[cur];
                    cur = graph[cur][j];
                }
                k++;
            }
            mapHK[cur] += incre;
        }
    }
    else{
        Timer tm(99, "conversion");
        for(auto it = mapR.begin(); it != mapR.end(); ++it){
            uint u = it->first%n_nodes;
            uint k = it->first/n_nodes;
            double r = it->second;
            mapHK[u] += r*poisson.m_stay[k];
        }
    }

    mapR.clear();
    return;
}



void teaSetting(Config& config, const Graph& graph){
    assert(config.epsilon>0 && config.epsilon<1.0);
    config.numWalk = 2*2*(1+config.epsilon/3.0)*log(1./config.pfail)/config.delta/pow(config.epsilon,2);
    config.rmax = config.scale*1.0/config.numWalk/config.t;
    config.display();
}

void tea(int src, vector<double>& mapHK, PoissonProb& poisson, Config& config, const Graph& graph){
    static unordered_map<uint64, double> mapR;
    queue<uint64> Q;
    static uint n_nodes = graph.getN();

    mapR[src] = 1.0;
    Q.push(src);
    double gamma = 0;
    uint64 rsize = 0;
{
        Timer tm(98, "fwd push");
        while(!Q.empty()){
            uint64 uk = Q.front();
            uint u = uk%n_nodes;
            uint k = uk/n_nodes;
            Q.pop();
            double& residue = mapR[uk];
            double reserve = residue*poisson.m_stay[k];
            mapHK[u] += reserve;
            residue -= reserve;
            double update = residue/graph.m_deg[u];
            mapR.erase(uk);
            for(const auto& v: graph[u]){
                double threshold = graph.m_deg[v]*config.rmax;
                uint64 vk = (k+1)*n_nodes+v;
                double& rvk = mapR[vk];
                rvk += update;
                double rvkd = rvk/graph.m_deg[v];
                if(rvk-update<threshold && rvk>=threshold){
                    Q.push(vk);
                }
            }
        }

        //Timer tm1(101, "residue");
        for(auto it = mapR.begin(); it != mapR.end(); ++it){
            uint u = it->first%n_nodes;
            uint k = it->first/n_nodes;
            double r = it->second;
            double rk = r*poisson.m_stay[k];
            double rkd = r-rk;
            it->second = rkd;
            mapHK[u] += rk;
            gamma += rkd;
            rsize++;
        }

        //cout << "rsum: " << gamma << endl;
}


{
    Timer tm(100, "randwalk");
    static DiscreteAliasSampler sampler;
    sampler.init(mapR, rsize, gamma, graph, config);
    uint64 numTrial = gamma*config.numWalk;
    double incre = 1.0/config.numWalk;

    
    for(uint64 i=0; i<numTrial; i++){
        uint64 x = sampler.sample();
        uint k = sampler.veckN[x].first;
        uint s = sampler.veckN[x].second;
        uint cur = s;
        uint l=k;
        while(true){
            double kp = poisson.m_stay[k];
            if(l!=k && mt_drand()<=kp){
                break;
            }
            else{
                uint j = xorshift32()%graph.m_deg[cur];
                cur = graph[cur][j];
            }
            k++;
        }
        mapHK[cur] += incre;
    }

}
    mapR.clear();
    return;
}