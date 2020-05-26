/*************************************************************************
    > File Name: sampler.h
    > Author: anryyang
    > Mail: anryyang@gmail.com 
    > Created Time: Fri 29 Sep 2017 10:35:25 AM
 ************************************************************************/

#ifndef SAMPLER_H
#define SAMPLER_H

#include<iostream>
#include<math.h>
#include<vector>
#include<map>
#include<unordered_map>
#include<random>
#include<boost/random/poisson_distribution.hpp>
#include<boost/random/variate_generator.hpp>
#include<boost/random/mersenne_twister.hpp>

#include "graph.h"

class DiscreteAliasSampler{
    public:
        std::vector<pair<uint,uint>> veckN;
    private:
        size_t m_n;
        std::vector<double> vecProb;
        std::vector<uint64> vecInv;
    public:
        void init(std::unordered_map<uint64, double>& mapResidue, uint64 rsize, double gamma, const Graph& graph, Config& config);
        uint64 sample();
        void batchSample(uint64 num, std::queue<uint64>& S);
        size_t size();
        DiscreteAliasSampler();
};

class PoissonProb{
public:
    std::vector<double> m_jump;
    std::vector<double> m_stay;
    std::vector<double> m_vecSqrtStay;
    std::vector<double> m_vecSqrtJump;
    std::vector<double> m_vecPoisson;
    std::vector<double> m_vecPoissonSum;
private:
    double m_t;
    uint m_k;
public:
    PoissonProb();
    PoissonProb(double t);
    // PoissonProb(double t, int k);
    double getJumpProb(uint k);
    double getStayProb(uint k);
    double getRemainProb(uint k);
};

#endif
