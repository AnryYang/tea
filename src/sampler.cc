/*************************************************************************
    > File Name: sampler.cc
    > Author: anryyang
    > Mail: anryyang@gmail.com 
    > Created Time: Fri 29 Sep 2017 10:35:41 AM
 ************************************************************************/

#include "sampler.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <stack>
#include <assert.h>
#include <functional>
#include <numeric>      // std::accumulate

#include "mtwist.h"

using namespace std;

// Walker's alias method
// http://code.activestate.com/recipes/576564-walkers-alias-method-for-random-objects-with-diffe/
// https://github.com/cantino/walker_method/
DiscreteAliasSampler::DiscreteAliasSampler() {
    srand(time(NULL));
    mt_seed();
}

void DiscreteAliasSampler::init(std::unordered_map<uint64, double>& mapResidue, uint64 rsize, double gamma, const Graph& graph, Config& config){
    m_n = rsize;//mapResidue.size();
    veckN.resize(m_n);
    vecInv.resize(m_n);
    vecProb.resize(m_n);

    uint n_nodes = graph.getN();
    uint i=0;
    std::deque<uint> shortQ;
    std::deque<uint> longQ;
    for(auto it = mapResidue.begin(); it != mapResidue.end(); ++it){
        uint u = it->first%n_nodes;
        uint k = it->first/n_nodes;
        double r = it->second;
        if(r==0){
            continue;
        }
        veckN[i] = MP(k,u);
        vecProb[i] = r*m_n/gamma;
        vecInv[i]=-1;

        if(vecProb[i]<1){
            shortQ.push_back(i);
        }
        else if(vecProb[i]>1){
            longQ.push_back(i);
        }

        ++i;
    }

    while(!shortQ.empty() && !longQ.empty()){
        uint j = shortQ.back();
        shortQ.pop_back();
        uint k = longQ.back();
        vecInv[j]=k;
        vecProb[k]-=(1-vecProb[j]);
        if(vecProb[k]<1){
            shortQ.push_back(k);
            longQ.pop_back();
        }
    }
}

uint64 DiscreteAliasSampler::sample(){
    double prob = mt_drand();
    uint i = xorshift32()%m_n;
    return prob<=vecProb[i]?i:vecInv[i];
}

void DiscreteAliasSampler::batchSample(uint64 num, queue<uint64>& S){
    for(uint64 j=0; j<num; j++){
        double prob = mt_drand();
        uint i = xorshift32()%m_n;
        if(prob<=vecProb[i]){
            S.push(i);
        }
        else{
            S.push(vecInv[i]);
        }
    }
}

size_t DiscreteAliasSampler::size(){
    return m_n;
}

PoissonProb::PoissonProb(){}

PoissonProb::PoissonProb(double t):m_t(t){
    m_k = 100000;
    m_vecPoisson.resize(m_k+1);
    m_vecPoissonSum.resize(m_k+2);
    m_vecPoisson[0] = 1.0/exp(m_t);
    m_vecPoissonSum[0] = 0;
    m_vecPoissonSum[1] = m_vecPoissonSum[0] + m_vecPoisson[0];
    for(int k=1; k<=m_k; k++){
        m_vecPoisson[k] = m_vecPoisson[k-1]*m_t*1.0/k;
        m_vecPoissonSum[k+1] = m_vecPoissonSum[k] + m_vecPoisson[k];
    }
    m_stay.resize(m_k+1);
    m_jump.resize(m_k+1);
    m_vecSqrtStay.resize(m_k+1);
    m_vecSqrtJump.resize(m_k+1);
    for(int k=0; k<=m_k; k++){
        m_stay[k] = m_vecPoisson[k]/(1.0-m_vecPoissonSum[k]);
        m_vecSqrtStay[k] = sqrt(m_stay[k]);
        m_jump[k] = 1.0-m_stay[k];
        m_vecSqrtJump[k] = sqrt(m_jump[k]);
    }
}

// PoissonProb::PoissonProb(double t, int k):m_t(t), m_k(k){
//     m_vecPoisson.resize(m_k+1);
//     m_vecPoissonSum.resize(m_k+2);
//     m_vecPoisson[0] = 1.0/exp(m_t);
//     m_vecPoissonSum[0] = 0;
//     m_vecPoissonSum[1] = m_vecPoisson[0];
//     for(int k=1; k<=m_k; k++){
//         m_vecPoisson[k] = m_vecPoisson[k-1]*m_t*1.0/k;
//         m_vecPoissonSum[k+1] = m_vecPoissonSum[k] + m_vecPoisson[k];
//     }
//     m_stay.resize(m_k+1);
//     m_jump.resize(m_k+1);
//     m_vecSqrtStay.resize(m_k+1);
//     m_vecSqrtJump.resize(m_k+1);
//     for(int k=0; k<=m_k; k++){
//         m_stay[k] = m_vecPoisson[k]/(1.0-m_vecPoissonSum[k]);
//         m_vecSqrtStay[k] = sqrt(m_stay[k]);
//         m_jump[k] = 1.0-m_stay[k];
//         m_vecSqrtJump[k] = sqrt(m_jump[k]);
//     }
// }

double PoissonProb::getJumpProb(uint k){
    return m_jump[k];
}

double PoissonProb::getStayProb(uint k){
    return m_stay[k];
}

double PoissonProb::getRemainProb(uint k){
    return 1.0-m_vecPoissonSum[k];
}
