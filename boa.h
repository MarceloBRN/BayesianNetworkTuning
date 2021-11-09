#ifndef BOA_H
#define BOA_H

#include "population.h"
#include "bayesiannetwork.h"

class Boa
{
public:
    Boa(const Scenario &scenario, const int &num_jobs = 1);
    void run();
    
    //double shannon_entropy(T first, T last) const;

private:

    Scenario _scenario;
    Population *_pop;
    Individual _best;
    BayesianNetwork *_graph = nullptr;
    size_t _numparam = 0;
    int _num_jobs = 1;
    std::vector<size_t> _similarity;

    double _calculate_similarity(const Individual& id1, const Individual& id2);


};

#endif // BOA_H