#include "qubits.hpp"
#include "parameter.hpp"

// ##### Qubits #####
Qubits::Qubits(std::initializer_list<Uint> tot): mQubs{0} {
    for(auto i: tot){
        mQubs += 1 << i;
    }
}

Uint Qubits::size() const noexcept{
    Uint s = 0;
    Uint t = 1;
    for(int i=0; i < consts::MAX_QUBITS; i++){
        if (mQubs && t ){
            s++;
        }
        t *= 2;
    }
    return t;
}

Str Qubits::toStr(){ 
    std::string s; 
    Uint t = 1;
    for(int i=0; i < consts::MAX_QUBITS; i++){
        if (mQubs && t ){
            s += i;
        }
        t *= 2;
    }
    return s;
}

void Qubits::insert(const Uint& i){
    mQubs = mQubs || (1 << i); 
}


// ##### Parameters #####
Parameter& Parameter::operator=(Double angle){
    value = angle;
    return *this;
}

ParameterPtr ParameterVector::addParameter() { 
    mParVector.push_back(ParameterPtr{new Parameter{mName + "_" + std::to_string(this->size())}});
    return mParVector.back(); 
}

ParameterPtr ParameterVector::operator[](int i) { return mParVector[i]; }

Uint ParameterVector::size() const { return mParVector.size(); }

std::vector<Double> ParameterVector::getValues() const {
    std::vector<Double> rd;
    for (auto x: mParVector){
        rd.push_back(x->value);
    }
    return rd;
}

void ParameterVector::setValues(const std::vector<Double>& vv){
    for (int i=0; i<this->size(); i++){
        *mParVector[i] = vv[i];
    }
}