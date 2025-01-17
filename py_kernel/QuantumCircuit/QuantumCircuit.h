#ifndef _QCINIT
#define _QCINIT

#include "common.h"
#include "drawer.h"
#include <algorithm>


class MetaProvider;
class QC_representation;

class QuantumCircuit{
public:
    using container=std::vector<InstructionPtr>; 
    using iterator=typename container::iterator;
    using const_iterator=typename container::const_iterator;
    using reverse_iterator=typename container::reverse_iterator;
    using const_reverse_iterator=typename container::const_reverse_iterator;
    iterator               begin() { return ins.begin(); }
    iterator               end() { return ins.end(); }
    const_iterator         begin() const { return ins.begin(); }
    const_iterator         end() const { return ins.end(); }
    reverse_iterator       rbegin() { return ins.rbegin(); }
    reverse_iterator       rend() { return ins.rend(); }
    const_reverse_iterator rbegin() const { return ins.rbegin(); }
    const_reverse_iterator rend() const { return ins.rend(); }
    
    QuantumCircuit(): ins{}, qubs{} {}
    QuantumCircuit(int n): ins{}, qubs(n) {}
    ~QuantumCircuit() {  }
    QuantumCircuit(std::initializer_list<InstructionPtr> in);
    QuantumCircuit(const QuantumCircuit& qc): ins{qc.ins}, qubs{qc.qubs} { } 
    Qubits get_qubits() const { return qubs; } 
    // Qubits get_sorted_qubits() const { 
    //     Qubits qubs_copy =qubs;
    //     std::sort(qubs.begin(), qubs.end());
    //     return qubs;
    // } 
    std::size_t size() const { return ins.size(); }
    
    template<class cont>
    QuantumCircuit(const cont& vec): ins{vec} {
        for (auto x : vec){
            for (Qubit qub: x->get_qubits()){
                qubs.insert(qub);
            }
        }
    }

    InstructionPtr    to_instruction();
    QC_representation get_qcr() const;
    QuantumCircuit    decompose();

    void add_instruction(InstructionPtr in);
    void compose(const QuantumCircuit& qc);

private:
    container ins;
    Qubits qubs;
};

namespace BaseInstr{
    InstructionPtr X(Qubit qub);
    InstructionPtr Y(Qubit qub);
    InstructionPtr Z(Qubit qub);
    InstructionPtr I(Qubit qub);
    InstructionPtr S(Qubit qub);
    InstructionPtr H(Qubit qub);
    InstructionPtr Sdag(Qubit qub);
    InstructionPtr CX(Qubit ctrl, Qubit tar);
    InstructionPtr TOF(Qubit ctrl1, Qubit ctrl2, Qubit tar);
    InstructionPtr Rx(Qubit qub, double angle);
    InstructionPtr Ry(Qubit qub, double angle);
    InstructionPtr Rz(Qubit qub, double angle);
    InstructionPtr Rx(Qubit qub, const ExprPtr& angle);
    InstructionPtr Ry(Qubit qub, const ExprPtr& angle);
    InstructionPtr Rz(Qubit qub, const ExprPtr& angle);
    InstructionPtr PR(const PauliString& pauli, double theta);
    InstructionPtr PR(const PauliString& pauli, const ExprPtr& theta);
    // InstructionPtr PR()
    // InstructionPtr U1(Qubit qub, DataPtr dptr);
    // InstructionPtr U2(Qubit qub1, Qubit qub2, DataPtr dptr);
    InstructionPtr U(const Qubits& qubs, DataPtr dptr);
    InstructionPtr U_ordered(const Qubits& qubs, const Data& data);
}
#endif