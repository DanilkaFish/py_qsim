from cpp_kernel import Qubits, Instruction, State
from cpp_kernel import QException

from __future__ import annotations
#     public:
#     using container=std::vector<InstructionPtr>; 
#     using iterator=typename container::iterator;
#     using const_iterator=typename container::const_iterator;
#     using reverse_iterator=typename container::reverse_iterator;
#     using const_reverse_iterator=typename container::const_reverse_iterator;
#     iterator               begin() { return ins.begin(); }
#     iterator               end() { return ins.end(); }
#     const_iterator         begin() const { return ins.begin(); }
#     const_iterator         end() const { return ins.end(); }
#     reverse_iterator       rbegin() { return ins.rbegin(); }
#     reverse_iterator       rend() { return ins.rend(); }
#     const_reverse_iterator rbegin() const { return ins.rbegin(); }
#     const_reverse_iterator rend() const { return ins.rend(); }
    
#     QuantumCircuit(): ins{}, qubs{} {}
#     QuantumCircuit(int n): ins{}, qubs(n) {}
#     ~QuantumCircuit() {  }
#     QuantumCircuit(std::initializer_list<InstructionPtr> in);
#     QuantumCircuit(const QuantumCircuit& qc): ins{qc.ins}, qubs{qc.qubs} { } 
#     Qubits get_qubits() const { return qubs; } 
#     // Qubits get_sorted_qubits() const { 
#     //     Qubits qubs_copy =qubs;
#     //     std::sort(qubs.begin(), qubs.end());
#     //     return qubs;
#     // } 
#     std::size_t size() const { return ins.size(); }
    
#     template<class cont>
#     QuantumCircuit(const cont& vec): ins{vec} {
#         for (auto x : vec){
#             for (Qubit qub: x->get_qubits()){
#                 qubs.insert(qub);
#             }
#         }
#     }

#     InstructionPtr    to_instruction();
#     QC_representation get_qcr() const;
#     QuantumCircuit    decompose();

#     void add_instruction(InstructionPtr in);
#     void compose(const QuantumCircuit& qc);

# private:
#     container ins;
#     Qubits qubs;
# };
class QuantumCircuit:
    def __init__(self, n: Qubits|int, qc: QuantumCircuit = None):
        self._qubits = n
        self._instructions = []
        if qc is not None:
            self._instructions = Instruction(qc)
        self.init_state = State.from_label("0"*n)

    @property
    def qubits(self):
        return self._qubits
    
    @property
    def instructions(self):
        return self._instructions
    
    @property 
    def init_state(self):
        return self._init_state
    
    @init_state.setter
    def init_state(self, state):
        if state.qubits == self._qubits:
            self._init_state = state
        else:
            raise QException("Wrong qubits set in init_state setter")

    def decompose(self):
        pass
    
    def compose(qc: QuantumCircuit):
        pass

