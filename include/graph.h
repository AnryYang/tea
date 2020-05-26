/*************************************************************************
    > File Name: graph.h
    > Author: anryyang
    > Mail: anryyang@gmail.com 
    > Created Time: Thu 28 Sep 2017 02:20:29 PM
 ************************************************************************/

#include<iostream>

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include "utils.h"

class Graph{
    public:
        std::vector<std::vector<int>> m_edges;
        std::vector<int> m_deg;
    private:
        std::string m_folder;
        std::string m_graph;
        uint m_n;
        uint64 m_m;
        double m_avgDeg;
        std::vector<std::vector<int>> m_cmty;   // community list
        std::vector<std::vector<int>> m_cmtyMap;    // node: cmty_ids, one node may belong to several cmty

        void readNM();
        void readGraph();
        void readCommunity();
        void readGraphMemMap();
        void addEdge(int u, int v);
    public:
        uint getDeg(int u) const;
        uint64 getM() const;
        uint getN() const;
        double getAvgDeg() const;
        std::string getGraphFolder() const;
        const std::vector<int>& operator [] (int u) const;
        int randOutNeighbor(int u) const;
        void samplePath(int src, uint len, std::vector<int>& vecPath) const;
        int sampleTarget(int src, uint len) const;
        const std::vector<int>& getNoeCmtyIds(int u) const;
        const std::vector<int>& getCmtyById(int i) const;

        Graph(const std::string& t_folder, const std::string& t_graph);
};
#endif