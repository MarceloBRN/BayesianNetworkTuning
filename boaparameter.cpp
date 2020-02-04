#include "boaparameter.h"

_boaparameter::_boaparameter()
{
}

_boaparameter::_boaparameter(int num_bits, int max_iter, int pop_size, int select_size, int num_children)
{
    this->num_bits = num_bits;
    this->max_iter = max_iter;
    this->pop_size = pop_size;
    this->select_size = select_size;
    this->num_children = num_children;
}
