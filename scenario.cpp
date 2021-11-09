#include "scenario.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <cassert>
#include <sstream>
#include <chrono>
#include <set>

#define MAX_LINE 2048

Scenario::Scenario() {
}

Scenario::Scenario(const char* scenario) {

	char *cline = new char[MAX_LINE];
	std::ifstream file(scenario);
	if (!file.is_open()) {
		std::cout << "Error: File " << scenario << " not found!" << std::endl;
		exit(-1);
	}
	std::string sline;
	std::string instanceline;
	double solutionline;

	std::string paramindicator;
	std::string indicator;
	std::vector<std::string> splited_line;
    

	char sep = ',';

	while (!file.eof()) {
		file.getline(cline, MAX_LINE);
		sline = cline;
		paramindicator.clear();
		indicator.clear();

		size_t i = 0;
		while (isspace(sline[i]) && i < sline.size()) {
			i++;
		}

		while (!isspace(sline[i]) && sline[i] != '=' && i < sline.size()) {
			paramindicator += sline[i];
			i++;
		}
		
		i++;
		while ((isspace(sline[i]) || sline[i] == '=')  && i < sline.size()) {
			i++;
		}

		while (i < sline.size()) {
			indicator += sline[i];
			i++;
		}

		std::cout  << "paramindicator: " << paramindicator << std::endl;


		std::cout  << "indicator: " << indicator << std::endl;

		if (paramindicator == "program" || paramindicator == "algo") {
			this->_program = indicator;
		}
		else if (paramindicator == "deterministic") {
			this->_deterministic = std::stoi(indicator);
		}
		else if (paramindicator == "overall_obj") {
			this->_overall_obj = indicator;
		}
		else if (paramindicator == "cutoff_time") {
			this->_cutoff_time = std::stod(indicator);
		}
		else if (paramindicator == "tunerTimeout" || paramindicator == "wallclock_limit") {
			this->_tunerTimeout = std::stoul(indicator);
		}
		else if (paramindicator == "paramfile") {
			this->_paramfilename = indicator;
			this->_param = *(new ParamReader(this->_paramfilename.c_str()));
		}
		else if (paramindicator == "outdir") {
			this->_outdir = indicator;
		}
		else if (paramindicator == "train_instance_file" || paramindicator == "instance_file") {
			this->_train_instance_file = indicator;
			std::ifstream train(this->_train_instance_file);
            if (!train.fail()) {
				std::stringstream ss;
				std::string item;
                while (!train.eof()) {
                    train.getline(cline, MAX_LINE);
                    instanceline = cline;
					// splited_line = this->_split(cline, " ");
                    // this->_train_instances.push_back(splited_line[0]);
					if(instanceline != ""){
						this->_train_instances.push_back(instanceline);
					}
                    // //train.getline(cline, MAX_LINE);
                    // //solutionline = std::stod(cline);
                    // this->_solutions_train_instances.push_back(std::stod(splited_line[1]));
					this->_solutions_train_instances.push_back(1.0);
					// splited_line.clear();
                }
            }
            else {
                std::cout << "[ERROR]: Train Instance File not found!" << std::endl;
            }
            train.close();
			
		}
		else if (paramindicator == "test_instance_file") {
			this->_test_instance_file = indicator;
			std::ifstream test(this->_test_instance_file);
            if (!test.fail()) {
                while (!test.eof()) {
                    test.getline(cline, MAX_LINE);
                    instanceline = cline;
					// splited_line = this->_split(cline, " ");
                    // this->_test_instances.push_back(splited_line[0]);
					if(instanceline != ""){
						this->_test_instances.push_back(instanceline);
					}
                    // //test.getline(cline, MAX_LINE);
                    // //solutionline = std::stod(cline);
                    // this->_solutions_test_instances.push_back(std::stod(splited_line[1]));
					this->_solutions_test_instances.push_back(1.0);
					// splited_line.clear();
                }
            }
            else {
                std::cout << "[ERROR]: Test Instance File not found!" << std::endl;
            }
			test.close();
		}
		//else if (paramindicator == "args") {
		//	
		//	std::string vals = indicator;
		//	vals.erase(remove_if(vals.begin(), vals.end(), isspace), vals.end());
		//	for (size_t p = 0, q = 0; p != vals.npos; p = q) {
		//		this->_args.push_back(vals.substr(p + (p != 0), (q = vals.find(sep, p + 1)) - p - (p != 0)));
		//	}
		//}
		//else {
			//std::cout << "Error: Indicator " << paramindicator << " not recognize!" << std::endl;
			//exit(-1);
		//}
	}

	

	delete[] cline;
}


Scenario::~Scenario()
{
	//delete _param;
}

ParamReader Scenario::getParamHeader() const
{
	return _param;
}

double Scenario::getCutOffTime() const
{
	return _cutoff_time;
}

unsigned Scenario::getTunerTimeout() const
{
	return _tunerTimeout;
}

std::string Scenario::getOverallObj() const
{
	return _overall_obj;
}

std::string Scenario::getProgramName() const
{
	return _program;
}

std::vector<std::string> Scenario::getTrainInstance() const
{
	return _train_instances;
}

std::vector<std::string> Scenario::getTestInstance() const
{
	return _test_instances;
}

std::vector<double> Scenario::getSolutionsTrainInstance() const
{
	return _solutions_train_instances;
}

std::vector<double> Scenario::getSolutionsTestInstance() const {
	return _solutions_test_instances;
}

void Scenario::setParamHeader(const ParamReader & param)
{
    _param = param;
}

Individual * Scenario::genData() const{
	//uint32_t *data = new uint32_t[_param.getNumParam()];
    std::vector<size_t> data;

	uint32_t min = 0;
	uint32_t max = 0;
	std::uniform_int_distribution<uint32_t> distribution;
    Individual *result = nullptr;
	
    thread_local std::mt19937 _generator;
    uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
    //std::random_device rd;
	_generator.seed(seed);
	
    uint32_t v = 0;
    for (size_t i = 0; i < _param.getNumParam(); i++) {
		max = _param.getDomain(i).size() - 1;
        
		distribution = std::uniform_int_distribution<uint32_t>(min, max);
        v = distribution(_generator);
        data.push_back(v);
    }

    result = new Individual(data, this->_cost(data));
    data.resize(0);
    data.clear();

	return result;
}


double Scenario::_cost(const std::vector<size_t> &vector) const {

    double fitness = 0.0;

    int ct = 0;

    for (size_t i = 0; i < _param.getNumParam(); i++) {
		for (size_t j = 0; j < _param.getForbiddenParms().size(); j++) {
            ct = 0;
            for (size_t k = 0; k < _param.getForbiddenParms()[j].size(); k++) {
                if(_param.getForbiddenParms()[j][k]._name == _param.getParms()[i]){
                    if(_param.getForbiddenParms()[j][k]._value == _param.getDomain(i).at(vector[i])){
                        ct = ct | 1;
                    } else {
                        ct = ct & 0;
                        break;
                    }
                }
            }
            if (ct == 1){
                break;
            }
        }
        if (ct == 1){
            break;
        }
    }

    if (ct == 1){
        fitness = INFINITY;
    } else {
        std::string command;

        // for (int i = 0; i < _param.getNumParam(); i++) {
        //     this->getParamHeader().setParamDefault(
        //         this->getParamHeader().getParms()[i], 
        //         this->getParamHeader().getDomain(i).at(vector[i]));
        // }

        int t, i;

        double val = 0.0;

        // #pragma omp critical 
        // {
        //     std::cout << "New Incumbent: ";
        //     std::cout << " " << static_cast<double>(0.0) / 1000.0;
        //     std::cout << ", " << 0;
        //     std::cout << std::endl;
        //     std::cout << "With state ";

        //     for (int i = 0; i < _param.getNumParam() - 1; i++) {
        //         std::cout << _param.getParms().at(i) << "=" << _param.getDomain(i).at(vector[i]) << ", ";
        //     }
            
        //     std::cout << _param.getParms().at(_param.getNumParam() - 1) << "=" << _param.getDomain(_param.getNumParam() - 1).at(vector[_param.getNumParam() - 1]) << std::endl;

        // }

        std::ifstream temp_file;
        std::pair<double, unsigned> pair;
        unsigned index = 0;
        //clock_t begin = 0, end = 0;

        char *cline = new char[MAX_LINE];
        std::string sline;

        std::string str;

        double total_runtime_time = 0.0;

        std::vector<uint32_t> my_vector;
        for(size_t i = 0; i < _train_instances.size(); i++){
            my_vector.push_back(i);
        }
        
        std::set<uint32_t> indexes;
        std::vector<uint32_t> choices;
        size_t max_index = my_vector.size() - 1;
        uint32_t min = 0;
        uint32_t max = 0;
        std::uniform_int_distribution<uint32_t> distribution;
        
        thread_local std::mt19937 _generator;
        uint32_t seed = std::chrono::system_clock::now().time_since_epoch().count();
        _generator.seed(1);
        
        //max = std::min(_param.getNumParam(), max_index);
        max = _param.getNumParam();
        //max = 2;
        distribution = std::uniform_int_distribution<uint32_t>(min, max_index);
        while (indexes.size() < max)
        {
            uint32_t random_index = distribution(_generator);
            if (indexes.find(random_index) == indexes.end())
            {
                choices.push_back(my_vector[random_index]);
                indexes.insert(random_index);
            }
        }

        // for(size_t i = 0; i < my_vector.size() - 1; i++){
        //     indexes.insert(i);
        // }

        int z = 0;
        
        for (std::set<uint32_t>::iterator it = indexes.begin(); it!=indexes.end(); ++it) {
            //std::cout << "set [" << *it << "]: " << this->scenario.getTrainInstance().at(*it) << std::endl;
            command = _program + " " + _train_instances[*it] + " ";

            command += "\"\" ";

            command += std::to_string(_cutoff_time) + " ";

            command += std::to_string(std::numeric_limits<int>::max()) + " ";

            command += "1056789135 ";

            size_t num_param = _param.getNumParam();

            for (i = 0; i < num_param - 1; i++) {
                command += "-" + _param.getParms()[i] + " ";
                std::string s = _param.getDomain(i)[vector[i]];
                command += _param.getDomain(i)[vector[i]] + " ";
            }
            command += "-" + _param.getParms().at(num_param - 1) + " ";
            command += _param.getDomain(num_param - 1).at(vector[num_param - 1]);

            std::cout << "[THREAD " << omp_get_thread_num() << ", EXEC: " << z << "] " << "Executing cmd: " << command << std::endl;
            //std::cout << "	Trial 1 for calling: " << command << std::endl;
            std::string tempname = "temp_" + std::to_string(omp_get_thread_num()) +".txt";
            command += " > ";
            command += tempname;

            int sys_rs = system(command.c_str());
            // if(sys_rs == 0){
            //     std::cout << "Error: Command " << sys_rs << std::endl;
            //     exit(-1);
            // }

            temp_file.open(tempname);
            if (!temp_file.is_open()) {
                std::cout << "Error: File " << tempname << " not found!" << std::endl;
                exit(-1);
            }

            double quality = 0.0;
            double runtime_time = 0.0;

            while (!temp_file.eof()) {
                temp_file.getline(cline, MAX_LINE);
                sline = cline;
                if (sline[0] != '[' && sline[0] != '{') {
                    std::string v = sline;
                    std::replace(v.begin(), v.end(), ' ', ',');
                    std::replace(v.begin(), v.end(), ',', ' ');
                    std::istringstream iss(v);
                    std::vector<std::string> str_split((std::istream_iterator<std::string>(iss)),
                                                        std::istream_iterator<std::string>());
                    // std::cout << "v = " << v << std::endl;
                    // std::cout << "str_split[6] = " << str_split[6] << std::endl;
                    // std::cout << "str_split[4] = " << str_split[4] << std::endl;
                    quality = std::stod(str_split[6]);
                    runtime_time = std::stod(str_split[4]);
                    break;
                }
            }

            double val = std::log(quality) * runtime_time;
            fitness += val;

            total_runtime_time += runtime_time;

            #pragma omp critical 
            {
                std::cout << "[THREAD " << omp_get_thread_num() << ", EXEC: " << z << "] " << "quality: " << quality << std::endl;
                std::cout << "[THREAD " << omp_get_thread_num() << ", EXEC: " << z << "] " << "runtime_time: " << runtime_time << std::endl;
                std::cout << "[THREAD " << omp_get_thread_num() << ", EXEC: " << z << "] " << "total_runtime_time: " << total_runtime_time << std::endl;
                std::cout << "[THREAD " << omp_get_thread_num() << ", EXEC: " << z << "] " << "fitness: " << val << std::endl;
                z++;
            }

            if(total_runtime_time >= _tunerTimeout){
                temp_file.close();
                break;
            }

            temp_file.close();
        }

        fitness = fitness / static_cast<double>(max_index);
    }

    std::cout << "----------------------------------------------\n";
    std::cout << "fitness global: " << fitness << std::endl;

    return fitness;
}
