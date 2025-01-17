#include "QuantumCircuit.h"
#include "instruction.h"
#include "drawer.h"

#include <utility>

constexpr double pi = 3.141592653589;
// TODO
void validate_in_range(Qubits all, Qubits in){}

InstructionPtr makeInsPtr(Instruction *in){
    return std::shared_ptr<Instruction>(in);
}

InstructionPtr QuantumCircuit::to_instruction(){
    return makeInsPtr(new MagicInstruction(*this));
}

QuantumCircuit Instruction::decompose(const InstructionPtr& that) {
    return QuantumCircuit({that});
}

QuantumCircuit MagicInstruction::decompose(const InstructionPtr& that)  {
    return this->qc;
}

void MetaProvider::apply_instructions() {
    SetUp();
    for(auto ins: qc){
        ins->apply(*this);
    }
}

std::string instruction_repr(const InstructionPtr& inptr){
    return inptr->get_name() + ":" +  inptr->get_qubits().to_str() + ";";
}

QC_representation QuantumCircuit::get_qcr() const {
    QC_representation toto;
    toto.qubits = qubs;
    for(const auto ops: ins){
        toto.s = toto.s + instruction_repr(ops);
    }
    return toto;
}

void QuantumCircuit::add_instruction(InstructionPtr in) { 
    ins.push_back(in); 
    for (Qubit x: in->get_qubits()){
        qubs.insert(x);
    }
}


QuantumCircuit::QuantumCircuit(std::initializer_list<InstructionPtr> in) : ins{in} {
    for (auto x: ins){
        for(Qubit qub: x->get_qubits()){
            qubs.insert(qub);
        }
    }
}


void QuantumCircuit::compose(const QuantumCircuit& qc){
    add_instruction(InstructionPtr{new MagicInstruction(qc)});
    for (Qubit x: qc.get_qubits()){
        qubs.insert(x);
    }
}


QuantumCircuit QuantumCircuit::decompose() {
    QuantumCircuit qc(qubs.size());
    for(auto& x: (*this)){
        for (auto& y: x->decompose(x)){
            qc.add_instruction(y);
        }
    }
    return qc;
};


InstructionPtr BaseInstr::X(Qubit qub) { return InstructionPtr{ new _X{{qub}}}; }
InstructionPtr BaseInstr::Y(Qubit qub) { return InstructionPtr{ new _Y{{qub}}}; }
InstructionPtr BaseInstr::Z(Qubit qub) { return InstructionPtr{ new _Z{{qub}}}; }
InstructionPtr BaseInstr::I(Qubit qub) { return InstructionPtr{ new _I{{qub}}}; }
InstructionPtr BaseInstr::S(Qubit qub) { return InstructionPtr{ new _S{{qub}}}; }
InstructionPtr BaseInstr::H(Qubit qub) { return InstructionPtr{ new _H{{qub}}}; }
InstructionPtr BaseInstr::Sdag(Qubit qub) { return InstructionPtr{ new _Sdag{{qub}}}; }
InstructionPtr BaseInstr::CX(Qubit ctrl, Qubit tar) { return InstructionPtr{ new _CX{{ctrl, tar}}}; }
InstructionPtr BaseInstr::TOF(Qubit ctrl1, Qubit ctrl2, Qubit tar) { return InstructionPtr{ new _TOF{Qubits({ctrl1, ctrl2, tar})}}; }
InstructionPtr BaseInstr::Rz(Qubit qub, const ExprPtr& angle) { return InstructionPtr{ new _Rz{{qub}, angle}};}
InstructionPtr BaseInstr::Rz(Qubit qub, double theta) { return InstructionPtr{ new _Rz{{qub},  ExprPtr(new ParameterConst(theta))}};}
InstructionPtr BaseInstr::Ry(Qubit qub, const ExprPtr& angle) { return InstructionPtr{ new _Ry{{qub}, angle}};}
InstructionPtr BaseInstr::Ry(Qubit qub, double theta) { return InstructionPtr{ new _Ry{{qub},  ExprPtr(new ParameterConst(theta))}};}
InstructionPtr BaseInstr::Rx(Qubit qub, const ExprPtr& angle) { return InstructionPtr{ new _Rx{{qub}, angle}};}
InstructionPtr BaseInstr::Rx(Qubit qub, double theta) { return InstructionPtr{ new _Rx{{qub},  ExprPtr(new ParameterConst(theta))}};}
InstructionPtr BaseInstr::PR(const PauliString& ps, double theta){ return InstructionPtr{ new _PR{ps, ExprPtr(new ParameterConst(theta))}}; };
InstructionPtr BaseInstr::PR(const PauliString& ps, const ExprPtr& theta){ return InstructionPtr{ new _PR{ps, theta}}; };
InstructionPtr BaseInstr::U(const Qubits& qubs, DataPtr dptr) { return InstructionPtr{ new _U{qubs, dptr}}; } 
InstructionPtr BaseInstr::U_ordered(const Qubits& qubs, const Data& data) { return InstructionPtr{ new _U_ordered{qubs, data}}; } 


// QuantumCircuit _TOF::decompose(const InstructionPtr& ip){
//     QuantumCircuit qc;
//     Qubits qubs = ip->get_qubits();
//     Qubit q0 = qubs[0];
//     Qubit q1 = qubs[1];
//     Qubit q2 = qubs[2];
//     // Hadamard(wires=[2]),
//     //      CNOT(wires=[1, 2]),
//     //      Adjoint(T(wires=[2])),
//     //      CNOT(wires=[0, 2]),
//     //      T(wires=[2]),
//     //      CNOT(wires=[1, 2]),
//     //      Adjoint(T(wires=[2])),
//     //      CNOT(wires=[0, 2]),
//     //      T(wires=[2]),
//     //      T(wires=[1]),
//     //      CNOT(wires=[0, 1]),
//     //      Hadamard(wires=[2]),
//     //      T(wires=[0]),
//     //      Adjoint(T(wires=[1])),
//     //      CNOT(wires=[0, 1])]
//     qc.add_instruction(BaseInstr::H(q2));
//     qc.add_instruction(BaseInstr::CX(q1,q2));
//     qc.add_instruction(BaseInstr::Rz(q2, -pi/4));
//     qc.add_instruction(BaseInstr::CX(q0,q2));
//     qc.add_instruction(BaseInstr::Rz(q2, pi/4));
//     qc.add_instruction(BaseInstr::CX(q1,q2));
//     qc.add_instruction(BaseInstr::Rz(q2, -pi/4));
//     qc.add_instruction(BaseInstr::CX(q0,q2));
//     qc.add_instruction(BaseInstr::Rz(q2, pi/4));
//     qc.add_instruction(BaseInstr::Rz(q1, pi/4));
//     qc.add_instruction(BaseInstr::CX(q0,q1));
//     qc.add_instruction(BaseInstr::H(q2));
//     qc.add_instruction(BaseInstr::Rz(q0, pi/4));
//     qc.add_instruction(BaseInstr::Rz(q1, -pi/4));
//     qc.add_instruction(BaseInstr::CX(q0,q1));
//     return qc;
// }

QuantumCircuit _PR::decompose(const InstructionPtr& x){
    QuantumCircuit qc;
    // preparation layer
    for(int i=0; i<ps.size(); i++){
        switch (tolower(ps.get_ch(i)))
        {
        case 'x':
            qc.add_instruction(BaseInstr::H(ps.get_qn(i)));
            break;
        case 'y':
            qc.add_instruction(BaseInstr::Sdag(ps.get_qn(i)));
            qc.add_instruction(BaseInstr::H(ps.get_qn(i)));
            break;
        case 'z':
            break;
        default:
            break;
            // custom_error("wrong pauli string format");
        }
    }
    // cnot left layer
    for(int i=ps.size() - 1; i>0; i--){
        qc.add_instruction(BaseInstr::CX(ps.get_qn(i), ps.get_qn(i-1 )));
    }
    // z rotation
    qc.add_instruction( BaseInstr::Rz(ps.get_qn(0), angle));

    // cnot right layer
    for(int i=0; i<ps.size() - 1; i++){
        qc.add_instruction(BaseInstr::CX(ps.get_qn(i+1), ps.get_qn(i)));
    }
    for(int i=0; i<ps.size(); i++){
        switch (tolower(ps.get_ch(i)))
        {
        case 'x':
            qc.add_instruction(BaseInstr::H(ps.get_qn(i)));
            break;
        case 'y':
            qc.add_instruction(BaseInstr::H(ps.get_qn(i)));
            qc.add_instruction(BaseInstr::S(ps.get_qn(i)));
            break;
        case 'z':
            break;
        default:
            break;
            // custom_error("wrong pauli string format");
        }
    }
    return qc;
}
// InstructionPtr BaseInstr::U(Qubits qubs, DataPtr dptr) { return InstructionPtr{ new _U{qubs, dptr}}; }


