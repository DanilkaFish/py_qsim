#pragma once

#include "typedef.hpp"
#include <string>
#include <iostream>


class Qubits{
public:
    explicit Qubits(Uint n): mQubs{(1<<n) - 1} {}
    Qubits(std::initializer_list<Uint> tot);
    void insert(const Uint& i);
    Uint size() const ;
    Uint getQubits(){ return mQubs; }
    Str  toStr();
private:
    Uint mQubs;
};

