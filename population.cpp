#include "population.h"

Population::Population(int pop_size, const Scenario &scenario)
{
    srand(static_cast<unsigned int>(time(NULL)));
    Individual *aux = nullptr;
    for (int i=0;i<pop_size;i++){
        aux = new Individual(scenario);
        aux->cost = aux->calc_fitness(aux->individual);
        ind.push_back(*aux);
    }

    this->pop_size = pop_size;
}

bool sort_func(Individual i, Individual j) {
    return (i.cost < j.cost);
}

void Population::sort()
{
    std::sort(ind.begin(), ind.end(), sort_func);
}

void Population::swap(vector<Individual> *v, int x, int y)
{
    Individual *id = (*v).data();
    Individual aux(scenario);

    aux = id[x];
    id[x] = id[y];
    id[y] = aux;

}

void Population::first(int size)
{
    for(int i = 0; i < pop_size - size; i++){
        ind.pop_back();
    }
}


