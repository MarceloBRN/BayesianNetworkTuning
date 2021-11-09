#include "individual.h"
#include <limits>

Individual::Individual()
{
    //_data = nullptr;
    _cost = std::numeric_limits<double>::infinity();
}

Individual::Individual(const std::vector<size_t> &data, const double &cost) {
    _cost = cost;
    _data = data;
}

double Individual::cost() const {
    return _cost;
}


void Individual::print(std::ostream &output) const
{
    size_t i = 0;
    output << "[";
    for(i = 0; i < _data.size() - 1; i++){
        output << _data[i] << ", ";
    }
    output << _data[i] << "]";
    output << " => " << _cost;
}

const std::vector<size_t> Individual::get_data() const{
    return _data;
}

Individual::~Individual()
{
    //delete[] _data;
}
