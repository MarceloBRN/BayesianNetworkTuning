#ifndef BOAPARAMETER_H
#define BOAPARAMETER_H

typedef struct _boaparameter
{
    _boaparameter();
    _boaparameter(int num_bits, int max_iter, int pop_size, int select_size, int num_children);

    int num_bits;
    int max_iter;
    int pop_size;
    int select_size;
    int num_children;
}BOAParameter;

#endif // BOAPARAMETER_H


