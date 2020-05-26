/*************************************************************************
    > File Name: hk.cc
    > Author: anryyang
    > Mail: anryyang@gmail.com 
    > Created Time: Thu 28 Sep 2017 02:19:39 PM
 ************************************************************************/

#include<iostream>
#include<boost/program_options.hpp>
#include<string>

#include "graph.h"
#include "cluster.h"

using namespace std;
namespace po = boost::program_options;

namespace { 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0; 
  const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
 
} // namespace

Config parseParams(int argc, char** argv){
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("data-folder,f", po::value<string>()->required(), "graph data folder")
        ("graph-name,g", po::value<string>()->required(), "graph file name")
        ("algo,a", po::value<string>()->required(), "algorithm name")
        ("seed,s", po::value<string>(), "seed file name")
        ("epsilon,e", po::value<double>()->default_value(0), "epsilon")
        ("max-len,l", po::value<int>()->default_value(0), "max length")
        ("heat-t,t", po::value<double>()->default_value(5), "heat kernel t")
        ("pfail,p", po::value<double>()->default_value(0), "failure probability")
        ("delta,d", po::value<double>()->default_value(0), "delta")
        ("num-seed,n", po::value<int>()->default_value(10), "number of seeds/queries")
        ("scale,c", po::value<double>()->default_value(2.5), "scale for max-len")
    ;

    po::variables_map vm; 
    po::store(po::parse_command_line(argc, argv, desc),  vm); // can throw 
    po::notify(vm);

    Config config;

    if (vm.count("help")){
        cout << desc << '\n';
        exit(0);
    }
    if (vm.count("data-folder")){
        config.strFolder = vm["data-folder"].as<string>();
    }
    if (vm.count("graph-name")){
        config.strGraph = vm["graph-name"].as<string>();
    }
    if (vm.count("algo")){
        config.strAlgo = vm["algo"].as<string>();
    }
    if (vm.count("seed")){
        config.strSeed = vm["seed"].as<string>();
    }
    if (vm.count("epsilon")){
        config.epsilon = vm["epsilon"].as<double>();
    }
    if (vm.count("delta")){
        config.delta = vm["delta"].as<double>();
    }
    if (vm.count("heat-t")){
        config.t = vm["heat-t"].as<double>();
    }
    if (vm.count("pfail")){
        config.pfail = vm["pfail"].as<double>();
    }
    if (vm.count("max-len")){
        config.maxLen = vm["max-len"].as<int>();
    }
    if (vm.count("num-seed")){
        config.numSeed = vm["num-seed"].as<int>();
    }
    if (vm.count("scale")){
        config.scale = vm["scale"].as<double>();
    }

    return config;
}

int main(int argc, char **argv){
    Config config;
    try{
        config = parseParams(argc, argv);
        config.check();
    }
    catch (const exception &ex){
        cerr << ex.what() << '\n';
        return ERROR_IN_COMMAND_LINE;
    }

    Graph graph(config.strFolder, config.strGraph);

    cluster(config, graph);
    
    Timer::show();

    return SUCCESS;
}
