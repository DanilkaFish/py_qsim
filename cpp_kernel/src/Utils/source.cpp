
#include <stdexcept> 

#include "qubits.hpp"
#include "parameter.hpp"
// ##### Qubits #####
void Qubits::_iterate(std::function<void(int)> func) const{
    Uint t = 1;
    for(int i=0; i < consts::MAX_QUBITS; i++){
        if (_qubs & t ){
            func(i);
        }
        t *= 2;
    }
}

Qubits::Qubits(const std::vector<Uint>& tot): _qubs{0} {
    for(auto i: tot){
        _qubs += 1 << i;
    }
}

Uint Qubits::size() const {
    Uint s = 0;
    _iterate([&s](int i)  {Uint s=0; s++; });
    return s;
}

Str Qubits::to_str() const{ 
    Str s("Qubs("); 
    _iterate([&s](int i){ s = s + std::to_string(i) + ", "; });
    s.pop_back();
    s.pop_back();
    s.push_back(')');
    return s;
}

void Qubits::insert(const Uint& i){
    _qubs = _qubs | (1 << i);
}

Double ExpressionSum::evaluate()  { return lexpr.evaluate() + rexpr.evaluate(); }
Double ExpressionProd::evaluate() { return lexpr.evaluate() * rexpr.evaluate(); }
Double DoubleSum::evaluate()      { return add + expr.evaluate(); }
Double DoubleProd::evaluate()     { return mult * expr.evaluate(); }
Double SinExpression::evaluate()  { return std::sin(expr.evaluate()); }
Double CosExpression::evaluate()  { return std::cos(expr.evaluate()); }
Double PowExpression::evaluate()  { return std::pow(expr.evaluate(), pow); }

void ExpressionProd::backward(Double grad_mult){ lexpr.backward(grad_mult * rexpr.get_argument()); 
                                                 rexpr.backward(grad_mult * lexpr.get_argument()); }
void ExpressionSum::backward(Double grad_mult) { lexpr.backward(grad_mult);  rexpr.backward(grad_mult);}
void DoubleSum::backward(Double grad_mult)     { expr.backward(grad_mult); }
void DoubleProd::backward(Double grad_mult)    { expr.backward(grad_mult*mult); }
void CosExpression::backward(Double grad_mult) { expr.backward(-grad_mult * std::sin(expr.get_argument())); }
void SinExpression::backward(Double grad_mult) { expr.backward( grad_mult * std::cos(expr.get_argument())); }
void PowExpression::backward(Double grad_mult) { expr.backward( grad_mult * pow * std::pow(expr.get_argument(), pow-1)); }

Str ExpressionSum::to_str()  const { return lexpr.to_str() + " + " + rexpr.to_str() ; }
Str ExpressionProd::to_str() const { return lexpr.to_str() + " * " + rexpr.to_str() ; }
Str DoubleSum::to_str()      const { return std::to_string(add) + " + " + expr.to_str() ; }
Str DoubleProd::to_str()     const { return std::to_string(mult) + " * " + expr.to_str() ; }
Str SinExpression::to_str()  const { return "sin(" + expr.to_str() + ")"; }
Str CosExpression::to_str()  const { return "cos(" + expr.to_str() + ")"; }
Str PowExpression::to_str()  const { return "(" + expr.to_str() + ")^" + std::to_string(pow); }


ExpressionContainer operator*(const ExpressionContainer& lExpr, const ExpressionContainer& rExpr){
    return new ExpressionProd(lExpr, rExpr);
}

ExpressionContainer operator*(Double prod, const ExpressionContainer& rExpr){
    return new DoubleProd(prod, rExpr);
}

ExpressionContainer autograd::sin(const ExpressionContainer& expr){
    return new SinExpression(expr);
}

ExpressionContainer autograd::cos(const ExpressionContainer& expr){
    return new CosExpression(expr);
}

ExpressionContainer autograd::pow(const ExpressionContainer& expr, Double pow){
    return new PowExpression(expr, pow);
}

// class VariableVector{
// public:
//     Variable  add_variable(const);
//     Variable  operator[](Uint i);
//     DoubleVec evals() const;
//     Uint      size() const;
//     DoubleVec get_values(const DoubleVec& sv);
//     void      set_values(const DoubleVec& sv);
// private:
//     Str                   mName;
//     Uint                  mCounter=0;
//     std::vector<Variable> mVarVector;
// };

Variable VariableVector::generate_variable(){
    Variable var(mName + std::to_string(size()));
    mVarVector.push_back(var);
    return var;
}

DoubleVec VariableVector::get_values(){
    DoubleVec dvec{};
    for (auto x: mVarVector){
        dvec.push_back(x.get_value());
    }
    return dvec;
}

void VariableVector::set_values(const DoubleVec& dvec){
    if (mVarVector.size() == dvec.size()){
        for (int i=0; i<dvec.size(); i++){
            mVarVector[i].set_value(dvec[i]);
        }
    } else {
        throw std::runtime_error("Different VariableVector and input vector sizes");
    }
}

// ParameterExpr operator*(Double mult, const ParameterExpr& expr){
//     return ParameterProd(mult, expr);
// }

// ParameterExpr operator+(const ParameterExpr& lexpr, const ParameterExpr& rexpr){
//     return ParameterSum(lexpr, rexpr);
// }

// // ##### Parameters #####

// // Parameter& Parameter::operator=(Double angle){
// //     value = angle;
// //     return *this;
// // }

// // ParameterPtr ParameterVector::addParameter() { 
// //     mParVector.push_back(ParameterPtr{new Parameter{mName + "_" + std::to_string(this->size())}});
// //     return mParVector.back(); 
// // }

// // ParameterPtr ParameterVector::operator[](int i) { return mParVector[i]; }

// // Uint ParameterVector::size() const { return mParVector.size(); }

// // std::vector<Double> ParameterVector::getValues() const {
// //     std::vector<Double> rd;
// //     for (auto x: mParVector){
// //         rd.push_back(x->value);
// //     }
// //     return rd;
// // }

// // void ParameterVector::setValues(const std::vector<Double>& vv){
// //     for (int i=0; i<this->size(); i++){
// //         *mParVector[i] = vv[i];
// //     }
// // }