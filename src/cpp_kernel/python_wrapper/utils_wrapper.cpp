#define PYBIND11_DETAILED_ERROR_MESSAGES
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

double add(double l, double r){
    return l + r;
}
namespace py = pybind11;
PYBIND11_MODULE(addition, m) {
    m.doc() = "cppsim python interface";
    m.def("add", &add, R"pbdoc(
        Add two numbers

        Some other explanation about the add function.
    )pbdoc"
);

//     py::class_<PauliOperator>(m, "PauliOperator")
//         .def(py::init<std::complex<double>>(), "Constructor",
//             py::arg("coef") = 1. + 0.i)
//         .def(py::init<std::string, std::complex<double>>(), "Constructor",
//             py::arg("pauli_string"),
//             py::arg("coef") = std::complex<double>(1., 0.))
//         .def(py::init<std::vector<UINT>, std::string, std::complex<double>>(),
//             "Constructor", py::arg("target_qubit_index_list"),
//             py::arg("pauli_operator_type_list"), py::arg("coef") = 1. + 0.i)
//         .def("get_index_list", &PauliOperator::get_index_list,
//             "Get list of target qubit indices")
//         .def("get_pauli_id_list", &PauliOperator::get_pauli_id_list,
//             "Get list of Pauli IDs (I,X,Y,Z) = (0,1,2,3)")
//         .def("get_coef", &PauliOperator::get_coef,
//             "Get coefficient of Pauli term")
//         .def("add_single_Pauli", &PauliOperator::add_single_Pauli,
//             "Add Pauli operator to this term", py::arg("index"),
//             py::arg("pauli_type"))
//         .def("get_expectation_value", &PauliOperator::get_expectation_value,
//             "Get expectation value", py::arg("state"))
//         .def("get_expectation_value_single_thread",
//             &PauliOperator::get_expectation_value_single_thread,
//             "Get expectation value", py::arg("state"))
//         .def("get_transition_amplitude",
//             &PauliOperator::get_transition_amplitude,
//             "Get transition amplitude", py::arg("state_bra"),
//             py::arg("state_ket"))
//         .def("copy", &PauliOperator::copy,
//             py::return_value_policy::take_ownership,
//             "Create copied instance of Pauli operator class")
//         .def("get_pauli_string", &PauliOperator::get_pauli_string,
//             "Get pauli string")
//         .def("change_coef", &PauliOperator::change_coef, "Change coefficient",
//             py::arg("new_coef"))
//         .def(py::self * py::self)
//         .def(
//             "__mul__",
//             [](const PauliOperator& a, std::complex<double>& b) {
//                 return a * b;
//             },
//             py::is_operator())
//         .def(py::self *= py::self)
//         .def(
//             "__IMUL__",
//             [](PauliOperator& a, std::complex<double>& b) { return a *= b; },
//             py::is_operator());
}