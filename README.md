## BayesianNetworkTuning

BNT was implemented in C++. The algorithm loads the meta-heuristic executable just like CALIBRA, however it adopts the ParamILS scenario approach. The scenario approach is more flexible in several ways as shown above, and is more easily adapted to any operating system. Parameter values are discret ranges. On the other hand, the scenario file is more simplified. The scenario file has the following settings:

- program: ParamILS_BRKGA_BinPacking.exe
- overall_obj: mean
- tunerTimeout: 43200‬
- paramfile: brkga-params.txt
- outdir: parambrkga-out
- train_instance_file: data/binpacking1d-train.txt
- test_instance_file: data/binpacking1d-test.txt

This executable must follow the following standards:

[EXECUTABLE] [INSTANCE_FILE] [PARAMETER_VALUES]

The [INSTANCE_FILE] is the instance and [PARAMETER_VALUES] are the same as those determined in the parameter file. In each call of the executable over an instance, a TEMP.DAT file stores the cost of that operation. The "tunerTimeout" attribute represents the maximum run time of the calibrator. Finally, the "mean" is used to evaluate the set of parameters provided for the meta-heuristic executable. Because this version is not yet adaptive, the cost of a set of parameters is only calculated after evaluating all training instances.
