#include "ParamReader.h"
#include <fstream>
#include <iostream>
#include <regex>

using namespace std;

#define MAX_SIZE_STR_LINE 1024

ParamReader::ParamReader()
{
}

ParamReader::ParamReader(const char* pfile)
{
	std::ifstream file(pfile);
	if (!file.is_open()) {
		std::cout << "Error: File " << pfile << " not found!" << std::endl;
		exit(-1);
	}

	char *cline = new char[MAX_SIZE_STR_LINE];
	std::string sline;
	std::smatch m;
	std::regex r1;
	r1 = "^\\s*(#.*)?\\n$";
	std::regex r2;
	r2 = "^(.*)\\s*\\{(.*)\\}\\s*\\[(.*)\\]";
	std::regex r3;
	r3 = "^(.*)\\|(.*) in \\{(.*)\\}";

	std::string parname, vals, default_val;
	std::string conditional_param, deciding_param, deciding_vals;
	std::vector<std::string> values;
	char sep = ',';

	while (!file.eof()) {
		//Match parameter format: param_name {value1,value2,...}[default_value]
		file.getline(cline, MAX_SIZE_STR_LINE);
		sline = cline;
		//if (!std::regex_match(sline, r1)) { break; } //deal with empty and comment lines
		std::regex_search(sline, m, r2);
		parname = m[1].str();
		while (parname.back() == ' ') {
			parname.pop_back();
		}
		vals = m[2].str();
		default_val = m[3].str();
		vals.erase(remove_if(vals.begin(), vals.end(), isspace), vals.end());
		for (size_t p = 0, q = 0; p != vals.npos; p = q)
			values.push_back(vals.substr(p + (p != 0), (q = vals.find(sep, p + 1)) - p - (p != 0)));
		//for (size_t p = 0; p < values.size(); p++) {
		//	std::cout << values[p] << " ";
		//}
		//std::cout << "\n";
		_param.push_back(parname);
		_domain.push_back(values);
		_pdefault[parname] = default_val;
		values.clear();
	}

	while (!file.eof()) {
		file.getline(cline, MAX_SIZE_STR_LINE);
		sline = cline;
	}

	file.close();
	delete[] cline;
}

ParamReader::~ParamReader()
{
	
}

std::string ParamReader::filename()
{
	return _filename;
}

int ParamReader::getNumParam() const
{
	return static_cast<int>(_param.size());
}

int ParamReader::getNumConditionalParam()
{
	return static_cast<int>(_conditionals.size());
}

int ParamReader::getNumForbiddenCombo()
{
	return static_cast<int>(_forbidden_combos.size());
}

std::vector<std::string> ParamReader::getParms() const
{
	return this->_param;
}

int ParamReader::getNumCombos()
{
	int ncombo = 1;
	for (size_t i = 0; i < this->_domain.size(); i++) {
		ncombo *= static_cast<int>(this->_domain[i].size());
	}

	return ncombo;
}

std::string ParamReader::getParamDefault(std::string param)
{
	return this->_pdefault[param];
}

std::vector<std::string> ParamReader::getDomain(size_t idx)
{
	return this->_domain[idx];
}

void ParamReader::setParamDefault(std::string param, std::string value)
{
	_pdefault[param] = value;
}