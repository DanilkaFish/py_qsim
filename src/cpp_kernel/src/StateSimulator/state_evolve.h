#ifndef _GATESIM
#define _GATESIM
#include <string>
#include <utility>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>

#include "common.h"
#include "instruction.h"
#include "tensorspec.h"
// #include "QuantumCircuit.h"
    
class Operator;
class State;

constexpr int num_thread=1;
// class Param
void _fill(int init, int finit, int dif, const Data& l, Data& r, const mask& buffer_mask, const std::vector<int>& expand);


class BaseTensorProvider: public MetaProvider{
public:
    using MetaProvider::MetaProvider;
    void I(const Qubits& qubs) override;
    void X(const Qubits& qubs) override;
    void Y(const Qubits& qubs) override;
    void Z(const Qubits& qubs) override;
    void CX(const Qubits& qubs) override ;
    void TOF(const Qubits& qubs ) override;
    void H(const Qubits& qubs) override ;
    void S(const Qubits& qubs) override ;
    void Sdag(const Qubits& qubs) override ;
    void U(const Qubits& qubs, DataPtr dptr) override ;
    void U(const Qubits& qubs, const Data& data) override ;
    void PR(const PauliString& ps, double dptr) override;
    void Rx(const Qubits& qubs, double dptr) override;
    void Ry(const Qubits& qubs, double dptr) override;
    void Rz(const Qubits& qubs, double dptr) override;
    virtual void SetUp() = 0;
    virtual double evaluate_cost() = 0 ;
protected:
    virtual void inplace_evolve(const Operator& op) = 0;
    // virtual void inplace_sparse_evolve(std::) = 0;
};


class StateProvider: public BaseTensorProvider{
public:
    StateProvider(const QuantumCircuit& qc): BaseTensorProvider{qc} { 
        init_state = State(qc.get_qubits()); 
    }
    void CX(const Qubits& qubs) override ;
    // void TOF(const Qubits& qubs ) override;
    // void S(const Qubits& qubs) override ;
    // void Sdag(const Qubits& qubs) override ;
    virtual void SetUp() override { state = init_state; }
    virtual double evaluate_cost() override { return 0; };
    State& get_evolved_state() { return state; }
    void set_init_state(State& state) {
        init_state = state;
    }

protected:
    void inplace_evolve(const Operator& op) override;
    State init_state;
    State state;
};


class Hamiltonian{
public:
    Hamiltonian()=default;
    virtual double eval(const State& state) const {return 0; };
};

class HamOpSet: public Hamiltonian{
public:
    virtual double eval(const State& state) const override;
    std::vector<Operator> svec;
};

class HamDiag: public Hamiltonian{
public:
    HamDiag(const State& state): diag_ham_state{state} {};
    virtual double eval(const State& state) const override;
    State diag_ham_state;
};



class StateVQE: public StateProvider{
public:
    StateVQE(const QuantumCircuit& qc, const Hamiltonian& Ham): StateProvider(qc), Ham{Ham} {}
    virtual double evaluate_cost() override { num++; state_evolve(); return Ham.eval(state); }
    int num = 0; 
protected:
    const Hamiltonian& Ham;
};


// class DensityMatrixProvider: public BaseTensorProvider{
// public:
//     DensityMatrixProvider(const QuantumCircuit& qc): BaseTensorProvider{qc} { 
//         state = DensityMatrix(qc.get_qubits()); 
//     }

//     virtual void SetUp() override { state = init_state; }
//     virtual double evaluate_cost() {};
//     DensityMatrix& get_evolved_state() { return state; }
//     DensityMatrix init_state;
// protected:
//     void inplace_evolve(const Operator& op) override;
//     DensityMatrix state;
// };


// class DensityMatrixVQE: public DensityMatrixProvider{
//     DensityMatrixVQE(const QuantumCircuit& qc, const Operator& Ham): DensityMatrixProvider(qc), Ham{Ham} {}
//     virtual double evaluate_cost() override { 
//         DataType energy = (state.conj()*Ham*state)[0];
//         if (std::abs(std::imag(energy)) > 0.0000000000001){
//             throw QException("Hamiltonian is not hermitian");
//         }
//         return std::real(energy); 
//     }
// private:
//     const Operator& Ham;
// };


namespace bm{
    Operator X(Qubit);
    Operator Y(Qubit);
    Operator Z(Qubit);
    Operator I(Qubit);
    Operator I(const Qubits& qubs);
    Operator H(Qubit);
    Operator S(Qubit);
    Operator Sdag(Qubit);
    Operator CX(Qubit trg, Qubit ctrl);
    Operator TOF(Qubit ctrl1, Qubit ctrl2, Qubit trg);
    Operator PR(const PauliString& pauli, double theta);
    Operator Rx(Qubit qub, double theta);
    Operator Ry(Qubit qub, double theta);
    Operator Rz(Qubit qub, double theta);
};

#endif