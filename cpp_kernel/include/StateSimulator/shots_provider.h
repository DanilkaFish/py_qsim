#ifndef SHOTSSIM
#define SHOTSSIM

#include "state_evolve.h"
#include "python_singleton.h"

State sampler(const State& st, size_t shots){
    RunPython& obj{RunPython::getInstance()};
    py::object numpy_multinomial = py::module_::import("numpy.random");
    
    int size = st.size();
    py::array_t<double> probs_state({size}, new double[size]); 
    DataPtr sampled_vec(new Data(size));
    for (int j =0; j<size; j++){
        probs_state[py::int_(j)] = std::abs(st[j])*std::abs(st[j]);
    }

    py::array_t<int> res = numpy_multinomial.attr("multinomial")(py::int_(shots), probs_state);
    double sqrt_shots = std::sqrt(shots);
    for (int j =0; j<size; j++){
        (*sampled_vec)[j] = std::sqrt(*static_cast<const int *>(res.data(j))/double(shots));
    }
    // std::cerr << "STATE : " << State(st.get_shape(), sampled_vec);
    return State(st.get_shape(), sampled_vec);
}

class ShotsVQE: public StateVQE{

public:
    ShotsVQE(const QuantumCircuit& qc, const Hamiltonian& Ham, size_t shots=1024): StateVQE{qc, Ham}, shots{shots} {}

    virtual double evaluate_cost() override {
        num+=shots;
        state_evolve();
        return Ham.eval(sampler(state, shots));
    }

    void set_shots(size_t shots) {
        this->shots = shots;
    }
    int num = 0; 
private:
    size_t shots;
};

#endif