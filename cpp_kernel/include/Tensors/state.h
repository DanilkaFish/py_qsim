#include "tensor.h"


class StateTensor: public Tensor{
public:
    using Tensor::Tensor;
    StateTensor& operator=(const StateTensor& s){
        dptr = DataPtr{new Data(s.size())};
        shape = s.shape; 
        for (int i=0; i<this->size(); i++){
            (*dptr)[i] = s[i];
        }
        return *this;
    }
    static StateTensor from_label(const std::string& str, const Qubits& qubs);
    StateTensor(const Qubits& qubs): Tensor{{qubs, Qubits()}} {(*dptr)[0] = 1;}
    StateTensor(const Qubits& qubs, DataPtr dptr): Tensor{{qubs, Qubits()}, dptr} { }
    StateTensor(const Qubits& qubs, const Data& data): Tensor{qubs, Qubits(), data} { }
};


class MatrixTensor: public Tensor{
        using Tensor::Tensor;
    MatrixTensor& operator=(const MatrixTensor& s){
        dptr = DataPtr{new Data(s.size())};
        shape = s.shape; 
        for (int i=0; i<this->size(); i++){
            (*dptr)[i] = s[i];
        }
        return *this;
    }
    MatrixTensor(const Qubits& qubs): Tensor{{qubs, Qubits()}} {(*dptr)[0] = 1;}
    MatrixTensor(const Qubits& qubs, DataPtr dptr): Tensor{{qubs, qubs}, dptr} { }
    MatrixTensor(const Qubits& qubs, const Data& data): Tensor{qubs, qubs, data} { }
};


