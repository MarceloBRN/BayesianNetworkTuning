#include <ctime>
#include "individual.h"

Individual::Individual()
{
    individual = nullptr;
    this->cost = -1.0;
}

Individual::Individual(const Scenario &scenario)
{
    individual = new int[scenario.getParamHeader().getNumParam()];

    for (int i = 0; i < scenario.getParamHeader().getNumParam(); i++) {
        individual[i] = rand() % scenario.getParamHeader().getDomain(i).size();

        //std::cout << individual[i]  << ", ";
    }
    //std::cout << "\n";
    this->scenario = scenario;

    //this->cost = calc_fitness(individual);
}

double Individual::calc_fitness(int * &vector)
{

    double fitness = 0.0;

    std::string command;

    //ParamReader param = this->scenario.getParamHeader();

    int t, i;

    for (t = 0; t < this->scenario.getParamHeader().getNumParam(); t++) {
        this->scenario.getParamHeader().setParamDefault(
            this->scenario.getParamHeader().getParms().at(t), 
            this->scenario.getParamHeader().getDomain(t).at(vector[t]));
    }

    double val = 0.0;

    std::cout << "New Incumbent: ";
    std::cout << " " << static_cast<double>(0.0) / 1000.0;
    std::cout << ", " << 0;
    //std::cout << " [" << iter << ", " << scenario.getCutOffTime() << "]";
    std::cout << std::endl;
    std::cout << "With state ";
    for (int i = 0; i < this->scenario.getParamHeader().getNumParam() - 1; i++) {
        //std::string p = this->scenario.getParamHeader().getParms().at(i);
        std::cout << this->scenario.getParamHeader().getParms().at(i) << "=" << this->scenario.getParamHeader().getDomain(i).at(vector[i]) << ", ";
    }
    std::cout << this->scenario.getParamHeader().getParms().at(this->scenario.getParamHeader().getNumParam() - 1) << "=" << this->scenario.getParamHeader().getDomain(this->scenario.getParamHeader().getNumParam() - 1).at(vector[this->scenario.getParamHeader().getNumParam() - 1]) << std::endl;

    std::ifstream temp;
    std::pair<double, unsigned> pair;
    unsigned index = 0;
    clock_t begin = 0, end = 0;

    for (t = 0; t < this->scenario.getTrainInstance().size(); t++) {
        command = this->scenario.getProgramName() + " " + this->scenario.getTrainInstance().at(t) + " ";

        for (i = 0; i < this->scenario.getParamHeader().getNumParam() - 1; i++) {
            command += this->scenario.getParamHeader().getDomain(i).at(vector[i]) + " ";
        }
        command += this->scenario.getParamHeader().getDomain(this->scenario.getParamHeader().getNumParam() - 1).at(vector[this->scenario.getParamHeader().getNumParam() - 1]);

        std::cout << "	Trial 1 for calling: " << command << std::endl;
        std::string tempname = "temp.txt";
        command += " > ";
        command += tempname;
        std::cout << "Executing cmd: " << command << std::endl;

        begin = clock();

#if defined(_WIN32) ||  defined(_WIN64)
        system(command.c_str());
#endif

        end = clock();

        temp.open(tempname);
        if (!temp.is_open()) {
            std::cout << "Error: File " << tempname << " not found!" << std::endl;
            exit(-1);
        }

        //temp >> val;

        double bins = 0.0;
        temp >> bins;
        //std::cout << "fitness: " << val << std::endl;

        double runtime_time = static_cast<double>(end - begin) / 1000.0;

        double val = (bins - this->scenario.getSolutionsTrainInstance().at(t)) * (bins - this->scenario.getSolutionsTrainInstance().at(t)) * runtime_time;
        //double val = (bins / this->scenario.getSolutionsTrainInstance().at(t)) - 1.0;
        //double val = (1.0 / bins);
        fitness += val;

        //index++;
        //pair.first = val;
        //pair.second = index;
        
        std::cout << "fitness: " << val << std::endl;
        std::cout << "runtime_time: " << runtime_time << std::endl;
        //temp >> val;
        //std::cout << "bins: " << static_cast<int>(bins) << "(" << static_cast<int>(this->scenario.getSolutionsTrainInstance().at(t)) << ")" << std::endl;

        temp.close();
    }

    fitness = fitness / static_cast<double>(scenario.getTrainInstance().size());
    std::cout << "----------------------------------------------\n";
    std::cout << "fitness global: " << fitness << std::endl;

    return fitness;
}
