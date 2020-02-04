#ifndef BAYESIANNETWORK_H
#define BAYESIANNETWORK_H

#include <vector>

using namespace std;

typedef struct _node {
    _node() {
        out.resize(0);
        in.resize(0);

        size_out = 0;
        size_in = 0;
    }

    int index;
    vector<int> out;
    vector<int> in;

    int size_out;
    int size_in;
}Node;

class BayesianNetwork
{
public:
    BayesianNetwork(int numnodes);

    Node *node;
    int size;

    int arcs();

};

#endif // BAYESIANNETWORK_H
