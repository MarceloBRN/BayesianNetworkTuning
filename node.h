#ifndef NODE_H
#define NODE_H

#include <vector>

using namespace std;

class Node
{
public:
    Node();

    int index;
    vector<int> out;
    vector<int> in;

    int size_out;
    int size_in;

};

#endif // NODE_H
