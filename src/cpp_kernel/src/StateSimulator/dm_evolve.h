#ifndef _GATESIM
#define _GATESIM
#include <string>
#include <utility>

#include "common.h"
#include "instruction.h"
#include "tensorspec.h"
// #include "QuantumCircuit.h"
    
class Operator;
class DensityMatrix;

constexpr int num_thread=1;
// class Param
void _fill(int init, int finit, int dif, const Data& l, Data& r, const mask& buffer_mask, const std::vector<int>& expand);




#endif