#include "boa.h"
#include <ctime>
#include <string>

ofstream arq("resultado.txt");

const double fat[] = {
    1.0,
    1,0,
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

Boa::Boa()
{
}

Boa::Boa(const Scenario &scenario, const BOAParameter &boaparam)
{
    this->scenario = scenario;
    this->boaparam = boaparam;
    cout << "Construindo populacao..."<< endl;
    pop = new Population(this->boaparam.pop_size, this->scenario);
    graphT = new BayesianNetwork(scenario.getParamHeader().getNumParam());

}

void Boa::search()
{
    BayesianNetwork *graph = nullptr;

    vector<int *> children;

    Individual aux(scenario);

    int arcs;

    cout << "Ordenando populacao..." << endl;

    const clock_t begin = clock();

    //imprimePopulation(*pop);
    //cout << endl;

    pop->sort();

    //imprimePopulation(*pop);
    //cout << endl;

    bool converged = false;

    best = &(pop->ind.at(0));

    cout << "Carregando..." << endl;

    for(int it = 0; it < boaparam.max_iter; it++){

        pop->first(boaparam.select_size);
        pop->pop_size = boaparam.select_size;

        construct_network(graph, *pop, scenario.getParamHeader().getNumParam());

        arcs = graph->arcs();

        sample_from_network(children, *pop, *graph, boaparam.num_children);

        for(int c = 0; c < boaparam.num_children; c++){
            aux.individual = children[c];
            aux.cost = aux.calc_fitness(children[c]);
            pop->ind.push_back(aux);
            pop->pop_size++;
        }

        pop->sort();

//        imprimePopulation(pop);
//        arq << endl;

        pop->first(boaparam.select_size);
        pop->pop_size = boaparam.select_size;

        const clock_t end = clock();

        best = &(pop->ind.at(0));


        unsigned int currenttime = static_cast<unsigned int>(end - begin) / 1000.0;
        printf("currenttime = %u\n", currenttime);

        cout << ">it=" << it+1 << ", arcs=" << arcs << ", f=" << best->cost << ", s=[";
        int j = 0;
        for(j = 0; j< scenario.getParamHeader().getNumParam() - 1; j++){
           cout << this->scenario.getParamHeader().getParms().at(j) << "=" << this->scenario.getParamHeader().getDomain(j).at(best->individual[j]) << ", ";
        }
        cout << this->scenario.getParamHeader().getParms().at(j) << "=" << this->scenario.getParamHeader().getDomain(j).at(best->individual[j]);
        cout << "]\n" << endl;

        //if(pop->ind[0].cost == pop->ind[pop->ind.size()-1].cost){
        //    break;
        //}

        if (currenttime >= scenario.getTunerTimeout()) {
            break;
        }
    }

    if (graph) {
        imprimeGraph(*graph);
    }
    else {
        std::cout << "Graph is NULL!!!" << std::endl;
    }
    

    cout << endl;

    imprimePopulation(*pop);

    cout << endl;
}

void Boa::construct_network(BayesianNetwork * &graph, const Population &pop, int num_bits)
{
    int max_edges = 3*pop.pop_size;
    graph = new BayesianNetwork(num_bits);
    vector<double*> gains;
    gains.resize(0);
    double v;
    

//    imprimePopulation(pop);
//    arq << endl;
    for(int g = 0; g < max_edges; g++){
        double max = -1.0f;
        int from = -1;
        int to = -1;

        for(int i = 0; i < num_bits; i++){

            double *aux = new double[graph->size];
            for (int x = 0; x < graph->size; x++) {
                aux[x] = -1.0f;
            }

            compute_gains(aux, graph->node[i], *graph, pop);
            
            gains.push_back(aux);
            for(int j = 0; j < num_bits; j++){
                v = gains[i][j];
                if(v > max){
                    from = i;
                    to = j;
                    max = v;
                } 
            }
//            arq << "gain " << i << ": ";
//            imprimeVectorDoubleComum(gains.at(i), param->numeroNavios);
//            arq << endl;
            delete[] aux;
        }
        if (max <= 0.0f) break;
        graph->node[from].out.push_back(to);
        graph->node[to].in.push_back(from);
//        arq << "to: " << to << ", from: " << from << ", max: " << max << endl;
//        arq << endl;
        gains.clear();
    }
//    imprimeGraph(graph);

    //free(aux);
    //gains.clear();
    //return graph;
}

void Boa::compute_gains(double * & gains, const Node &node, const BayesianNetwork &graph, const Population &pop)
{
    vector<int> viable;
    unsigned int max = 2;

    //gains = new double[graph.size];

//    imprimePopulation(pop);

    get_viable_parents(viable, node.index, graph);

    //arq << node.index << " >>> in: ";
    //imprimeVectorInt(node.in);
    //arq << ",   out: ";
    //imprimeVectorInt(node.out);
    //arq << endl;

    //arq << "      viable: ";
    //imprimeVectorInt(viable);
    //arq << endl;

    for (int i = 0; i < graph.size; i++){
        if(graph.node[i].in.size() < max && include(viable, i)){
            gains[i] = k2equation(node.index, node.in, i, pop);
            //gains[i] = 1.0f;
            //arq << "gains[" << i << "]: " << gains[i] << endl;
        }
        else {
            gains[i] = -1.0f;
            //arq << "gains[" << i << "]: " << gains[i] << endl;
        }
    }
    //imprimeVectorDoubleComum(gains, scenario.getParamHeader().getNumParam());
    //arq << endl;

    viable.clear();
    //return gains;

}


void Boa::get_viable_parents(vector<int> &viable, const int &index_node, const BayesianNetwork &graph)
{
    //vector<int> viable;

    for (int i = 0; i < graph.size; i++){
        if(index_node!=i && can_add_edge(index_node, i, graph)){
            viable.push_back(i);
        }
    }
    //return viable;
}

bool Boa::can_add_edge(const int &i, const int &j, const BayesianNetwork &graph)
{
    return (!(include(graph.node[i].out, j)) && !(path_exists(j, i, graph)));
}

bool Boa::include(const vector<int> &v, const int &val) noexcept
{
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

double Boa::k2equation(const int &index_node, const vector<int> &candidates, const int &index_candidate, const Population &pop)
{
    vector<int> cand;
    int *a = new int[scenario.getParamHeader().getDomain(index_node).size()];
    double total = 1.0f, val;
    bool flag = false;

//    imprimePopulation(pop);

    cand.push_back(index_node);

    for(unsigned int i = 0; i < candidates.size(); i++){
        cand.push_back(candidates.at(i));
    }
    cand.push_back(index_candidate);


    //arq << "node: " << index_node << ", candidates: ";
    //imprimeVectorInt(cand);
    //arq << endl;
    int *counts;
    compute_count_for_edges(counts, pop, cand);

    val = 1;
    for (int n = 0; n < cand.size(); n++) {
        if (cand[n] != index_node) {
            val *= scenario.getParamHeader().getDomain(cand[n]).size();
        }
    }

    //arq << ">>> val: " << val << "\n";

    //arq << "counts: ";
    //imprimeVectorIntComum(counts, val * scenario.getParamHeader().getDomain(index_node).size());
    //arq << endl;

    double rs;
    int sum = 0;

    //val = static_cast<int>((power(scenario.getParamHeader().getDomain(index_node).size(),cand.size())/ scenario.getParamHeader().getDomain(index_node).size()));

    for(int i = 0; i < val; i++){

        for(int j = 0; j < scenario.getParamHeader().getDomain(index_node).size(); j++){
            //a[j] = 0;
            a[j] = counts[(i*scenario.getParamHeader().getDomain(index_node).size())+j];
        }

        //arq << "a: ";
        //imprimeVectorIntComum(a, scenario.getParamHeader().getDomain(index_node).size());
        //arq << endl;

        sum = 0;

        for(int j = 0; j < scenario.getParamHeader().getDomain(index_node).size(); j++){
            sum = sum + a[j];
        }
        //arq << "sum: " << sum << "\n";

        //rs = static_cast<double>(scenario.getParamHeader().getDomain(index_node).size() - 1)/ fact(static_cast<double>(sum + scenario.getParamHeader().getDomain(index_node).size() - 1));
        rs = static_cast<double>(scenario.getParamHeader().getDomain(index_node).size() - 1) / fat[sum + scenario.getParamHeader().getDomain(index_node).size() - 1];
////        arq << "rsini: " << rs << endl;

        for(int l = 0; l < scenario.getParamHeader().getDomain(index_node).size(); l++){
            //rs = rs* fact(static_cast<double>(a[l]));
            rs = rs * fat[a[l]];
        }

        //arq << "rs: " << rs << endl;

        if(flag == false){
            total = rs;
            flag = true;
        }else{
            total = total*rs;
        }
    }

    //arq << "total: " << total << endl;

    //if (a != NULL) delete[] a;
    //arq << ">>(1) total: " << total << endl;
    //if (counts != NULL) delete[] counts;
    //free(counts);
    //arq << ">>(2) total: " << total << endl;
    //free(a);
    
    return total;
}


bool Boa::path_exists(const int &i, const int &j, const BayesianNetwork &graph) noexcept
{
    vector<int> visited;
    vector<int> stacki;

    visited.resize(0);
    stacki.resize(0);
    //arq << endl;
    //arq << "i: " << i << ", j: " << j << endl;
    //imprimeGraph(graph);

    bool flag = false;

    int k = -1;
    stacki.push_back(i);

    //arq << "visited: ";
    //imprimeVectorInt(visited);
    //arq << ", stacki: ";
    //imprimeVectorInt(stacki);
    //arq << endl;

    while(!(stacki.empty())){
        //arq << "1  visited: ";
        //imprimeVectorInt(visited);
        //arq << ", stacki: ";
        //imprimeVectorInt(stacki);
        //arq << endl;

        if(include(stacki, j)){
            flag =  true;
            break;
        }

        k = stacki.back();
        stacki.pop_back();

        //arq << "2    visited: ";
        //imprimeVectorInt(visited);
        //arq << ", stacki: ";
        //imprimeVectorInt(stacki);
        //arq << endl;

        //cout << stacki.size() << endl;

        if(!include(visited, k)){
            visited.push_back(k);

            //arq << "3      visited: ";
            //imprimeVectorInt(visited);
            //arq << ", stacki: ";
            //imprimeVectorInt(stacki);
            //arq << endl;

            for (int m = 0; m < graph.node[k].out.size(); m++){
                if(!include(visited, graph.node[k].out[m])){
                    stacki.push_back(graph.node[k].out[m]);

                    //arq << "4        visited: ";
                    //imprimeVectorInt(visited);
                    //arq << ", stacki: ";
                    //imprimeVectorInt(stacki);
                    //arq << endl;
                }
            }
        }

        //arq << "5          visited: ";
        //imprimeVectorInt(visited);
        //arq << ", stacki: ";
        //imprimeVectorInt(stacki);
        //arq << endl;

    }
    //visited.clear();
    //stacki.clear();
    return flag;
}


void Boa::compute_count_for_edges(int *&counts, const Population &pop, const vector<int> &indexes)
{
    int valor = 1;
    for (int n = 0; n < indexes.size(); n++) {
        valor *= scenario.getParamHeader().getDomain(indexes[n]).size();
    }

    //arq << "    valor = " << valor << "\n";
    counts = new int[valor];
    for(int c = 0; c < valor; c++){
        counts[c] = 0;
    }

    //imprimePopulation(pop);
    //arq << endl;

    //arq << "indexes: ";
    //imprimeVectorInt(indexes);
    //arq << endl;

    int index;
    int v;

    for(int p = 0; p < pop.pop_size; p++){
        index = 0;
//        imprimeIndividual(pop->ind[p]);
//        arq << endl;

        int it = indexes.size()-1;

        v = indexes[it];
        index = index + pop.ind[p].individual[v];
        //arq << "v: " << v << " [" << pop.ind[p].individual[v] << "], index: " << index << " (" << 0 << ")" << endl;

        for(int i = it - 1; i >= 0; i--){
            v = indexes[i];
            valor = 1;
            for (int n = it; n > i; n--) {
                valor *= scenario.getParamHeader().getDomain(indexes[n]).size();
            }
            index = index + (pop.ind[p].individual[v])*valor;
            //arq << "v: " << v << " [" << pop.ind[p].individual[v] << "], index: " << index << " (" << valor << ")" << endl;
            
        }
        counts[index]++;

        //valor = 1;
        //for (int n = 0; n < indexes.size(); n++) {
        //    valor *= scenario.getParamHeader().getDomain(indexes[n]).size();
        //}
        //int j;
        //arq << "counts = [";
        //for (j = 0; j < valor - 1; j++) {
        //    arq << counts[j] << ", ";
        //}
        //arq << counts[j] << "]\n";
        //arq << endl;
    }

    //return counts;
}

void Boa::topological_ordering(BayesianNetwork * &ordered, const BayesianNetwork &graph)
{
    int *count = new int[graph.size];
    int edge;

    for(int i = 0; i < graph.size; i++){
        count[i] = static_cast<int>(graph.node[i].in.size());
    }

//    imprimeGraph(graph);
//    arq << endl;

    ordered = new BayesianNetwork(scenario.getParamHeader().getNumParam());

    vector<int> vordered;
    Node current;
    queue<Node> stacki;

    vordered.clear();

    for(int i = 0; i < graph.size; i++){
        if(count[i] == 0){
            stacki.push(graph.node[i]);
        }
    }

//    arq << "ordered: ";
//    imprimeVectorInt(vordered);
//    arq << ", stack: ";
//    imprimeQueueNode(stacki);
//    arq << endl;

    Node node;
    int n;

    while(vordered.size() < graph.size){
        current = stacki.front();
        stacki.pop();

        for(unsigned int j = 0; j < current.out.size(); j++){
            edge = current.out[j];

            for(n = 0; n < graph.size; n++){
                if(graph.node[n].index == edge){
                    node = graph.node[n];
                    break;

                }
            }
            count[n]--;
            if(count[n] <= 0){
                stacki.push(node);
            }
        }
        vordered.push_back(current.index);
//        arq << "ordered: ";
//        imprimeVectorInt(vordered);
//        arq << ", stack: ";
//        imprimeQueueNode(stacki);
//        arq << endl;
    }
//    arq << endl;
//    arq << "orderedfinal: " << endl;
//    imprimeVectorInt(ordered);

    for(int i = 0; i < graph.size; i++){
       ordered->node[i].index = vordered[i];
       ordered->node[i].out = graph.node[vordered[i]].out;
       ordered->node[i].in = graph.node[vordered[i]].in;
    }

//    imprimeGraph(ordered);

    free(count);
    //vordered.clear();
    //return ordered;
}


void Boa::marginal_probability(double * &prob, const int &index_node, const Population &pop)
{
    prob = new double[scenario.getParamHeader().getDomain(index_node).size()];
    int v;

//    arq << "index: " << index_node << endl;
//    imprimePopulation(pop);

    for(int p = 0; p < pop.pop_size; p++){
        v = pop.ind[p].individual[index_node]-1;
        prob[v]++;
    }

//    imprimeVectorDoubleComum(prob, param->numeroBercos);
//    arq << endl;

    for(int j = 0; j < scenario.getParamHeader().getDomain(index_node).size(); j++){
        prob[j] = prob[j]/((double)(pop.pop_size));
    }

//    imprimeVectorDoubleComum(prob, param->numeroBercos);
//    arq << endl;

    //return prob;

}


void Boa::calculate_probability(double * &prob, const Node &node, int *&numstring, const BayesianNetwork &graph, const Population &pop)
{
    vector<int> aux;
    if(node.in.empty()){
        marginal_probability(prob, node.index, pop);
        return;
    }

    aux.push_back(node.index);

    for(unsigned int i = 0; i < node.in.size(); i++){
        aux.push_back(node.in[i]);
    }

    //imprimePopulation(pop);
    //arq << endl;

    //imprimeGraph(graph);
    //arq << endl;

    //arq << "node: " << node.index << endl;
    //arq << "numstring: ";
    //imprimeVectorIntComum(numstring, scenario.getParamHeader().getNumParam());
    //arq << endl;

    int *counts;
    compute_count_for_edges(counts, pop, aux);

    int val = 1;
    for (int n = 0; n < node.in.size(); n++) {
        if (node.in[n] != node.index) {
            val *= scenario.getParamHeader().getDomain(node.in[n]).size();
        }
    }

    //arq << "counts: ";
    //imprimeVectorIntComum(counts, val * scenario.getParamHeader().getDomain(node.index).size());
    //arq << endl;

    int index = 0;

    int it = node.in.size() - 1;

    //arq << "node[:in]: ";
    //imprimeVectorInt(node.in);
    //arq << endl;

    index = index + numstring[node.in[0]];
    
    int valor;

    for(int i = it; i >= 0; i--){
        valor = 1;
        for (int n = it; n > i; n--) {
            valor *= scenario.getParamHeader().getDomain(node.in[i]).size();
        }
        index += numstring[node.in[i]] * valor;
        //arq << "    " << index << endl;
    }

    //arq << "index: " << index << endl;

    int *il = new int[scenario.getParamHeader().getDomain(node.index).size()];
    for(int i = 0; i < scenario.getParamHeader().getDomain(node.index).size(); i++){
        il[i] = 0;
    }

    //arq << power(scenario.getParamHeader().getDomain(node.index).size(), node.in.size()) << endl;

    valor = 1;
    for (int n = 0; n < node.in.size(); n++) {
        if (node.in[n] != node.index) {
            valor *= scenario.getParamHeader().getDomain(node.in[n]).size();
        }
    }
    for(int j = 0; j < scenario.getParamHeader().getDomain(node.index).size(); j++){
        il[j] = index + j * valor;
    }

    //arq << "il: ";
    //imprimeVectorIntComum(il, scenario.getParamHeader().getDomain(node.index).size());
    //arq << endl;

    double *a = new double[scenario.getParamHeader().getDomain(node.index).size()];
    for(int i = 0; i < scenario.getParamHeader().getDomain(node.index).size(); i++){
        a[i] = 0.0f;
    }

    int aux2;
    for(int j = 0; j < scenario.getParamHeader().getDomain(node.index).size(); j++){
        aux2 = il[j];
        a[j] = (double)(counts[aux2]);
    }

    //arq << "a: ";
    //imprimeVectorDoubleComum(a, scenario.getParamHeader().getDomain(node.index).size());
    //arq << endl;

    double sum = 0.0f;

    for(int i = 0; i < scenario.getParamHeader().getDomain(node.index).size(); i++){
        sum = sum + a[i];
    }

    //arq << "sum: " << sum << endl;

    prob = new double[scenario.getParamHeader().getDomain(node.index).size()];
    for(int i = 0; i < scenario.getParamHeader().getDomain(node.index).size(); i++){
        prob[i] = 0.0f;
    }

    for(int j = 0; j < scenario.getParamHeader().getDomain(node.index).size(); j++){
        prob[j] = a[j]/sum;
    }

    double ct = 0;
    for(int i = 0; i < scenario.getParamHeader().getDomain(node.index).size(); i++){
        if(prob[i] <= 0.0){
            ct++;
        }
    }

    ct = ct*0.001f/((double)scenario.getParamHeader().getDomain(node.index).size() - ct);

    for(int i = 0; i < scenario.getParamHeader().getDomain(node.index).size(); i++){
        if(prob[i] > 0.0){
            prob[i] -= ct;
        }else{
            prob[i] = 0.001;
        }
    }

    //arq << "prob: ";
    //imprimeVectorDoubleComum(prob, scenario.getParamHeader().getDomain(node.index).size());
    //arq << endl;

    free(counts);
    aux.clear();
    //return prob;

}


void Boa::probabilistic_logic_sample(int * &numstring, const BayesianNetwork &graph, const Population &pop)
{
    numstring = new int[graph.size];
    double *prob;


    int index = -1;
    double sumprob = 0.0;
    double val;

    //imprimePopulation(pop);
    //arq << endl;

    //imprimeGraph(graph);
    //arq << endl;

    for(int j = 0; j < graph.size; j++){
        numstring[j] = -1;
    }

    for(int i = 0; i < graph.size; i++){

        //arq << "node[:num]: " << graph.node[i].index << endl;

        calculate_probability(prob, graph.node[i], numstring, graph, pop);

        /*arq << ">>prob: ";
        imprimeVectorDoubleComum(prob, scenario.getParamHeader().getDomain(graph.node[i].index).size());
        arq << endl;*/

        sumprob = 0.0;
        index = -1;
        val = (((double)((rand()%10000)+1))/10000.0f); //MAX_RAND é 7fff

        //arq << ">>val: " << val << endl;

        //arq << ">>n: " << scenario.getParamHeader().getDomain(graph.node[i].index).size() << endl;

        for(int k = 0; k < scenario.getParamHeader().getDomain(graph.node[i].index).size(); k++){
            index = k;
            sumprob = sumprob + prob[k];
            //arq << ">>sumprob: " << sumprob << endl;
            if(val <= sumprob){
                break;
            }
        }

        //arq << "index: " << index << endl;

        numstring[graph.node[i].index] = index;

        //arq << ">>numstring[" << graph.node[i].index << "]: " <<  numstring[graph.node[i].index] << endl;
        //arq << endl;
    }

    //imprimeVectorIntComum(numstring, scenario.getParamHeader().getNumParam());

    //return numstring;
}

void Boa::sample_from_network(vector<int *> &samples, const Population &pop, const BayesianNetwork &graph, const int &num_samples)
{
//    imprimePopulation(pop);
//    arq << endl;

//    imprimeGraph(graph);
//    arq << endl;

    BayesianNetwork *ordered;
    topological_ordering(ordered, graph);

//    imprimeGraph(ordered);
//    arq << endl;


    for(int i = 0; i < num_samples; i++){
        int *numstring;
        probabilistic_logic_sample(numstring, *ordered, pop);
        samples.push_back(numstring);
        //imprimeVectorIntComum(samples[i], scenario.getParamHeader().getNumParam());
        //arq << endl;
    }

    free(ordered);

    //return samples;
}

double Boa::fact(double v)
{
//    cout << "           teste16" << endl;
    if(v < 2.0)
        return 1.0;
    double val = 1.0;
    for (double i = 2.0; i <= v; i++) {
        val = val * i;
    }
    return val;
}

int Boa::power(int a, int b)
{
    int val = 1;
    for(int i = 0; i < b; i++){
        val = val*a;
    }
    return val;
}

void Boa::imprimeGraph(const BayesianNetwork &graph)
{
    int i, j;
    for(i = 0; i < graph.size; i++){
        arq << ":num=> " << graph.node[i].index << ", :out=>[";
        if(!graph.node[i].out.empty()){
            for(j = 0; j < (graph.node[i].out.size()-1); j++){
                arq << graph.node[i].out[j] << ", ";
            }
            arq << graph.node[i].out[j];
        }
        arq << "], ";

        arq << ":in=>[";
        if(!graph.node[i].in.empty()){
            for(j = 0; j < (graph.node[i].in.size()-1); j++){
                arq << graph.node[i].in[j] << ", ";
            }
            arq << graph.node[i].in[j];
        }
        arq << "]" << endl;
    }
}

void Boa::imprimeVectorInt(vector<int> v)
{
    int i;
    arq << "[";
    if(!v.empty()){
        for(i = 0; i < v.size() - 1;i++){
            arq << v[i] << ", ";
        }
        arq << v[i] << "]";
    }else{
        arq << "]";
    }
}

void Boa::imprimeVectorNode(vector<Node> v)
{
    int i;
    arq << "[";
    if(!v.empty()){
        for(i = 0; i < v.size() - 1;i++){
            arq << v[i].index << ", ";
        }
        arq << v[i].index << "]";
    }else{
        arq << "]";
    }
}

void Boa::imprimeQueueNode(queue<Node> v)
{
    queue<Node> aux = v;
    int i;
    arq << "[";
    if(!aux.empty()){
        for(i = 0; i < aux.size() - 1;i++){
            arq << aux.front().index << ", ";
            aux.pop();
        }
        arq << aux.front().index << "]";
        aux.pop();
    }else{
        arq << "]";
    }
}

void Boa::imprimePopulation(const Population &pop)
{
    for(int i = 0; i < pop.pop_size; i++){
        imprimeIndividual(pop.ind[i]);
        arq << endl;
    }
}

void Boa::imprimeIndividual(Individual ind)
{
    int i;
    arq << "[";
    for(i = 0; i < scenario.getParamHeader().getNumParam() - 1; i++){
        arq << ind.individual[i] << ", ";
    }
    arq << ind.individual[i] << "]";
    arq << " => " << ind.cost;
}

void Boa::imprimeVectorIntComum(int * &v, const int &length)
{
    int i;
    arq << "[";
    if(length > 0){
        for(i = 0; i < length - 1;i++){
            arq << v[i] << ", ";
        }
        arq << v[i] << "]";
    }else{
        arq << "]";
    }
}

void Boa::imprimeVectorDoubleComum(double *v, int length)
{
    int i;
    arq << "[";
    if(length > 0){
        for(i = 0; i < length - 1;i++){
            arq << v[i] << ", ";
        }
        arq << v[i] << "]";
    }else{
        arq << "]";
    }
}

