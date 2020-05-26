/*************************************************************************
    > File Name: graph.cc
    > Author: anryyang
    > Mail: anryyang@gmail.com 
    > Created Time: Thu 28 Sep 2017 02:20:50 PM
 ************************************************************************/
#include "graph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "mtwist.h"

#define ASSERT(v) {if (!(v)) {cerr<<"ASSERT FAIL @ "<<__FILE__<<":"<<__LINE__<<endl; exit(1);}}

using namespace std;

void handle_error(const char* msg) {
	perror(msg);
	exit(255);
}

Graph::Graph(const string& t_folder, const string& t_graph): 
            m_folder(t_folder), m_graph(t_graph) {
    mt_goodseed();
    xorshifinit();
    readNM();
    cout << "graph info: n=" << m_n << " m=" << m_m << " avg-degree=" << this->m_avgDeg << endl;

    this->m_edges = std::vector<std::vector<int>>(m_n, std::vector<int>());
    this->m_edges = std::vector<std::vector<int>>(m_n, std::vector<int>());
    this->m_cmtyMap = std::vector<std::vector<int>>(m_n, std::vector<int>());
    this->m_deg = std::vector<int>(m_n, 0);

    readGraph();
    readCommunity();
}

void Graph::readNM(){
    ifstream fin((m_folder + "/" + m_graph + "/attribute.txt").c_str());
    string s;
    if (fin.is_open()){
        while (fin >> s){
            if (s.substr(0, 2) == "n="){
                this->m_n = atoi(s.substr(2).c_str());
                continue;
            }
            if (s.substr(0, 2) == "m="){
                this->m_m = atoi(s.substr(2).c_str());
                continue;
            }
        }
        fin.close();
        this->m_avgDeg = this->m_m*1.0/this->m_n;
    }
    else handle_error("Fail to open attribute file!");
}

void Graph::readCommunity(){
    ifstream fin((m_folder + "/" + m_graph + "/community5000.txt").c_str());
    std::string line;
    unsigned long i=0;
    if (fin.is_open()){
        while (std::getline(fin, line)){
            this->m_cmty.push_back(std::vector<int>());
            std::istringstream lineStr(line);
            int v;
            while(lineStr>>v){
                this->m_cmty[i].push_back(v);
                this->m_cmtyMap[v].push_back(i);
            }
            i+=1;
        }
        fin.close();
    }
    else{
        cout << "Fail to open community file!" << endl;
    }
}

void Graph::readGraph(){
    FILE *fin = fopen((m_folder + "/" + m_graph + "/graph.txt").c_str(), "r");
    uint64 readCnt = 0;
    int u, v;
    while (fscanf(fin, "%d%d", &u, &v) != EOF) {
        readCnt++;
        ASSERT( u < m_n );
        ASSERT( v < m_n );
        if(u == v)
            continue;
        addEdge(u, v);
        // addEdge(v, u);
    }
    fclose(fin);
    ASSERT(readCnt == m_m);
    // ASSERT(readCnt == 2*m_m);
    cout << "read Graph: Done!" << endl;
}

void Graph::readGraphMemMap(){
    size_t length;
    int fd = open((m_folder + "/" + m_graph + "/graph.txt").c_str(), O_RDWR);
    if (fd == -1)
        handle_error("open");
    struct stat sb;
    int rc = fstat(fd, &sb);
    if (rc == -1)
        handle_error("fstat");

    length = sb.st_size;
    auto ptr = static_cast<char*>(mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0u));  //byte by byte
    auto f = ptr;

    int gap = 2 * sizeof(int);
    //ASSERT(fin != false);
    uint64 readCnt = 0;
    int a, b;
    for (int i = 0; i < m_m; i++){
        readCnt ++;
        memcpy(&a, f, sizeof(int));
        memcpy(&b, f + sizeof(int), sizeof(int));
        f += gap;
        ASSERT( a < m_n );
        ASSERT( b < m_n );
        addEdge(a, b);
    }

    ASSERT(readCnt == m_m);
    rc = munmap(ptr, length);
    close(fd);
    cout << "read Graph: Done!" << endl;
}

void Graph::addEdge(int u, int v){
    this->m_edges[u].push_back(v);
    this->m_edges[v].push_back(u);
    this->m_deg[u]+=1;
    this->m_deg[v]+=1;
}

uint Graph::getDeg(int u) const{
    // return this->m_edges[u].size();
    return this->m_deg[u];
}

uint Graph::getN() const{
    return this->m_n;
}

uint64 Graph::getM() const{
    return this->m_m;
}

double Graph::getAvgDeg() const{
    return this->m_avgDeg;
}

std::string Graph::getGraphFolder() const{
    std::string folder(m_folder + "/" + m_graph + "/");
    return folder;
}

const std::vector<int>& Graph::operator[](int u) const{
    // if(u>=this->m_n||u<0){
    //     cout << "node id: " << u <<endl;
    //     handle_error("error node id!");
    // }
    return this->m_edges[u];
}

int Graph::sampleTarget(int src, uint len) const{
    int cur=src;
    for(uint i=0; i<len; i++){
        int v = mt_lrand()%this->m_deg[cur];
        cur = this->m_edges[cur][v];
    }
    return cur;
}

const std::vector<int>& Graph::getNoeCmtyIds(int u) const{
    return this->m_cmtyMap[u];
}

const std::vector<int>& Graph::getCmtyById(int i) const{
    return this->m_cmty[i];
}
