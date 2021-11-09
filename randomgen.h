// #ifndef RANDOMNUMBER_H
// #define RANDOMNUMBER_H

// #if defined(_OPENMP)
//    #include <omp.h>
// #endif

// #include <random>

// class RandomNumber
// {

// public:


//     int intRand(int min, int max)
//     {
//         std::uniform_int_distribution<int> uidist(min, max);
//         return uidist(engine);
//     }

//     double doubleRand(double min, double max) {
//         thread_local std::random_device rd;
//         thread_local std::mt19937 generator(rd());
//         std::uniform_real_distribution<double> distribution(min, max);
//         return distribution(generator);
//     }

// private:
//     std::mt19937 engine;


// };

// // RandomHelper.cpp
// RandomNumber& RandomNumber::getInstance()
// {
//     static RandomHelper instance;
//     return instance;
// }

// #endif //RANDOMNUMBER_H