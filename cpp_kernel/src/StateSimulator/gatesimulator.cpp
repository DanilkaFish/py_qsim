#include "state_evolve.h"


Operator bm::I(const Qubits& qubs){
    int n = 1 << qubs.size();
    DataPtr dptr(new Data((n*n), 0));
    for(int i=0; i < n; i++){
        (*dptr)[i*n + i] = 1;
    }
    return Operator(qubs, dptr);
}

Operator bm::I(Qubit qub){
    static DataPtr dptr{new Data{1,0,
                                 0,1}};
    return Operator({qub}, dptr);
}

Operator bm::X(Qubit qub){
    static DataPtr dptr(new Data{0,1,
                                 1,0});
    return Operator({qub}, dptr);
}

Operator bm::Y(Qubit qub){
    static DataPtr dptr{new Data{0,{0,-1},
                                {0,1},0}};
    return Operator({qub}, dptr);
}

Operator bm::Z(Qubit qub){
    static DataPtr dptr{new Data{1,0,
                                 0,-1}};
    return Operator({qub}, dptr);
}

Operator bm::S(Qubit qub){
    static DataPtr dptr{new Data{1,0,
                                 0,{0,1}}};
    return Operator({qub}, dptr);
}

Operator bm::Sdag(Qubit qub){
    static DataPtr dptr{new Data{1,0,
                                 0,{0,-1}}};
    return Operator({qub}, dptr);
}

Operator bm::CX(Qubit trg, Qubit ctrl){
    static DataPtr dptr{new Data{1,0,0,0,
                                 0,1,0,0,
                                 0,0,0,1,
                                 0,0,1,0}};
    static DataPtr dptrinv{new Data{1,0,0,0,
                                 0,0,0,1,
                                 0,0,1,0,
                                 0,1,0,0}};
    if (trg > ctrl){
        return Operator({trg, ctrl}, dptr);
    }else{
        return Operator({trg, ctrl}, dptrinv);
    }
}

const double A = 1.0/sqrt(2);

Operator bm::H(Qubit qub){
    static DataPtr dptr{new Data{A, A,
                                 A,-A}};
    return Operator({qub}, dptr);
}

Operator bm::TOF(Qubit ctrl1, Qubit ctrl2, Qubit trg ){
    static Data data{1,0,0,0,0,0,0,0,
                    0,1,0,0,0,0,0,0,
                    0,0,1,0,0,0,0,0,
                    0,0,0,0,0,0,0,1,
                    0,0,0,0,1,0,0,0,
                    0,0,0,0,0,1,0,0,
                    0,0,0,0,0,0,1,0,
                    0,0,0,1,0,0,0,0};
    return Operator({ctrl1, ctrl2, trg}, data);
}

Operator bm::Rx(Qubit qub, double angle){
    DataType c = cos(angle/2);
    DataType is = i::i*sin(angle/2);
    return Operator({qub}, Data{c,is,is,c});
}

Operator bm::Ry(Qubit qub, double angle){
    DataType c = cos(angle/2);
    DataType s = sin(angle/2);
    return Operator({qub}, Data{c, -s, s,c});
}
Operator bm::Rz(Qubit qub, double angle){
    DataType c = cos(angle/2);
    DataType is = i::i*sin(angle/2);
    return Operator({qub}, Data{c + is, 0, 0, c - is});
}

Operator bm::PR(const PauliString& pauli, double theta){
    Tensor T(1);
    for(int i=0; i<pauli.size(); i++){
        switch (tolower(pauli.get_ch(i)))
        {
        case 'x':
            T = T*bm::X(pauli.get_qn(i));
            break;
        case 'y':
            T = T*bm::Y(pauli.get_qn(i));
            break;
        case 'z':
            T = T*bm::Z(pauli.get_qn(i));
            break;
        case 'i':
            T = T*bm::I(pauli.get_qn(i));
            break;
        default:
            throw QException("wrong pauli string name :" + pauli.get_ch(i));
        }
    }
    return Operator{Tensor{cos(theta/2)}*bm::I(pauli.get_qubs()) + Tensor{i::i*sin(theta/2)}*T};
}

void BaseTensorProvider::I(const Qubits& qubs){
    inplace_evolve(bm::I(qubs[0]));
}
void BaseTensorProvider::X(const Qubits& qubs){
    inplace_evolve(bm::X(qubs[0]));
}
void BaseTensorProvider::Y(const Qubits& qubs){
    inplace_evolve(bm::Y(qubs[0]));
}
void BaseTensorProvider::Z(const Qubits& qubs){
    inplace_evolve(bm::Z(qubs[0]));
}
void BaseTensorProvider::H(const Qubits& qubs){
    inplace_evolve(bm::H(qubs[0]));
}
void BaseTensorProvider::S(const Qubits& qubs){
    inplace_evolve(bm::S(qubs[0]));
}
void BaseTensorProvider::Sdag(const Qubits& qubs){
    inplace_evolve(bm::Sdag(qubs[0]));
}
void BaseTensorProvider::CX(const Qubits& qubs){
    inplace_evolve(bm::CX(qubs[0], qubs[1]));
}
void BaseTensorProvider::TOF(const Qubits& qubs){
    inplace_evolve(bm::TOF(qubs[0], qubs[1], qubs[2]));
}

void BaseTensorProvider::PR(const PauliString& ps, double angle){
    inplace_evolve(bm::PR(ps, angle));
}
void BaseTensorProvider::Rx(const Qubits& qubs, double dptr){
    inplace_evolve(bm::Rx(qubs[0], dptr));
}
void BaseTensorProvider::Ry(const Qubits& qubs, double dptr){
    inplace_evolve(bm::Ry(qubs[0], dptr));
}
void BaseTensorProvider::Rz(const Qubits& qubs, double dptr){
    inplace_evolve(bm::Rz(qubs[0], dptr));
}

void BaseTensorProvider::U(const Qubits& qubs, DataPtr dptr){
    inplace_evolve(Operator{qubs, dptr});
}

void BaseTensorProvider::U(const Qubits& qubs, const Data& data){
    inplace_evolve(Operator{qubs, data});
}


void StateProvider::CX(const Qubits& qubs){
    // BaseTensorProvider::CX(qubs);
    int ctrl = qubs[0].num;
    int targ = qubs[1].num;
    Data& data = *(state.get_dptr().get());

    int size = data.size();
    DataType index = 0;
    int s_min;
    int s_max;
    if (targ > ctrl){
        s_min = 1<<(ctrl+1);
        s_max = 1<<(targ+1);
    }else{
        s_min = 1<<(targ+1);
        s_max = 1<<(ctrl+1);
    }

    for (int i=0; i<s_min/2; i++){
        for (int j=i ; j<s_max/2; j+=s_min){
            for (int k=j + (1<<ctrl); k<size; k+=s_max){
                std::swap(data[k], data[k + (1 << targ)]);
            }
        }
    }
}
// void StateProvider::TOF(const Qubits& qubs ){
//     int ctrl = qubs[0].num;
//     int ctrl = qubs[1].num;
//     int targ = qubs[2].num;
//     Data& data = *(state.get_dptr().get());

//     int size = data.size();
//     int index = 0;
//     int s_min;
//     int s_max;
//     if (targ > ctrl){
//         s_min = 1<<ctrl;
//         s_max = 1<<targ;
//     }else{
//         s_min = 1<<targ;
//         s_max = 1<<ctrl;
//     }
//     for (int i=0; i<s_min; i++){
//         for (int j=0; i<(s_min-s_max)/2; j++){
//             for (int k=0; k<(size-s_max)/2; k++){
//                 index = i + j*s_min*2 + k*s_max*2 + 1<<ctrl;
//                 std::swap(data[index], data[index + (1 << targ)]);
//             }
//         }
//     }
// }
// void StateProvider::S(const Qubits& qubs){

// }
// void StateProvider::Sdag(const Qubits& qubs){

// }

inline void _fill(int init, int finit, int dif, const Data& l, Data& r, const mask& buffer_mask, const std::vector<int>& expand){
    int buffer_i;
    DataType val;
    // int cache_size = 1 << 4; 
    std::vector<DataType> bufferr(dif);
    for (int i=init; i<finit; i++){
        buffer_i = buffer_mask.expand(i);

        for(int up_index=0; up_index<dif; up_index++){
            bufferr[up_index] = r[buffer_i + expand[up_index]];
        }

        for(int up_index=0; up_index<dif; up_index++){
            val = 0;
            for (int down_index=0; down_index<dif; down_index++){
                val += l[up_index*dif + down_index] * bufferr[down_index];
            }
            r[buffer_i + expand[up_index]] = val;
        }
    }
}; 


double HamOpSet::eval(const State& state) const { 
    State state_copy = Operator(0)*state;
    for (auto op: svec){
        state_copy = state_copy + op*state;
    }
    DataType energy = (state.conj()*state_copy)[0];
    if (std::abs(std::imag(energy)) > 0.0000000000001){
        throw QException("Hamiltonian is not hermitian");
    }
    return std::real(energy); 
}

double HamDiag::eval(const State& state) const { 
    int size = state.size();
    DataPtr dptr(new Data(size));
    for (int i=0; i<size; i++){
        (*dptr)[i] = std::conj(state[i])*state[i];
    }
    DataType energy = (this->diag_ham_state.conj()*State{state.get_shape(), dptr})[0];
    if (std::abs(std::imag(energy)) > 0.0000000000001){
        throw QException("Hamiltonian is not hermitian");
    }
    return std::real(energy); 
}

void StateProvider::inplace_evolve(const Operator& op){
    mask buffer_mask(state.get_shape().pos_up.compress(state.get_shape().pos_up.msk()) ^ 
                        state.get_shape().pos_up.compress(op.get_shape().pos_down.msk()));
    int dif = 1 << index_sum(op.get_shape().pos_down.msk());
    std::vector<int> expand;
    mask op_mask(state.get_shape().pos_up.compress(op.get_shape().pos_down.msk()));
    for (int j=0; j < (dif); j++){
        expand.push_back(op_mask.expand(j));
    }
    _fill(0, 
          state.get_dptr()->size() / dif, 
          dif,
          std::cref(*(op.get_dptr().get())), 
          std::ref(*(state.get_dptr().get())),
          std::cref(buffer_mask), 
          std::cref(expand)
         );
    // std::vector<std::thread> threads(num_thread);
    // int size = state.get_dptr()->size() / dif / num_thread;
    // for (int j=0; j<num_thread; j++){
    //     threads[j] = std::thread(_fill, size*j, 
    //                                     size*(j+1), 
    //                                     dif,
    //                                     std::cref(*(op.get_dptr().get())), 
    //                                     std::ref(*(state.get_dptr().get())),
    //                                     std::cref(buffer_mask), 
    //                                     std::cref(expand));
    // }
    // for (int j=0; j<num_thread; j++){
    //     threads[j].join();
    // }
}

std::ostream& operator<<(std::ostream& os, const State& s){
    int size = s.get_shape().size();
    for(int i=0; i<s.size(); i++){
        if (s[i].imag() != 0 || s[i].real() != 0){
            os << '+' << s[i] << "|";
            int j = i;
            for(int pos=0; pos<size; pos++){
                os << (j>>(size-pos-1) & 1);
                // j = j >> 1;
            }   
            os << "> ";
        }
    }
    return os << "\n";
}

std::ostream& operator<<(std::ostream& os, const Operator& op){
    os << "[ ";
    int size = 1 << op.get_shape().nu;
    for(int i = 0; i < size; i++ ){
        for(int j = 0; j < size; j++ ){
            os << op[i*size + j] << " ";
        }
        os << std::endl;
    }
    return os << "]\n";
}


