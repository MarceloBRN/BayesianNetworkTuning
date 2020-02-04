#include <iostream>
#include <string>
#include "paramreader.h"
#include "scenario.h"
#include "boa.h"

int main(int argc, char* argv[]) {

    int num_arg = 1;
    std::string arg;

    std::string scenariofile;
    int numRun = 0;
    int seedforParamBRKGA = static_cast<int>(time(nullptr));

    int numinstances = 2000;
    unsigned int tuner_timeout = 10;
    double timeout = 0.0;
    double currenttime = 0.0;

    while (num_arg < argc) {
        arg = argv[num_arg];
        if (arg == "-scenariofile") {
            num_arg++;
            scenariofile = argv[num_arg];
        }
        else if (arg == "-numrun") {
            num_arg++;
            numRun = std::stoi(argv[num_arg]);
        }
        else if (arg == "-seedBRKGA") {
            num_arg++;
            seedforParamBRKGA = std::stoi(argv[num_arg]);
            if (seedforParamBRKGA < 0) {
                std::cerr << "ERROR: Invalid seed value!" << std::endl;
                exit(-1);
            }
        }
        num_arg++;
    }

    Scenario scenario(scenariofile.c_str());
    //ParamReader param = scenario.getParamHeader();
    tuner_timeout = scenario.getTunerTimeout();

    std::cout << scenario.getParamHeader().getNumParam() << " parameters in total" << std::endl;
    std::cout << scenario.getParamHeader().getNumConditionalParam() << " parameters are conditional" << std::endl;
    std::cout << scenario.getParamHeader().getNumForbiddenCombo() << " parameter combinations are forbidden" << std::endl;

    std::cout << "num_params = " << scenario.getParamHeader().getNumParam() << ", ";
    std::cout << "num_combos = " << scenario.getParamHeader().getNumCombos() << std::endl;

    std::cout << "seed: " << seedforParamBRKGA << std::endl;

    std::cout << "Run " << numRun << std::endl;

    std::cout << "========================================================" << std::endl;
    std::cout << "Starting ParamBRKGA for limit of N=" << numinstances << ", and tuner timeout=" << tuner_timeout << ".\nCurrent CPU time = " << currenttime << ", this run goes until "<< timeout << std::endl;
    std::cout << "========================================================" << std::endl;

    unsigned n = scenario.getParamHeader().getNumParam();		// size of chromosomes
    const unsigned p = 30;	// size of population
    const double pm = 0.35;		// fraction of population to be replaced by mutants
    const unsigned MAXT = 4;	// number of threads for parallel decodings

    unsigned generation = 0;		// current generation
    const unsigned MAX_GENS = 100;	// run for 1000 gens

    BOAParameter *boaparam = new BOAParameter(scenario.getParamHeader().getNumParam(), MAX_GENS, p, p, p/2);

    Boa *boa = new Boa(scenario, *boaparam);



    boa->search();

    //do {
    //    ++generation;
    //} while (generation < MAX_GENS);

    return 0;
}

