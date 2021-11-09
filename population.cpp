#include "population.h"
#include <chrono>

Population::Population()
{
    _ind.resize(0);
}

Population::Population(const Scenario &scenario, const int &num_jobs)
{
    //_pop_size = static_cast<size_t>(std::ceil(scenario.getParamHeader().getNumParam() / static_cast<double>(num_jobs)));
    _scenario = scenario;
    
    size_t pop_size = scenario.getParamHeader().getNumParam();
    // double *runtime_time = new double[pop_size];
    // for(size_t i = 0; i < pop_size; i++){
    //     runtime_time[i] = 0.0;
    // }

    auto start = chrono::steady_clock::now();
    omp_set_num_threads(num_jobs);
    #pragma omp parallel shared(runtime_time)
    {
        int thread_id = omp_get_thread_num();

        //#pragma omp for
        for (size_t i = static_cast<size_t>(thread_id); i < pop_size; i += num_jobs){
            // #pragma omp critical 
            // {
            //     std::cout << "There are " << omp_get_num_threads() << " threads" <<endl;
            // }
            std::cout << "[THREAD " << thread_id << ", IND: " << i << " -> START] " << std::endl;
            // auto start = chrono::steady_clock::now();
            Individual *aux = scenario.genData();
            // auto end = chrono::steady_clock::now();

            #pragma omp critical 
            {
                // runtime_time[thread_id] += chrono::duration<double> (end - start).count(); 
                // std::cout << "[THREAD " << thread_id << ", IND: " << i << "] " << ">>>>>>>>>>runtime_time: " << runtime_time[thread_id] << std::endl;
                // std::cout << "[THREAD " << thread_id << ", IND: " << i << "] " << ">>>>>>>>>>scenario.getTunerTimeout(): " << scenario.getTunerTimeout() << std::endl;
                _ind.push_back(*aux);  
            }
        }

        std::cout << "[THREAD " << thread_id << " FINISH]" << std::endl;
    }


    auto end = chrono::steady_clock::now();
    runtime_time += std::chrono::duration<double> (end - start).count(); 

    std::cout << "_pop_size: " << _ind.size()  << std::endl;


    if (_ind.size() < pop_size) {
        _incomplete_pop_size = _ind.size();
        //_pop_size = _ind.size();
    }else{
        _incomplete_pop_size = scenario.getParamHeader().getNumParam();
        //_pop_size = scenario.getParamHeader().getNumParam();
    }

    //delete[] runtime_time;
    
}

Population::Population(const Scenario &scenario, const size_t &pop_size, const int &num_jobs)
{
    //_pop_size = static_cast<size_t>(std::ceil(scenario.getParamHeader().getNumParam() / static_cast<double>(num_jobs)));

    _scenario = scenario;
    
    // double *runtime_time = new double[pop_size];
    // for(size_t i = 0; i < pop_size; i++){
    //     runtime_time[i] = 0.0;
    // }

    auto start = chrono::steady_clock::now();

    omp_set_num_threads(num_jobs);
    #pragma omp parallel shared(runtime_time)
    {
        int thread_id = omp_get_thread_num();

        //#pragma omp for
        for (size_t i = static_cast<size_t>(thread_id); i < pop_size; i+=num_jobs){
            // #pragma omp critical 
            // {
            //     std::cout << "There are " << omp_get_num_threads() << " threads" <<endl;
            // }

            std::cout << "[THREAD " << thread_id << ", CHILD: " << i << " -> START] " << std::endl;
            // auto start = chrono::steady_clock::now();
            Individual *aux = scenario.genData();
            // auto end = chrono::steady_clock::now();

            #pragma omp critical 
            {
                // runtime_time[thread_id] += chrono::duration<double> (end - start).count(); 
                // std::cout << "[THREAD " << thread_id << ", IND: " << i << "] " << ">>>>>>>>>>runtime_time: " << runtime_time[thread_id] << std::endl;
                // std::cout << "[THREAD " << thread_id << ", IND: " << i << "] " << ">>>>>>>>>>scenario.getTunerTimeout(): " << scenario.getTunerTimeout() << std::endl;
                _ind.push_back(*aux);  
            }
        }

        std::cout << "[THREAD " << thread_id << " FINISH]" << std::endl;
    }

    auto end = chrono::steady_clock::now();

    runtime_time += std::chrono::duration<double> (end - start).count(); 

    std::cout << "_pop_size: " << _ind.size()  << std::endl;

    if (_ind.size() < pop_size) {
        _incomplete_pop_size = _ind.size();
        //_pop_size = _ind.size();
    }else{
        _incomplete_pop_size = pop_size;
        //_pop_size = pop_size;
    }

    //delete[] runtime_time;
    
}

void Population::concat(const Population &&p)
{
    //_pop_size += p._pop_size;
    std::move(p._ind.begin(), p._ind.end(), std::back_inserter(_ind));
}

bool sort_func(const Individual &i, const Individual &j) {
    return (i.cost() < j.cost());
}

void Population::sort()
{
    std::sort(_ind.begin(), _ind.end(), sort_func);
}

void Population::swap(const size_t &x, const size_t &y)
{
    Individual aux;
    aux = _ind[x];
    _ind[x] = _ind[y];
    _ind[y] = aux;

}

const Individual Population::first() const
{
    return _ind.at(0);
}

void Population::print(std::ostream &output) const
{
    for(size_t i = 0; i < _ind.size(); i++){
        _ind[i].print(output);
        output << std::endl;
    }
}

void Population::print_best(std::ostream &output) const
{        
    size_t j = 0;
    output << "[";
    for(j = 0; j < _scenario.getParamHeader().getParms().size() - 1; j++){
        output << _scenario.getParamHeader().getParms().at(j);
        output << ": ";
        output << _scenario.getParamHeader().getDomain(j).at(this->first().get_data().at(j));
        output << ", ";
    }
    output << _scenario.getParamHeader().getParms().at(j);
    output << ": ";
    output << _scenario.getParamHeader().getDomain(j).at(this->first().get_data().at(j));
    output << ", ";
    output << " => " << this->first().cost();
    //output << std::endl;
    output << "]";
}

void Population::print_all(std::ostream &output) const
{
    size_t j = 0;
    output << "[";
    for(size_t i = 0; i < _ind.size(); i++){
        for(j = 0; j < _scenario.getParamHeader().getParms().size() - 1; j++){
            output << _scenario.getParamHeader().getParms().at(j);
            output << ": ";
            output << _scenario.getParamHeader().getDomain(j).at(_ind[i].get_data().at(j));
            output << ", ";
        }
        output << _scenario.getParamHeader().getParms().at(j);
        output << ": ";
        output << _scenario.getParamHeader().getDomain(j).at(_ind[i].get_data().at(j));
        output << ", ";
        output << " => " << _ind[i].cost();
        output << std::endl;
    }
    output << "]";
}

const size_t Population::size()
{
    return _ind.size();
}

const Scenario Population::get_scenario() const{
    return _scenario;
}

const Individual Population::get_indinvidual(const size_t &idx) const{
    return _ind[idx];
}

void Population::add_child(const Individual& child){
    _ind.push_back(child);
}

void Population::resize(const size_t& new_size) {
    if (new_size < _ind.size()){
        for(int i = 0; i < _ind.size() - new_size; i++){
            _ind.pop_back();
        }
    }
}

Population::~Population()
{
    //delete[] _data;
}
