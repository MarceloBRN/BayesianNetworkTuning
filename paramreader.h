#ifndef PARAMREADER_H
#define PARAMREADER_H

#include <vector>
#include <unordered_map>
#include <string>

#define MAX_SIZE_STR_LINE 1024

#pragma once

class ParamReader
{
public:
    ParamReader();
	ParamReader(const char* pfile);
	~ParamReader();

	std::string filename();
	int getNumParam() const;
	int getNumConditionalParam();
	int getNumForbiddenCombo();

	std::vector<std::string> getParms() const;
	int getNumCombos();

	std::string getParamDefault(std::string param);

	void setParamDefault(std::string param, std::string value);

	std::vector<std::string> getDomain(size_t idx);

private:
	std::vector<std::string> _param;
	std::vector<std::vector<std::string>> _domain;
	std::unordered_map<std::string, std::string> _pdefault;
	std::unordered_map<std::string, std::vector<std::string>> _conditionals;
	std::vector<std::string> _forbidden_combos;

	std::string _filename;

};
#endif // PARAMREADER_H
