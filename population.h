#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "individual.h"

#define MAX 32766

using namespace std;

class Population
{
public:
    Population(int pop_size, const Scenario &scenario);
    void sort();
    void swap(vector<Individual> *v, int x, int y);
    void first(int size);

    vector<Individual> ind;
    int pop_size;
    //ParamReader param;
    Scenario scenario;
};

#endif // POPULATION_H
