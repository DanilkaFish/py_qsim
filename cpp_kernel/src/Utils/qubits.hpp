#pragma once


#include "typedef.hpp"
#include <string>
#include <iostream>
#include <functional>

struct Qubs{
    explicit Qubs(Uint qubs): qubs{qubs} {}
    Uint qubs;    
};

class Qubits{
public:
    explicit Qubits(Uint n): _qubs{Uint((1<<n) - 1)} {}
    explicit Qubits(Qubs n): _qubs{n.qubs} {}
    // explicit Qubits(mask)
    Qubits(const std::vector<Uint>& tot);
    void insert(const Uint& i);
    Uint size() const ;
    Uint get_qubits() const { return _qubs; }
    Str  to_str() const;
private:
    Uint _qubs;
    void _iterate(std::function<void(int)> func) const;
};

