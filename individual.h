#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>
#include <cstdint>
#include <iostream>

using namespace std;

class Individual
{
    public:
        Individual();
        Individual(const std::vector<size_t> &data, const double &cost);
        double cost() const;
        void print(std::ostream &output = std::cout) const;
        const std::vector<size_t> get_data() const;

        virtual ~Individual();

    private:
        double _cost;
        std::vector<size_t>_data;
};

#endif // INDIVIDUAL_H
