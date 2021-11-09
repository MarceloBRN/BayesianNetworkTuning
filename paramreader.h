#ifndef PARAMREADER_H
#define PARAMREADER_H

#include <vector>
#include <unordered_map>
#include <string>

#define MAX_SIZE_STR_LINE 1024

#define MAX_SIZE_INTERVAL 5.0

#pragma once

typedef struct _param{
	std::string _name;
	std::vector<std::unordered_map<std::string, size_t>> _indexes;
}_Param;

typedef struct _cond_param{
	std::string _name;
	std::vector<std::string> _values;
	std::string _op;
}_CondParam;


typedef struct _forbidden_param{
	std::string _name;
	std::string _value;
}_ForbiddenParam;


class ParamReader
{
public:
    ParamReader();
	ParamReader(const char* pfile);
	~ParamReader();

	std::string filename();
	size_t getNumParam() const;
	size_t getNumConditionalParam() const;
	size_t getNumForbiddenCombo() const;

	std::vector<std::string> getParms() const;
	std::unordered_map<std::string, std::vector<struct _cond_param>> getConditionalParms() const;
	std::vector<std::vector<struct _forbidden_param>> getForbiddenParms() const;
	size_t getNumCombos() const;

	std::string getParamDefault(const std::string &param) const;

	void setParamDefault(const std::string &param, const std::string &value);

	std::vector<std::string> getDomain(const size_t &idx) const;

private:
	std::vector<std::string> _param;
	std::vector<std::vector<std::string>> _domain;
	std::unordered_map<std::string, std::string> _pdefault;
	
	std::vector<std::string> _forbidden_combos;

	std::string _filename;
	std::unordered_map<std::string, std::vector<struct _cond_param>> _conditionals;
	std::vector<std::vector<struct _forbidden_param>> _forbidden;
	std::vector<struct _param> _param_indexes;

	std::unordered_map<std::string, size_t> _param_map;

};
#endif // PARAMREADER_H
