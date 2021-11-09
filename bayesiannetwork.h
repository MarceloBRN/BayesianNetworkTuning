#ifndef BAYESIANNETWORK_H
#define BAYESIANNETWORK_H

#include <fstream>
#include <iostream>
#include <vector>
#include <tuple>
#include "population.h"

class Node 
{
public:
    Node() {
        _out.resize(0);
        _in.resize(0);
    }
    size_t _id; //index
    std::vector<size_t> _out;
    std::vector<size_t> _in;

    virtual ~Node(){
        _out.clear();
        _in.clear();
    }

} ;

class BayesianNetwork
{
public:
    BayesianNetwork(const Population &pop, const int &num_jobs = 1);
    size_t arcs() const;
    void print(std::ostream &output = std::cout) const;
    void set_from(const size_t& from, const size_t& to);
    void set_to(const size_t& to, const size_t& from);
    void set_edge(const size_t& n1, const size_t& n2);
    void topological_sort();
    Population *sample_from_network(const int &num_samples);


    virtual ~BayesianNetwork();

private:
    BayesianNetwork(const size_t &size);
    std::vector<Node> _node;
    std::vector<Node> _ordered;
    std::vector<double *> _gains;
    Scenario _scenario;
    Population _pop;
    size_t _num_nodes = 0;
    int _num_jobs = 1;
    std::vector<std::vector<double>> _probs;

    const std::tuple<size_t, size_t, double> compute_new_edge();
    double *compute_gains(const Node& node);
    std::vector<size_t> get_viable_parents(const size_t &id_node);
    bool include(const std::vector<size_t> &v, const size_t &val) const;
    bool can_add_edge(const size_t &i, const size_t &j) const;
    bool path_exists(const size_t &i, const size_t &j) const;
    double k2equation(const size_t &id_node, const size_t &index_candidate, const std::vector<size_t> &candidates);
    std::vector<size_t> compute_count_for_edges(const std::vector<size_t> &id_node);
    Individual *probabilistic_logic_sample();
    std::vector<double> calculate_probability(const Node &node);
    std::vector<double> marginal_probability(const size_t &id_node);
    const double fact(const double &v);
    const std::tuple<std::vector<double>, double> get_compact_probs(const Node &nod, const std::vector<size_t> &parents);
};

#endif // BAYESIANNETWORK_H