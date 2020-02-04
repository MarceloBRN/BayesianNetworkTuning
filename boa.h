#ifndef BOA_H
#define BOA_H

#include <vector>
#include <cmath>
#include <stack>
#include <fstream>
#include <queue>
#include "population.h"
#include "boaparameter.h"
#include "bayesiannetwork.h"

using namespace std;

class Boa
{
public:
    Boa();
    Boa(const Scenario &scenario, const BOAParameter &boaparam);
    void search();
    void construct_network(BayesianNetwork * &graph, const Population &pop, int num_bits); //OK
    void compute_gains(double * & gains, const Node &node, const BayesianNetwork &graph, const Population &pop); //OK
    void get_viable_parents(vector<int> &viable, const int &index_node, const BayesianNetwork &graph); //OK
    bool can_add_edge(const int &i, const int &j, const BayesianNetwork &graph); //OK
    bool include(const vector<int> &v, const int &val) noexcept; //OK
    double k2equation(const int &index_node, const vector<int> &candidates, const int &index_candidate, const Population &pop); //OK
    bool path_exists(const int &i, const int &j, const BayesianNetwork &graph) noexcept; //OK
    void compute_count_for_edges(int * &counts, const Population &pop, const vector<int> &indexes); //OK
    void topological_ordering(BayesianNetwork * &ordered, const BayesianNetwork &graph); //OK
    void marginal_probability(double * &prob, const int &index_node, const Population &pop); //OK
    void calculate_probability(double * &prob, const Node &node, int *&numstring, const BayesianNetwork &graph, const Population &pop); //OK
    void probabilistic_logic_sample(int * &lsample, const BayesianNetwork &graph, const Population &pop); //OK
    void sample_from_network(vector<int *> &numstring, const Population &pop, const BayesianNetwork &graph, const int &num_samples); //OK
    double fact(double v);  //OK
    int power(int a, int b); //OK

    //PRINT
    void imprimeGraph(const BayesianNetwork &graph);
    void imprimeVectorInt(vector<int> v);
    void imprimeVectorNode(vector<Node> v);
    void imprimeQueueNode(queue<Node> v);
    void imprimePopulation(const Population &pop);
    void imprimeIndividual(Individual ind);
    void imprimeVectorIntComum(int * &v, const int &length);
    void imprimeVectorDoubleComum(double *v, int length);

    Scenario scenario;
    BOAParameter boaparam;
    Population *pop;
    Individual *best;

    BayesianNetwork *graphT;

private:
    int tempo(int i, int l);



};

#endif // BOA_H
