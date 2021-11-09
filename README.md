## BayesianNetworkTuning

BNT was implemented in C++. 

The scenario file has the following settings:

- algo = ParamILS_BRKGA_BinPacking.exe
- execdir = .
- deterministic = 0
- run_obj = runtime
- wallclock_limit = 28800
- cutoff_time = 300
- paramfile = ./target_algorithms/sat/cryptominisat-cssc14/params.pcs
- instance_file = ./instances/sat/sets/circuit_fuzz/training.txt
- test_instance_file = ./instances/sat/sets/circuit_fuzz/test.txt
- feature_file = ./instances/sat/sets/circuit_fuzz/features.txt

This executable must follow the following standards:

[EXECUTABLE] [INSTANCE_FILE] [PARAMETER_VALUES]

The [INSTANCE_FILE] is the instance and [PARAMETER_VALUES] are the same as those determined in the parameter file. The "wallclock_limit" attribute represents the maximum run time of the calibrator. Finally, the "mean10" is used to evaluate the set of parameters provided for the meta-heuristic executable. Because this version is not yet adaptive, the cost of a set of parameters is only calculated after evaluating all training instances.
