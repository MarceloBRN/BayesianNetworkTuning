#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "scenario.h"

#ifndef _OPENMP
#define _OPENMP
#endif

#if defined(_OPENMP)
   #include <omp.h>
#endif


#define MAX 32766

class Population
{
public:
    Population();
    Population(const Scenario &scenario, const int &num_jobs = 1);
    Population(const Scenario &scenario, const size_t &pop_size, const int &num_jobs = 1);
    void sort();
    void swap(const size_t &x, const size_t &y);
    void concat(const Population &&p);
    //void first(size_t size);
    const Individual first() const;
    const size_t size();
    const Scenario get_scenario() const;
    const Individual get_indinvidual(const size_t &idx) const;
    void add_child(const Individual& child);
    void resize(const size_t& new_size);
    double runtime_time = 0.0;

    void print(std::ostream &output = std::cout) const;
    void print_best(std::ostream &output = std::cout) const;
    void print_all(std::ostream &output = std::cout) const;
    

    virtual ~Population();

private:
    std::vector<Individual> _ind;
    
    
    size_t _incomplete_pop_size;
    Scenario _scenario;
};

#endif // POPULATION_H