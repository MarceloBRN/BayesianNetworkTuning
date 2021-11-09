#include "boa.h"
#include <chrono>
#include <map>

Boa::Boa(const Scenario &scenario, const int &num_jobs)
{
    _scenario = scenario;
    std::cout << "Construindo populacao..." << std::endl;
    _num_jobs = num_jobs;
    _pop = new Population(_scenario, _num_jobs);
    _numparam = _scenario.getParamHeader().getNumParam();
    
    //_b_net = new BayesianNetwork(scenario.getParamHeader().getNumParam());
}

void Boa::run(){

    std::vector<size_t *> children;
    //Individual aux(_scenario);
    size_t arcs;
    bool converged = false;
    double current_time = 0.0;
    std::ofstream best_param("traj.csv");
    std::ofstream result_file("resultado.txt");
    double max_iter = _scenario.getParamHeader().getNumParam();

    std::cout << "Ordenando populacao..." << std::endl;

    current_time += _pop->runtime_time;

    auto start = std::chrono::steady_clock::now();

    _pop->sort();
    _best = _pop->first();

    std::cout << "Carregando..." << std::endl;
    best_param << "\"Total Time\",\"Mean Performance\",\"Wallclock Time\",\"Incumbent ID\",\"Automatic Configurator Time\",\"Configuration...\"\n";

    max_iter = 3;
    //boaparam.select_size = pop->pop_size;
    size_t old_pop_size = 0;

    for(size_t it = 0; it < max_iter; it++){
        old_pop_size = _pop->size();
        if (_graph) {
            delete _graph;
        }

        std::cout << "Constructing Bayesian Network ..." << std::endl;

        _graph = new BayesianNetwork(*_pop, _num_jobs);

        if (_graph) {
            _graph->print();
        }
        else {
            std::cout << "Graph is NULL!!!" << std::endl;
        }

        std::cout << "Arcs size: " << _graph->arcs() << std::endl;

        Population sample = *(_graph->sample_from_network(_pop->size()/ 2));

        std::cout << "Calculating Similarity ..." << std::endl;
        for (size_t s = 0; s < sample.size(); s++) {
            if(_calculate_similarity(_pop->first(), sample.get_indinvidual(s)) >= 0.9){
                 _pop->add_child(sample.get_indinvidual(s));
                 old_pop_size++;
            }
        }
        
        _pop->concat(std::move(sample));

        std::cout << "New Sorting: (it = " << it << ")" << std::endl;
        _pop->sort();

        _best = _pop->first();

        _pop->resize(old_pop_size);

        auto end = chrono::steady_clock::now();

        

        current_time += chrono::duration<double> (end - start).count();
        std::cout << ">>>>>>>>>>currenttime: " << current_time << std::endl;

        best_param << current_time <<", "<< 10000000 << ", 0.0003, 0, " << current_time << ", ";

        int j = 0;
        std::map<string, string> map_best;
        for(j = 0; j < _scenario.getParamHeader().getNumParam(); j++){
           map_best[this->_scenario.getParamHeader().getParms().at(j)] = this->_scenario.getParamHeader().getDomain(j).at(_best.get_data()[j]);
        }

        std::string buffer_param = "";
        for (std::map<string, string>::iterator i = map_best.begin(); i != map_best.end(); i++){
            buffer_param += (i->first + "='" + i->second + "', ");
        }
        buffer_param = buffer_param.substr(0, buffer_param.size() - 2);

        best_param << buffer_param;

        std::cout << ">it=" << it+1 << ", arcs=" << _graph->arcs() << ", f=" << _best.cost() << ", s=";
        _pop->print_best();

        std::cout << std::endl;

        if (current_time >= _scenario.getTunerTimeout()) {
            break;
        }

    }

    if (_graph) {
        _graph->print(result_file);
    }
    else {
        std::cout << "Graph is NULL!!!" << std::endl;
    }
    
    cout << endl;

    _pop->print(result_file);
    _pop->print_all(result_file);

    delete _graph;

    //imprimePopulation(*pop);

    //cout << endl;
    
}

double Boa::_calculate_similarity(const Individual& id1, const Individual& id2){
    double s = 0.0;
    for (size_t i = 0; i < id1.get_data().size(); i++) {
        if(id1.get_data().at(i) == id2.get_data().at(i)){
            s++;
        }
    }
    
    return (s / static_cast<double>(id1.get_data().size()));
}


