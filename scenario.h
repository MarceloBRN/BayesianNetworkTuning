#ifndef SCENARIO_H
#define SCENARIO_H

#include <string>
#include <vector>
#include "ParamReader.h"

#pragma once

class Scenario
{
public:
	Scenario();
	Scenario(const char*  scenario);
	virtual ~Scenario();
	ParamReader getParamHeader() const;
	double getCutOffTime() const;
	unsigned getTunerTimeout() const;
	std::string getOverallObj() const;
	std::string getProgramName() const;
	std::vector<std::string> getTrainInstance() const;
	std::vector<std::string> getTestInstance() const;
	std::vector<double> getSolutionsTrainInstance() const;
	std::vector<double> getSolutionsTestInstance() const;
	//std::vector<std::string> getArgs();
    void setParamHeader(const ParamReader &param);

private:
	std::string _program;
	bool _deterministic = true;
	std::string _overall_obj;
	double _cutoff_time;
	unsigned _tunerTimeout;
	std::string _paramfilename;
	std::string _outdir;
	std::string _train_instance_file;
	std::string _test_instance_file;
	std::vector<std::string> _train_instances;
	std::vector<std::string> _test_instances;
	std::vector<double> _solutions_train_instances;
	std::vector<double> _solutions_test_instances;
	//std::vector<std::string> _args;
	ParamReader _param;

	std::vector<std::string> _split(std::string s, std::string delimiter);

};

#endif //SCENARIO_H