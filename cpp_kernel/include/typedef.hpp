#ifndef TYPEDEF
#define TYPEDEF

#include <complex>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <utility>
// #include <list>

class Instruction;
class Parameter;

using Str                = std::string;
using Int                = int;
using Uint               = unsigned int;
using Double             = double;
using Complex            = std::complex<double>;
using ParameterPtr       = std::shared_ptr<Parameter>;
using ComplexVec         = std::vector<Complex>;
using DoubleVec          = std::vector<Double>;
using ParameterPtrVector = std::vector<ParameterPtr> ;

// typedef std::vector<Complex> Data;
// typedef std::shared_ptr<Data> DataPtr;
// typedef std::shared_ptr<Instruction> InstructionPtr;
// typedef std::shared_ptr<Parameter> ParameterPtr;

namespace consts{ 
    constexpr Complex i = {0,1}; 
    constexpr Uint MAX_QUBITS = sizeof(Uint); 
}

#endif