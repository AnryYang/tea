# Heat Kernel based Community Detection

## Installation
### Requirements

### Compile

## Related Code
- Fast Bidirectional Probability Estimation in Markov
Models, NIPS 2015. [[pdf]](https://papers.nips.cc/paper/5815-fast-bidirectional-probability-estimation-in-markov-models.pdf) [[code]](http://cs.stanford.edu/~plofgren/heat_kernel_experiments.zip)
- Heat Kernel Based Community Detection, KDD 2014. [[pdf]](https://arxiv.org/pdf/1403.3148.pdf) [[code]](https://gist.github.com/dgleich/7d904a10dfd9ddfaf49a)
- Solving Local Linear Systems with Boundary Conditions Using Heat Kernel Pagerank, Internet Mathematics 2015. [[pdf]](https://arxiv.org/pdf/1503.03157.pdf) [[code]](https://github.com/osimpson/hkpr)
- Parallel Local Graph Clustering, VLDB 2016. [[pdf]](http://people.csail.mit.edu/jshun/local.pdf) [[code]](http://github.com/jshun/ligra)

## How to Use

## Third-party Library
- Mersenne Twist pseudorandom number generator: http://www.cs.hmc.edu/~geoff/mtwist.html


## Dataset
- actor: http://konect.uni-koblenz.de/networks/actor-collaboration
- wordnet: http://konect.uni-koblenz.de/networks/wordnet-words
- flixster: http://konect.uni-koblenz.de/networks/flixster
- dblp, orkut, LJ, friendster: snap.stanford.edu/data/index.html
- twitter: http://an.kaist.ac.kr/traces/WWW2010.html
- flickr: http://konect.uni-koblenz.de/networks/flickrEdges
- facebook: http://konect.uni-koblenz.de/networks/facebook-wosn-links
- cit-Patents: https://graphchallenge.mit.edu/data-sets


## How to use
#### Requirements
- GNU Linux system
- GCC 4.8
- Boost library

#### Compile
```sh
$ cmake .
$ make clena all -j4
```

#### Run
```sh
$ ./hk -a teaplus -f ./data/ -g dblp -n 50 -s seeds.txt
```