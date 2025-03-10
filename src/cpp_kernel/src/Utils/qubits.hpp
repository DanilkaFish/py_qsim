#include "typedef.hpp"
#include <string>
#include <iostream>
int a = 1<<3;

class Qubits{
    explicit Qubits(Uint n): mQubs{(1<<n) - 1} {}
    Qubits(std::initializer_list<Uint> tot);
    void insert(const Uint& i);
    Uint size() const noexcept;
    Uint getQubits(){ return mQubs; }
    Str  toStr();
private:
    Uint mQubs;
};

