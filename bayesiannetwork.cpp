#include "bayesiannetwork.h"

BayesianNetwork::BayesianNetwork(int numnodes)
{
    node = new Node[numnodes];
    for(int i=0;i < numnodes;i++){
       node[i].index = i;
       node[i].out.clear();
       node[i].in.clear();
    }
    size = numnodes;
}

int BayesianNetwork::arcs()
{
    int sum = 0;
    for(int i = 0; i < size; i++){
       sum = sum + static_cast<int>(node[i].out.size());
    }

    return sum;
}
