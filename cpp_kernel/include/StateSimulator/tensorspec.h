#ifndef _TENSORSPEC
#define _TENSORSPEC

#include "tensor.h"
#include <unordered_map>


class Operator: public Tensor{
public:
    using Tensor::Tensor;
    Operator(const TensorProd<Operator, Operator>& tot): Tensor{tot} {}
    Operator(const Qubits& qubs): Tensor{Shape{qubs, qubs}} {(*dptr)[0] = 1;}
    Operator(const Qubits& qubs, DataPtr dptr): Tensor{{qubs, qubs}, dptr} { }
    Operator(const Qubits& qubs, const Data& data): Tensor{qubs, qubs, data} { }

};


class State: public Tensor{
public:
    using Tensor::Tensor;
    State& operator=(const State& s){
        dptr = DataPtr{new Data(s.size())};
        shape = s.shape; 
        for (int i=0; i<this->size(); i++){
            (*dptr)[i] = s[i];
        }
        return *this;
    }
    State(const Qubits& qubs): Tensor{{qubs, Qubits()}} {(*dptr)[0] = 1;}
    State(const Qubits& qubs, DataPtr dptr): Tensor{{qubs, Qubits()}, dptr} { }
    State(const Qubits& qubs, const Data& data): Tensor{qubs, Qubits(), data} { }
};


class DensityMatrix: public Tensor{
public:
    using Tensor::Tensor;
    DensityMatrix& operator=(const DensityMatrix& s){
        dptr = DataPtr{new Data(s.size())};
        shape = s.shape; 
        for (int i=0; i<this->size(); i++){
            (*dptr)[i] = s[i];
        }
        return *this;
    }
    DensityMatrix(const Qubits& qubs): Tensor{{qubs, qubs}} {(*dptr)[0] = 1;}
    DensityMatrix(const Qubits& qubs, DataPtr dptr): Tensor{{qubs, qubs}, dptr} { }
    DensityMatrix(const Qubits& qubs, const Data& data): Tensor{qubs, qubs, data} { }
};
// template<>
// class TensorProd<Operator, State>: public Expression<TensorProd<Operator, State>>{
// public:
//     TensorProd(const Operator& exprl, const State& exprr): exprl{&exprl}, exprr{&exprr}, shape{exprr.get_shape()} { }
//     Shape get_shape() const noexcept { return shape; }
//     DataType operator[](int i) const{
//         return 0;
//     }

//     Shape shape;
//     const Operator *exprl;
//     const State *exprr;
// };

std::ostream& operator<<(std::ostream& os, const State& s);
std::ostream& operator<<(std::ostream& os, const Operator& op);

template<typename Operator, typename State>
TensorProd<Operator, State> operator*(const Operator &e1, const State &e2)
{
    return TensorProd<Operator, State>(e1, e2);
}

#endif