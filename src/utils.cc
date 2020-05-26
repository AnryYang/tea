/*************************************************************************
    > File Name: utils.cc
    > Author: anryyang
    > Mail: anryyang@gmail.com 
    > Created Time: Thu 28 Sep 2017 02:25:42 PM
 ************************************************************************/

#include<iostream>
#include <sys/resource.h>
#include <unistd.h>
#include <ios>
#include <fstream>
#include "utils.h"

using namespace std;

void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   uint64 vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}

void disp_mem_usage(){
    double vm, rss;

    process_mem_usage(vm, rss);
    vm/=1024;
    rss/=1024;
    //cout<< "Memory Usage:" << msg << " vm:" << vm << " MB  rss:" << rss << " MB"<<endl;
    cout<< "PhysicalMem(MB) "<< rss << endl;
    cout<< "VirtualMem(MB) "<< vm << endl;
}

uint getProcMemory(){
	struct rusage r_usage;
	getrusage(RUSAGE_SELF, &r_usage);
	//string strMemUsage = to_string(r_usage.ru_maxrss);
	return r_usage.ru_maxrss / 1024.0 ;
	//return strMemUsage;
}

vector<double> Timer::timeUsed;
vector<string> Timer::timeUsedDesc;

void xorshifinit(){
    x_state = (uint32_t)time(NULL);
}
uint32_t xorshift32(void){
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	x_state ^= x_state << 13;
	x_state ^= x_state >> 17;
	x_state ^= x_state << 5;
	return x_state;
}

void fastSrand(){
    g_seed = time(NULL);
}

uint32_t fastRand() {
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}