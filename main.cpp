#include <iostream>
#include "boa.h"


void test_scenario();
void test_boa();

int main(int argc, char* argv[]){

    int num_arg = 1;
    std::string arg;

    std::string scenariofile;
    int njobs = 1;

    unsigned int tuner_timeout = 10;
    double timeout = 0.0;
    double currenttime = 0.0;

    while (num_arg < argc) {
        arg = argv[num_arg];
        if (arg == "-scenariofile") {
            num_arg++;
            scenariofile = argv[num_arg];
        }
        else if (arg == "-njobs") {
            num_arg++;
            njobs = std::stoi(argv[num_arg]);
        }
        num_arg++;
    }

    Scenario *scenario = new Scenario(scenariofile.c_str());
    //Scenario scenario(scenariofile.c_str());
    tuner_timeout = scenario->getTunerTimeout();

    std::cout << scenario->getParamHeader().getNumParam() << " parameters in total" << std::endl;
    std::cout << scenario->getParamHeader().getNumConditionalParam() << " parameters are conditional" << std::endl;
    std::cout << scenario->getParamHeader().getNumForbiddenCombo() << " parameter combinations are forbidden" << std::endl;

    std::cout << "num_params = " << scenario->getParamHeader().getNumParam() << ", ";
    std::cout << "num_combos = " << scenario->getParamHeader().getNumCombos() << std::endl;

    // std::cout << "seed: " << seedforParam << std::endl;

    // std::cout << "Run " << numRun << std::endl;

    // std::cout << "========================================================" << std::endl;
    // std::cout << "Starting BNT for limit of N=" << numinstances << ", and tuner timeout=" << tuner_timeout << ".\nCurrent CPU time = " << currenttime << ", this run goes until "<< timeout << std::endl;
    // std::cout << "========================================================" << std::endl;

    Boa *boa = new Boa(*scenario, njobs);
    boa->run();

    //srand(time(NULL));
    //test_marginal_probability();
    //test_scenario();
    //test_boa();

    delete boa;
    delete scenario;

    return 0;
}

// void test_scenario() {
//     Scenario *scenario = new Scenario("scenario3.txt");
//     //Population * pop = new Population(*scenario, 16);
//     delete scenario;
// }

// void test_boa() {
//     const Scenario *scenario = new Scenario("scenario.txt");
//     Boa *boa = new Boa(*scenario, 8);
//     boa->run();

//     delete boa;
//     delete scenario;
// }


