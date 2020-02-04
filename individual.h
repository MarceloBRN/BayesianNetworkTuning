#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include "scenario.h"

class Individual
{
public:
    Individual();
    Individual(const Scenario &scenario);

    int *individual;

    double calc_fitness(int * &vector);

    double cost;

private:

    Scenario scenario;
};

#endif // INDIVIDUAL_H
