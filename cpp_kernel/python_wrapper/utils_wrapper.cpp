#define PYBIND11_DETAILED_ERROR_MESSAGES
// #include <pybind11/complex.h>
// #include <pybind11/functional.h>
// #include <pybind11/operators.h>
#include "typedef.hpp"
#include "parameter.hpp"
#include "Utils/qubits.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
// #include <pybind11/common.h>

// class VariableExpr{

// };

namespace py = pybind11;

// class VariableWrapper{
// public:
//     VariableWrapper(const Variable& vptr): vptr{vptr} {}
//     operator const Expression::Ptr() {}
// private:
//     Variable vptr;
// }

PYBIND11_MODULE(MODULE_NAME, m) {
    m.doc() = "cppsim python Interface";
    m.attr("__version__") = "0.0.1";

    py::class_<Qubits>(m, "Qubits")
        .def(py::init<Uint>(), "Constructor",
            py::arg("n_qubits")=0)
        .def_static("from_list",  [](const py::iterable& lst) {
            std::vector<Uint> ids ;
            for (auto&& item : lst) {
                ids.push_back(item.cast<Uint>());
            }
            return Qubits(ids);
        },
        "Qubits generator from iterable"
        )
        .def_property_readonly("to_int", &Qubits::get_qubits)
        .def("__str__", &Qubits::to_str, "String representation")
        .def("insert", &Qubits::insert, "insert Qubit",
            py::arg("qubit_num"));

    py::class_<ExpressionContainer>(m, "Expression")
        .def(py::init<Variable>())
        .def(py::init<Double>())
        .def("evaluate", &ExpressionContainer::evaluate)
        .def("backward", &ExpressionContainer::grad_eval)
        .def("__str__", &ExpressionContainer::to_str)
        .def("__rmul__", 
            [](const ExpressionContainer &expr, Double a){ 
                return ExpressionContainer(new DoubleProd(a, expr));
            }, py::is_operator())
        .def("__mul__", 
            [](const ExpressionContainer &expr, Double a){ 
                return ExpressionContainer(new DoubleProd(a, expr));
            }, py::is_operator())
        .def("__mul__", 
            [](const ExpressionContainer &lexptr, const ExpressionContainer &rexptr){ 
                return ExpressionContainer(new ExpressionProd(lexptr, rexptr));
            }, py::is_operator())
        .def("__radd__", 
            [](const ExpressionContainer &var, Double a){ 
                return ExpressionContainer(new DoubleSum(a, var));
            }, py::is_operator())
        .def("__add__", 
            [](const ExpressionContainer &var, Double a){ 
                return ExpressionContainer(new DoubleSum(a, var));
            }, py::is_operator())
        .def("__add__", 
            [](const ExpressionContainer &lexptr, const ExpressionContainer &rexptr){ 
                return ExpressionContainer(new ExpressionSum(lexptr, rexptr));
            }, py::is_operator())
        .def("__rsub__", 
            [](const ExpressionContainer &var, Double a){ 
                return ExpressionContainer(new DoubleSum(a, (-1)*var));
            }, py::is_operator())
        .def("__sub__", 
            [](const ExpressionContainer &var, Double a){ 
                return ExpressionContainer(new DoubleSum(-a, var));
            }, py::is_operator())
        .def("__sub__", 
            [](const ExpressionContainer &lexptr, const ExpressionContainer &rexptr){ 
                return ExpressionContainer(new ExpressionSum(lexptr, (-1)*rexptr));
            }, py::is_operator())
        ;
        
    py::implicitly_convertible<Variable, ExpressionContainer>();

    py::class_<Variable>(m, "Variable")
        .def(py::init<Str, Double>(), "Constructor",
            py::arg("to_str") = "t",
            py::arg("value") = 0
        )
        .def("__rmul__", [](const Variable &var, Double a) {
            return ExpressionContainer(new DoubleProd(a, var));
        }, py::is_operator())
        .def("__mul__", [](const Variable &var, Double a) {
            return ExpressionContainer(new DoubleProd(a, var));
        }, py::is_operator())
        .def("__mul__", [](const Variable &var, const ExpressionContainer &rexpr) {
            return ExpressionContainer(new ExpressionProd(var, rexpr));
        }, py::is_operator())
        .def("__radd__", [](const Variable &var, Double a) {
            return ExpressionContainer(new DoubleSum(a, var));
        }, py::is_operator())
        .def("__add__", [](const Variable &var, Double a) {
            return ExpressionContainer(new DoubleSum(a, var));
        }, py::is_operator())
        .def("__add__", [](const Variable &var, const ExpressionContainer &rexpr) {
            return ExpressionContainer(new ExpressionSum(var, rexpr));
        }, py::is_operator())
        .def("__rsub__", [](const Variable &var, Double a) {
            return ExpressionContainer(new DoubleSum(a, (-1)*var));
        }, py::is_operator())
        .def("__sub__", [](const Variable &var, Double a) {
            return ExpressionContainer(new DoubleSum(-a, var));
        }, py::is_operator())
        .def("__sub__", [](const Variable &var, const ExpressionContainer &rexpr) {
            return ExpressionContainer(new ExpressionSum(var, (-1)*rexpr));
        }, py::is_operator())

        .def_property_readonly("name", &Variable::to_str)
        .def_property("value", &Variable::get_value, &Variable::set_value)
        .def("get_grad", &Variable::get_backward)
        // .def("backward", &Variable::backward)
        .def("__repr__", &Variable::to_str)
        .def("__str__",
            [](const Variable &a) {
                return a.to_str() + " = " + std::to_string(a.get_value());
            }
        );

    py::class_<VariableVector>(m, "VariableVector")
        .def(py::init<Uint, Str>(), "Constructor",
            py::arg("size") = 0,
            py::arg("name") = "t"
        )
        .def("__getitem__", &VariableVector::operator[])
        .def("__setitem__", &VariableVector::operator[])
        .def("set_values", &VariableVector::set_values) 
        .def("__len__", &VariableVector::size)
        // .def("get_values", &VariableVector::get_values) 
        .def("generate_variable", &VariableVector::generate_variable) 
        ;

    m.def("sin", &autograd::sin, "sin of expression");
    m.def("cos", &autograd::cos, "cos of expression");
    m.def("pow", &autograd::pow, "pow of expression");
    
}