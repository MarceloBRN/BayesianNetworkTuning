#include "paramreader.h"
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
	std::smatch m, m2;

	std::string parname, vals, default_val, cond_vals, forb_vals;
	std::string conditional_param, deciding_param, deciding_vals;
	std::vector<std::string> values;
	char sep = ',';

	while (!file.eof()) {
		//Match parameter format: param_name {value1,value2,...}[default_value]
		file.getline(cline, MAX_SIZE_STR_LINE);
		sline = cline;

        if (cline[0] != '#' && sline.find("Conditionals") == std::string::npos) {

            //if (!std::regex_match(sline, r1)) { break; } //deal with empty and comment lines

            parname.clear();
            vals.clear();
            default_val.clear();
            cond_vals.clear();

            size_t i = 0;
            while (isspace(sline[i]) && i < sline.size()) {
                i++;
            }

            while (!isspace(sline[i]) && sline[i] != '[' && i < sline.size()) {
                parname += sline[i];
                i++;
            }

            if (parname == "Conditionals:"){
                break;
            } 

            if (parname != ""){
                while (isspace(sline[i]) && i < sline.size()) {
                    i++;
                }

                if (sline[i] == '{') {
                    i++;
                    while (cline[i] != '}' && i < sline.size()) {
                        vals += sline[i];
                        i++;
                    }
                } else if (sline[i] == '[') {
                    i++;
                    while (sline[i] != ']' && i < sline.size()) {
                        vals += sline[i];
                        i++;
                    }

                    i++;
                    while (isspace(sline[i]) && i < sline.size()) {
                        i++;
                    }

                    if (sline[i] == '[') {
                        i++;
                        while (sline[i] != ']' && i < sline.size()) {
                            default_val += sline[i];
                            i++;
                        }
                    }


                    bool is_float = true;
                    i++;
                    while (isspace(sline[i]) && i < sline.size()) {
                        i++;
                    }
                    
                    if (i < sline.size() - 1){
                        if (sline[i] == 'i' || sline[i + 1] == 'l'){
                            is_float = false;
                        }
                        if (sline[i] == 'l' || sline[i + 1] == 'i'){
                            is_float = false;
                        }
                    }

                    if (i < sline.size()){
                        if (sline[i] == 'i'){
                            is_float = false;
                        }
                    }

                    std::string v = vals;
                    std::replace(v.begin(), v.end(), ',', ' ');
                    std::istringstream iss(v);
                    std::vector<std::string> str_split((std::istream_iterator<std::string>(iss)),
                                                        std::istream_iterator<std::string>());
                    
                    double ini = std::stod(str_split[0]);
                    double end = std::stod(str_split[1]);
                    double interval = (end - ini) / MAX_SIZE_INTERVAL;
                    std::ostringstream ss;
                    ss << ini << ", ";
                    for (double i = 1.0; i <= (MAX_SIZE_INTERVAL - 1); i++)
                    {
                        if(is_float){
                            ss << static_cast<double>(ini + i * interval) << ", ";
                        } else {
                            ss << static_cast<long int>(ini + i * interval) << ", ";
                        }
                        
                    }
                    ss << end;
                    vals = ss.str();
                } else if(sline[i] == '|') {
                    i++;
                    while (isspace(sline[i]) && i < sline.size()) {
                        i++;
                    }

                    std::string cond_param_name = "";

                    while (!isspace(sline[i]) && i < sline.size()) {
                        cond_param_name += sline[i];
                        i++;
                    }

                    while (isspace(sline[i]) && i < sline.size()) {
                        i++;
                    }

                    if (sline[i] == 'i' && sline[i + 1] == 'n') {
                        i += 2;
                    }

                    while (isspace(sline[i]) && i < sline.size()) {
                        i++;
                    }

                    if (sline[i] == '{') {
                        i++;
                        while (cline[i] != '}' && i < sline.size()) {
                            cond_vals += sline[i];
                            i++;
                        }

                        if (cond_vals != ""){
                            _CondParam cp;
                            cp._name = cond_param_name;
                            cond_vals.erase(remove_if(cond_vals.begin(), cond_vals.end(), ::isspace), cond_vals.end());
                            for (size_t p = 0, q = 0; p != cond_vals.npos; p = q) {
                                cp._values.push_back(cond_vals.substr(p + (p != 0), (q = cond_vals.find(sep, p + 1)) - p - (p != 0)));
                            }
                            _conditionals[parname].push_back(cp);
                        }
                    }
                }

                vals.erase(remove_if(vals.begin(), vals.end(), ::isspace), vals.end());
                for (size_t p = 0, q = 0; p != vals.npos; p = q) {
                    values.push_back(vals.substr(p + (p != 0), (q = vals.find(sep, p + 1)) - p - (p != 0)));
                }

                if (vals != ""){
                    _param.push_back(parname);
                    _domain.push_back(values);
                    _pdefault[parname] = default_val;
                }
                
            }            
            
            values.clear();
        } 
        
        if(cline[0] == '{'){
            parname.clear();
            forb_vals.clear();

            size_t i = 0;
            i++;
            while (isspace(sline[i]) && i < sline.size()) {
                i++;
            }

            while (sline[i] != '}' && i < sline.size()) {
                forb_vals += sline[i];
                i++;
            }

            if (forb_vals != ""){
                _ForbiddenParam fp;
                std::vector<_ForbiddenParam> fp_vec;
                fp_vec.clear();
                std::string val = "";

                forb_vals.erase(remove_if(forb_vals.begin(), forb_vals.end(), ::isspace), forb_vals.end());
                for (size_t p = 0, q = 0; p != forb_vals.npos; p = q) {
                    val = forb_vals.substr(p + (p != 0), (q = forb_vals.find(sep, p + 1)) - p - (p != 0));
                    std::stringstream ss(val);
                    std::string s1;
                    getline(ss, s1, '=');
                    std::string s2;
                    getline(ss, s2, '=');
                        
                    fp._name = s1;
                    fp._value = s2;
                    fp_vec.push_back(fp);
                }

                _forbidden.push_back(fp_vec);
            }

        }
		
	}

    for(size_t i = 0; i < this->_param.size(); i++){
        _Param *p = new _Param();
        p->_name = this->_param[i];
        
        for(size_t j = 0; j < this->_domain[i].size() - 1; j++){
            std::unordered_map<std::string, size_t> v;
            v[this->_domain[i][j]] = j;
            p->_indexes.push_back(v);
        }
        _param_indexes.push_back(*p);
        _param_map[this->_param[i]] = i;
    }

    std::cout << "\nPARAMETERS:\n";
    for(size_t i = 0; i < this->_param.size(); i++){
        std::cout << this->_param[i] << ": " ;
        for(size_t j = 0; j < this->_domain[i].size() - 1; j++){
            std::cout << this->_domain[i][j] << ", ";
        }
        std::cout << this->_domain[i][this->_domain[i].size() - 1];
        std::cout << "\n";
    }
    std::cout << "\n";

    if(!_conditionals.empty()){
        std::cout << "\nCONDITIONALS:\n";
        for (auto& cp: _conditionals) {
            std::cout << cp.first << " | " ;
            for(size_t j = 0; j < cp.second.size(); j++){
                std::cout << cp.second[j]._name << " in {";
                for(size_t k = 0; k < cp.second[j]._values.size() - 1; k++){
                    std::cout << cp.second[j]._values[k] << ", ";
                }
                std::cout << cp.second[j]._values[cp.second[j]._values.size() - 1];
                std::cout << "}, ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    if(!_forbidden.empty()){
        std::cout << "\nFORBIDDEN:\n";
        for (auto& fp: _forbidden) {
            std::cout << "{" ;
            for(size_t j = 0; j < fp.size(); j++){
                std::cout << fp[j]._name << " = " << fp[j]._value;
                std::cout << ", ";
            }
            std::cout << "}\n";
        }
        std::cout << "\n";
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

size_t ParamReader::getNumParam() const
{
	return static_cast<int>(_param.size());
}

size_t ParamReader::getNumConditionalParam() const
{
	return static_cast<int>(_conditionals.size());
}

size_t ParamReader::getNumForbiddenCombo() const
{
	return static_cast<int>(_forbidden.size());
}

std::vector<std::string> ParamReader::getParms() const
{
	return _param;
}

std::unordered_map<std::string, std::vector<struct _cond_param>> ParamReader::getConditionalParms() const
{
	return _conditionals;
}

std::vector<std::vector<struct _forbidden_param>> ParamReader::getForbiddenParms() const{
    return _forbidden;
}

size_t ParamReader::getNumCombos() const
{
	int ncombo = 1;
	for (size_t i = 0; i < this->_domain.size(); i++) {
		ncombo *= static_cast<int>(this->_domain[i].size());
	}

	return ncombo;
}

std::string ParamReader::getParamDefault(const std::string &param) const
{
	return _pdefault.at(param);
}

std::vector<std::string> ParamReader::getDomain(const size_t &idx) const
{
	return _domain[idx];
}

void ParamReader::setParamDefault(const std::string &param, const std::string &value)
{
	_pdefault[param] = value;
}
