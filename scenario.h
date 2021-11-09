#ifndef SCENARIO_H
#define SCENARIO_H

#ifndef _OPENMP
#define _OPENMP
#endif


#if defined(_OPENMP)
   #include <omp.h>
#endif


#include <string>
#include <vector>
#include <random>
#include "individual.h"
#include "paramreader.h"


class Scenario
{
public:
	Scenario();
	Scenario(const char* scenario);
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
	Individual *genData() const;
	
    void setParamHeader(const ParamReader &param);
	double _cost(const std::vector<size_t> &vector) const;

private:
	std::string _program;
	bool _deterministic = true;
	std::string _overall_obj = "mean";
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
	ParamReader _param;

};

#endif //SCENARIO_H
