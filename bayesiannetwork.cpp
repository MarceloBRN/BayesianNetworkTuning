#include "bayesiannetwork.h"
#include <queue>
#include <chrono>

const double fat[] = {
    1.0,
    1.0,
    2.0,
    6.0,
    24.0,
    120.0,
    720.0,
    5040.0,
    40320.0,
    362880.0,
    3628800.0,
    39916800.0,
    479001600.0
};

BayesianNetwork::BayesianNetwork(const Population &pop, const int &num_jobs) {
    _pop = pop;
    _scenario = _pop.get_scenario();
    _num_nodes = _scenario.getParamHeader().getNumParam();
    _num_jobs = num_jobs;

    size_t max_edges = _num_nodes;
    //size_t max_edges = 5;
    double v = 0.0;
    double max = -1.0f;
    size_t from = -1;
    size_t to = -1;
    std::tuple<size_t, size_t, double> edge;

    for(size_t i = 0; i < _num_nodes; i++){
        Node n;
        n._id = i;
        n._out.resize(0);
        n._out.clear();
        n._in.resize(0);
        n._in.clear();
       _node.push_back(n);
    }

    _gains.resize(0);

    for(size_t g = 0; g < max_edges; g++){
        if(_gains.size() > 0){
            for(size_t i = 0; i < _num_nodes; i++){
                delete[] _gains[i];
            }
            _gains.clear();
        }

        edge = compute_new_edge();

        from = std::get<0>(edge);
        to = std::get<1>(edge);
        max = std::get<2>(edge);

        if (max <= 0.0f) {
            break;
        }
            
        set_edge(from, to);

    }

    this->topological_sort();

    _probs.resize(_num_nodes);
    for(size_t i = 0; i < _num_nodes; i++){
        _probs[_node[i]._id] = calculate_probability(_node[i]);
    }

    //_ordered.clear();
}

BayesianNetwork::BayesianNetwork(const size_t &size){
    _scenario = _pop.get_scenario();
    _num_nodes = _scenario.getParamHeader().getNumParam();

    //size_t max_edges = 3 * _pop->size();
    size_t max_edges = _num_nodes;
    double v = 0.0;
    double max = -1.0f;
    size_t from = -1;
    size_t to = -1;

    for(size_t i = 0; i < _num_nodes; i++){
        Node n;
        n._id = i;
        n._out.resize(0);
        n._out.clear();
        n._in.resize(0);
        n._in.clear();
       _node.push_back(n);
    }

    _gains.resize(0);

    _probs.resize(0);
}

const std::tuple<size_t, size_t, double> BayesianNetwork::compute_new_edge(){

    double v = 0.0;
    double max = 0.0;
    std::tuple<size_t, size_t, double> edge;
    size_t from, to;

    omp_set_num_threads(_num_jobs);
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        #pragma omp critical
        {
             std::cout << "[THREAD " << thread_id << ", COMPUTER GAIN: START] " << std::endl;
        }

        for (size_t i = static_cast<size_t>(thread_id); i < _num_nodes; i += _num_jobs){
            

            double *g = compute_gains(_node[i]);

            #pragma omp critical
            {
                _gains.push_back(g);
            }
        }

        std::cout << "[THREAD " << thread_id << ", COMPUTER GAIN: FINISH]" << std::endl;
    }

    for(size_t i = 0; i < _num_nodes; i++){
        for(size_t j = 0; j < _num_nodes; j++){
            v = _gains[i][j];
            if(v > max){
                from = i;
                to = j;
                max = v;
            }

        }

    }

    std::get<0>(edge) = from;
    std::get<1>(edge) = to;
    std::get<2>(edge) = max;

    return edge;
}


double *BayesianNetwork::compute_gains(const Node& node)
{
    std::vector<size_t> viable;
    size_t max = 2;

    double *aux = new double[_node.size()];
    for (int x = 0; x < _node.size(); x++) {
        aux[x] = -1.0f;
    }

    viable = get_viable_parents(node._id);

    for (size_t i = 0; i < _node.size(); i++){
        if(_node[i]._in.size() < max && include(viable, i)){
            aux[i] = k2equation(node._id, i, node._in);
        }
        else {
            aux[i] = -1.0;
        }
    }

    viable.clear();

    return aux;

}


std::vector<size_t> BayesianNetwork::get_viable_parents(const size_t &id_node)
{
    std::vector<size_t> viable;

    for (size_t i = 0; i < _node.size(); i++){
        if(id_node != i && can_add_edge(id_node, i)){
            viable.push_back(i);
        }
    }

    return viable;
}

bool BayesianNetwork::can_add_edge(const size_t &i, const size_t &j) const {
    return (!(include(_node[i]._out, j)) && !(path_exists(j, i)));
}

bool BayesianNetwork::include(const std::vector<size_t> &v, const size_t &val) const {
    bool flag = false;
    if(v.size() > 0){
        for(size_t i = 0; i < v.size(); i++){
            if(v.at(i) == val){
                flag = true;
                break;
            }
        }
    }
    return flag;
}


bool BayesianNetwork::path_exists(const size_t &i, const size_t &j) const
{
    std::vector<size_t> visited;
    std::vector<size_t> stacki;

    visited.resize(0);
    stacki.resize(0);

    bool flag = false;

    int k = -1;
    stacki.push_back(i);

    while(!(stacki.empty())){

        if(include(stacki, j)){
            flag =  true;
            break;
        }

        k = stacki.back();
        stacki.pop_back();

        if(!include(visited, k)){
            visited.push_back(k);

            for (size_t m = 0; m < _node[k]._out.size(); m++){
                if(!include(visited, _node[k]._out[m])){
                    stacki.push_back(_node[k]._out[m]);
                }
            }
        }

    }
    return flag;
}


double BayesianNetwork::k2equation(const size_t &id_node, const size_t &index_candidate, const std::vector<size_t> &candidates)
{
    std::vector<size_t> cand;
    size_t *a = new size_t[_scenario.getParamHeader().getDomain(id_node).size()];
    double total = 1.0f, val;
    bool flag = false;

    cand.push_back(id_node);

    for(size_t i = 0; i < candidates.size(); i++){
        cand.push_back(candidates.at(i));
    }
    cand.push_back(index_candidate);

    std::vector<size_t> counts = compute_count_for_edges(cand);

    val = 1;
    for (size_t n = 0; n < cand.size(); n++) {
        if (cand[n] != id_node) {
            val *= _scenario.getParamHeader().getDomain(cand[n]).size();
        }
    }

    double rs;
    size_t sum = 0;

    for(size_t i = 0; i < val; i++){

        for(size_t j = 0; j < _scenario.getParamHeader().getDomain(id_node).size(); j++){
            a[j] = counts[(i * _scenario.getParamHeader().getDomain(id_node).size()) + j];
        }

        sum = 0;

        for(size_t j = 0; j < _scenario.getParamHeader().getDomain(id_node).size(); j++){
            sum = sum + a[j];
        }

        if((sum + _scenario.getParamHeader().getDomain(id_node).size() - 1) > 12){
            rs = static_cast<double>(_scenario.getParamHeader().getDomain(id_node).size() - 1) / fact(sum + _scenario.getParamHeader().getDomain(id_node).size() - 1);
        } else {
            rs = static_cast<double>(_scenario.getParamHeader().getDomain(id_node).size() - 1) / fat[sum + _scenario.getParamHeader().getDomain(id_node).size() - 1];
        }

        for(size_t l = 0; l < _scenario.getParamHeader().getDomain(id_node).size(); l++){
            rs = rs * fat[a[l]];
        }


        if(flag == false){
            total = rs;
            flag = true;
        }else{
            total = total*rs;
        }
    }

    if (a != nullptr) {
        delete[] a;
    }
    
    
    return total;
}

std::vector<size_t> BayesianNetwork::compute_count_for_edges(const std::vector<size_t> &indexes){
    
    size_t valor = 1;
    for (size_t n = 0; n < indexes.size(); n++) {
        valor *= _scenario.getParamHeader().getDomain(indexes[n]).size();
    }

    std::vector<size_t> counts;
    for(size_t c = 0; c < valor; c++){
        counts.push_back(0);
    }

    size_t index;
    size_t v;

    for(size_t p = 0; p < _pop.size(); p++){
        index = 0;

        size_t it = indexes.size() - 1;

        v = indexes[it];
        index += _pop.get_indinvidual(p).get_data()[v];

        for(int i = it - 1; i >= 0; i--){

            v = indexes[i];
            valor = 1;
            
            for (int n = it; n > i; n--) {
                valor *= _scenario.getParamHeader().getDomain(indexes[n]).size();
            }
            index += _pop.get_indinvidual(p).get_data()[v] * valor;
   
        }
        counts[index]++;
    }

    return counts;
}


size_t BayesianNetwork::arcs() const
{
    size_t sum = 0;
    for(size_t i = 0; i < _node.size(); i++){
       sum = sum + static_cast<size_t>(_node[i]._out.size());
    }
    return sum;
}

void BayesianNetwork::print(std::ostream &output) const
{
    size_t i, j;
    output.flush();
    output << "digraph G {\n";
    for(i = 0; i < _node.size(); i++){
        if(!_node[i]._out.empty() || !_node[i]._in.empty()){
            output << "\t" << _node[i]._id  << " [label=\"" << _scenario.getParamHeader().getParms().at(_node[i]._id) << "\"]" << std::endl;
        }
    }

    output << std::endl;

    for(i = 0; i < _node.size(); i++){
        if(!_node[i]._out.empty()){
            output << "\t" << _node[i]._id << " -> ";
            for(j = 0; j < (_node[i]._out.size() - 1); j++){
                output << _node[i]._out[j] << ", ";
            }
            output << _node[i]._out[j] << std::endl;
        }
    }
    output << "}\n";
}

void BayesianNetwork::set_from(const size_t& from, const size_t& to){
    _node[from]._out.push_back(to);
}

void BayesianNetwork::set_to(const size_t& to, const size_t& from){
    _node[to]._in.push_back(from);
}

void BayesianNetwork::set_edge(const size_t& n1, const size_t& n2){
    _node[n1]._out.push_back(n2);
    _node[n2]._in.push_back(n1);
}


void BayesianNetwork::topological_sort()
{
    std::vector<size_t> count;
    size_t edge;

    for(size_t i = 0; i < _num_nodes; i++){
        count.push_back(static_cast<size_t>(_node[i]._in.size()));
    }

    //BayesianNetwork *ordered = new BayesianNetwork(_scenario.getParamHeader().getNumParam());
    std::vector<Node> ordered = _node;
    std::vector<size_t> vordered;
    Node *current =  nullptr;
    std::queue<Node> stacki;

    vordered.clear();
    //ordered.resize(_num_nodes);
    //ordered.clear();

    for(size_t i = 0; i < _num_nodes; i++){
        if(count[i] == 0){
            stacki.push(ordered[i]);
        }
    }

    size_t n;
    Node *node =  nullptr;

    while(vordered.size() < _num_nodes){
        
        if(!stacki.empty()){
            current = new Node(stacki.front());

            stacki.pop();
            for(size_t j = 0; j < current->_out.size(); j++){
                if(current != nullptr)
                    edge = current->_out[j];

                for(n = 0; n < _num_nodes; n++){
                    if(ordered[n]._id == edge){
                        if(node != nullptr)
                            delete node;
                        node = new Node(ordered[n]);
                        break;

                    }
                }
                count[n]--;
                if(count[n] <= 0){
                    stacki.push(*node);
                }
            }
            if(current != nullptr){
                vordered.push_back(current->_id);
                delete current;
            }
                
        }
        
    }
    
    for(size_t i = 0; i < _num_nodes; i++){
        ordered[i]._id = vordered[i];
        ordered[i]._out = _node[vordered[i]]._out;
        ordered[i]._in = _node[vordered[i]]._in;
    }

    _ordered.clear();
    _ordered = ordered;

    // if(node != nullptr)
    //     delete node;

    // if(current != nullptr)
    //     delete current;

}

Population *BayesianNetwork::sample_from_network(const int &num_samples){

    // size_t k = 0;
    // while(_node[k]._in.size() < 1 && k < (_pop.size() - 1)){
    //     k++;
    // }
    // std::vector<size_t> aux;
    // aux.push_back(_node[k]._id);

    // for(size_t i = 0; i < _node[k]._in.size(); i++){
    //     aux.push_back(_node[k]._in[i]);
    // }
    // std::vector<size_t> counts = compute_count_for_edges(aux);

    // this->print();

    // std::cout << "Teste\n";

    Population *sample = new Population();

    omp_set_num_threads(1);
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();

        for (size_t i = static_cast<size_t>(thread_id); i < num_samples; i += _num_jobs){

            Individual *aux = probabilistic_logic_sample();

            #pragma omp critical 
            {
                std::cout << "[THREAD " << thread_id << ", CHILD: " << i << "]: " << "ADD" << std::endl;
                sample->add_child(*aux);
            }
        }

        std::cout << "[THREAD " << thread_id << " FINISH]" << std::endl;
    }
    

    return sample;
}

Individual *BayesianNetwork::probabilistic_logic_sample() {

    std::vector<size_t> data;
    data.clear();
    //data.resize(_num_nodes);
    for(size_t i = 0; i < _num_nodes; i++){
        data.push_back(0);
    }
    double sumprob = 0.0;
    double threshold = 0.0;

    double min = 0.0;
	double max = 0.0;
	std::uniform_real_distribution<double> distribution;

    thread_local std::mt19937 _generator;
    uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    _generator.seed(seed);

    std::tuple<std::vector<double>, double> c_probs;
    std::vector<double> node_probs;

    size_t new_value = 0;
    Individual *result = nullptr;
    size_t k = 0;
    size_t index = 0;

    std::vector<size_t> parents;

    // std::cout << "[DEBUG] Domains: ";
    // for(size_t m = 0; m < _scenario.getParamHeader().getNumParam(); m++){
    //     std::cout << "[DEBUG] " << m << ": " << _scenario.getParamHeader().getDomain(m).size() << std::endl;
    // }
    // std::cout << std::endl;

    for(size_t i = 0; i < data.size(); i++){

        parents.resize(0);
        parents.clear();

        for(size_t p = 0; p < _ordered[i]._in.size(); p++){
            parents.push_back(data[_ordered[i]._in[p]]);
        }

        c_probs = get_compact_probs(_ordered[i], parents);
        sumprob = 0.0; 
        node_probs = std::get<0>(c_probs);
        max = std::get<1>(c_probs);

        distribution = std::uniform_real_distribution<double>(min, max);
        threshold = distribution(_generator);

        if(_scenario.getParamHeader().getDomain(_ordered[i]._id).size() != node_probs.size()){
            std::cout << "[DEBUG] _ordered.size(): " << _scenario.getParamHeader().getDomain(_ordered[i]._id).size() << std::endl;
            std::cout << "[DEBUG] node_probs.size(): " << node_probs.size() << std::endl;
        }

        for(k = 0; k < node_probs.size(); k++){
            
            sumprob = sumprob + node_probs[k];
            new_value = k;
            if(threshold <= sumprob){
                break;
            }

            // if(k == node_probs.size()){
            //     new_value = node_probs.size();
            //     break;
            // }

        }

        //index = _node[i]._id;

        //node_probs.clear();

        // std::cout << std::endl;
        // std::cout << "[DEBUG] k: " << new_value << std::endl;
        // std::cout << "[DEBUG] _ordered[i]._id: " << _ordered[i]._id << ", _ordered[i]._in.size:" << _ordered[i]._in.size() << std::endl;
        // std::cout << "[DEBUG] _node[i]._id: " << _node[i]._id << ", _node[i]._in.size:" << _node[i]._in.size() << std::endl;
        // std::cout << "[DEBUG] _ordered.size(): " << _scenario.getParamHeader().getDomain(_ordered[i]._id).size() << std::endl;
        // std::cout << "[DEBUG] _node.size(): " << _scenario.getParamHeader().getDomain(_node[i]._id).size() << std::endl;

        // std::cout << "[DEBUG] node_probs: ";
        // for(size_t m = 0; m < node_probs.size(); m++){
        //     std::cout << node_probs[m] << ", ";
            
        // }
        // std::cout << std::endl;

        //data[_node[i]._id] = new_value;
        data[_ordered[i]._id] = new_value;


        // if(_ordered[i]._id == 53){
        //     std::cout << "[DEBUG]" << std::endl;
        // }
    }

    // std::cout << "[DEBUG] Data: ";
    // for(size_t m = 0; m < data.size(); m++){
    //     std::cout << "[DEBUG] " << m << ": " << data[m] << std::endl;
    // }
    // std::cout << std::endl;

    //result = new Individual(data, 0.0);
    result = new Individual(data, _scenario._cost(data));
    //result = new Individual(data, 1000.0);
    //data.resize(0);
    //data.clear();

    return result;

}

std::vector<double> BayesianNetwork::calculate_probability(const Node &node){

    std::vector<double> prob;

    if(node._in.size() <= 0){
        prob = marginal_probability(node._id);
    } else {
        //size_t domain_size_node_id = _scenario.getParamHeader().getDomain(node._id).size();

        std::vector<size_t> aux;

        for(size_t i = 0; i < node._in.size(); i++){
            aux.push_back(node._in[i]);
        }
        aux.push_back(node._id); // precisa ser o último

        std::vector<size_t> counts = compute_count_for_edges(aux);

        for(size_t i = 0; i < counts.size(); i++){
            counts[i]++; //Adiciona +1 para as ocorrências que forem zero
        }

        for(size_t i = 0; i < counts.size(); i++){
            prob.push_back(0.0);
        }

        double adjusted_domain = static_cast<double>(_pop.size()) + static_cast<double>(counts.size());
        for(size_t j = 0; j < counts.size(); j++){
            prob[j] = static_cast<double>(counts[j]) / adjusted_domain;
        }

        counts.clear();
        aux.clear();
    }

    return prob;
    
}

std::vector<double> BayesianNetwork::marginal_probability(const size_t &id_node)
{
    size_t tp = _scenario.getParamHeader().getDomain(id_node).size();
    uint32_t v;

    std::vector<double> prob;

    for(size_t p = 0; p < tp; p++){
        prob.push_back(1.0);
    }

    for(size_t p = 0; p < _pop.size(); p++){
        v = _pop.get_indinvidual(p).get_data()[id_node];
        prob[v]++;
    }

    //double adjusted_size = static_cast<double>(_pop.size());
    double adjusted_size = static_cast<double>(_pop.size()) + static_cast<double>(prob.size());

    for(size_t j = 0; j < tp; j++){
        prob[j] = prob[j] / adjusted_size;
    }

    return prob;
}

const double BayesianNetwork::fact(const double &v)
{
    if(v < 2.0)
        return 1.0;
    
    double val = 1.0;
    for (double i = 2.0; i <= v; i++) {
        val = val * i;
    }
    return val;
}

const std::tuple<std::vector<double>, double> BayesianNetwork::get_compact_probs(const Node &node, const std::vector<size_t> &parents){
    std::tuple<std::vector<double>, double> result;

    // std::vector<size_t> aux;
    // aux.push_back(node._id);

    // for (size_t n = 0; n < node._in.size(); n++) {
    //     aux.push_back(node._in[n]);
    // }

    std::vector<size_t> domain_size;
    for (int n = node._in.size() - 1; n >= 0; n--) {
        domain_size.push_back(_scenario.getParamHeader().getDomain(node._in[n]).size());
    }
    //domain_size.push_back(_scenario.getParamHeader().getDomain(node._id).size());


    size_t index = 0;

    //Calcula indíce 
    if(parents.size() > 0) {
        size_t it = domain_size.size() - 1;
        size_t local_domain = 1;

        for(int i = it - 1; i >= 0; i--){

            //v = parents[i];
            local_domain = 1;
            
            for (int n = it; n > i; n--) {
                local_domain *= domain_size[n];
            }
            index = index + parents[i] * local_domain;

        }
    }

    //Domínio de node
    size_t node_domain_size = _scenario.getParamHeader().getDomain(node._id).size();
    double total_prob = 0.0;
    std::vector<double> c_probs;
    for (size_t n = 0; n < node_domain_size; n++) {
        c_probs.push_back(_probs[node._id].at(n + index));
        total_prob += _probs[node._id].at(n + index);
    }

    std::get<0>(result) = c_probs;
    std::get<1>(result) = total_prob;

    return result;
}

BayesianNetwork::~BayesianNetwork(){
    
}