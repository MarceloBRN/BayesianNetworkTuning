#include "Scenario.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <cassert>
#include <sstream>

#define MAX_LINE 2048

Scenario::Scenario()
{
}

Scenario::Scenario(const char* scenario)
{
	char *cline = new char[MAX_LINE];
	std::ifstream file(scenario);
	if (!file.is_open()) {
		std::cout << "Error: File " << scenario << " not found!" << std::endl;
		exit(-1);
	}
	std::string sline;
	std::string instanceline;
	double solutionline;
	std::smatch m;

	std::string paramindicator;

	std::regex r1;
	r1 = "^(.*)\\s*:\\s*(.*)";

	char sep = ',';

	while (!file.eof()) {
		file.getline(cline, MAX_LINE);
		sline = cline;
		std::regex_search(sline, m, r1);
		paramindicator = m[1].str();
		std::vector<std::string> splited_line;

		while (paramindicator.back() == ' ') {
			paramindicator.pop_back();
		}

		if (paramindicator == "program") {
			this->_program = m[2].str();
		}
		else if (paramindicator == "deterministic") {
			this->_deterministic = std::stoi(m[2].str());
		}
		else if (paramindicator == "overall_obj") {
			this->_overall_obj = m[2].str();
		}
		else if (paramindicator == "cutoff_time") {
			this->_cutoff_time = std::stod(m[2].str());
		}
		else if (paramindicator == "tunerTimeout") {
			this->_tunerTimeout = std::stoul(m[2].str());
		}
		else if (paramindicator == "paramfile") {
			this->_paramfilename = m[2].str();
			this->_param = *(new ParamReader(this->_paramfilename.c_str()));
		}
		else if (paramindicator == "outdir") {
			this->_outdir = m[2].str();
		}
		else if (paramindicator == "train_instance_file") {
			this->_train_instance_file = m[2].str();
			std::ifstream train(this->_train_instance_file);
            if (!train.fail()) {
				std::stringstream ss;
				std::string item;
                while (!train.eof()) {
                    train.getline(cline, MAX_LINE);
                    instanceline = cline;
					splited_line = this->_split(cline, " ");
                    this->_train_instances.push_back(splited_line[0]);
                    //train.getline(cline, MAX_LINE);
                    //solutionline = std::stod(cline);
                    this->_solutions_train_instances.push_back(std::stod(splited_line[1]));
					splited_line.clear();
                }
            }
            else {
                std::cout << "[ERROR]: Train Instance File not found!" << std::endl;
            }
            train.close();
			
		}
		else if (paramindicator == "test_instance_file") {
			this->_test_instance_file = m[2].str();
			std::ifstream test(this->_test_instance_file);
            if (!test.fail()) {
                while (!test.eof()) {
                    test.getline(cline, MAX_LINE);
                    instanceline = cline;
					splited_line = this->_split(cline, " ");
                    this->_test_instances.push_back(splited_line[0]);
                    //test.getline(cline, MAX_LINE);
                    //solutionline = std::stod(cline);
                    this->_solutions_test_instances.push_back(std::stod(splited_line[1]));
					splited_line.clear();
                }
            }
            else {
                std::cout << "[ERROR]: Test Instance File not found!" << std::endl;
            }
			test.close();
		}
		//else if (paramindicator == "args") {
		//	
		//	std::string vals = m[2].str();
		//	vals.erase(remove_if(vals.begin(), vals.end(), isspace), vals.end());
		//	for (size_t p = 0, q = 0; p != vals.npos; p = q) {
		//		this->_args.push_back(vals.substr(p + (p != 0), (q = vals.find(sep, p + 1)) - p - (p != 0)));
		//	}
		//}
		else {
			std::cout << "Error: Indicator " << paramindicator << " not recognize!" << std::endl;
			exit(-1);
		}
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
	return this->_cutoff_time;
}

unsigned Scenario::getTunerTimeout() const
{
	return this->_tunerTimeout;
}

std::string Scenario::getOverallObj() const
{
	return this->_overall_obj;
}

std::string Scenario::getProgramName() const
{
	return this->_program;
}

std::vector<std::string> Scenario::getTrainInstance() const
{
	return this->_train_instances;
}

std::vector<std::string> Scenario::getTestInstance() const
{
	return this->_test_instances;
}

std::vector<double> Scenario::getSolutionsTrainInstance() const
{
	return this->_solutions_train_instances;
}

std::vector<double> Scenario::getSolutionsTestInstance() const
{
	return this->_solutions_test_instances;
}

void Scenario::setParamHeader(const ParamReader & param)
{
    this->_param = param;
}

std::vector<std::string> Scenario::_split(std::string s, std::string delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

//std::vector<std::string> Scenario::getArgs()
//{
//	return this->_args;
//}
