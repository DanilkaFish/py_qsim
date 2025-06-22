#pragma once

#include "typedef.hpp"
#include <math.h>


class Expression{
public:
    using Ptr = std::shared_ptr<Expression>;
    virtual void   backward(Double grad_input)  = 0;
    virtual Double evaluate() = 0;
    virtual Str    to_str() const = 0;
};


struct VariableData{
    using Ptr = std::shared_ptr<VariableData>;
    Double   grad;
    Double   value;
    Str      to_str;
};


class Variable{
public:
    using VariablePtr = std::shared_ptr<Variable>;
    explicit Variable(Str to_str="t", Double val=0): 
        data(new VariableData{0, val, to_str}) {}
    Str    to_str() const { return data->to_str; }
    Double get_value() const { return data->value; }
    void   set_value(Double val) { data->value = val; }
    Double get_backward() const { return data->grad; }
    void   update_backward(Double grad_eval) { data->grad += grad_eval; }
    void   clear_gradient() { data->grad = 0;  }
private:
    VariableData::Ptr data;
};


class VariableExpression : public Expression{
public:
    VariableExpression(const Variable& var): var{var} {}
    virtual void   backward(Double grad_mult) override { var.update_backward(grad_mult); }
    virtual Double evaluate() override { var.clear_gradient(); return var.get_value(); }
    virtual Str    to_str() const override { return var.to_str(); }
private:
    Variable var;
};


class ConstExpression : public Expression{
public:
    ConstExpression(Double const_value): const_value{const_value} {}
    virtual void   backward(Double grad_mult)  {};
    virtual Double evaluate() { return const_value; }
    virtual Str    to_str() const { return std::to_string(const_value); }
private:
    const Double const_value;
};


class ExpressionContainer{
public:
    ExpressionContainer(const Double& val): expr{new ConstExpression(val)} {}
    ExpressionContainer(const Variable& var): expr{new VariableExpression(var)} {}
    ExpressionContainer(const Expression::Ptr& expr): expr{expr} {}
    ExpressionContainer(Expression* expr): expr{expr} {}
    ExpressionContainer& operator=(const ExpressionContainer& expr) {
        if (this != &expr){
            ExpressionContainer _expr(expr);
            std::swap(this->expr, _expr.expr);
        }
        return *this;
    }
    inline void   grad_eval()  { expr->backward(1); }
    inline void   backward(Double grad_mult)  { expr->backward(grad_mult); }
    inline Double evaluate() { argument_value = expr->evaluate(); return argument_value; }
    inline Double get_argument() { return argument_value; }
    inline Str    to_str() const { return expr->to_str(); }
private:
    Expression::Ptr expr;
    Double argument_value;
};


class SinExpression : public Expression{
public:
    SinExpression(const ExpressionContainer& expr): expr{expr} {}
    virtual Double evaluate() override;
    virtual void   backward(Double grad_mult) override;
    virtual Str    to_str() const override;
private:
    ExpressionContainer expr;
};


class CosExpression : public Expression{
public:
    CosExpression(const ExpressionContainer& expr): expr{expr} {}
    virtual void   backward(Double grad_mult) override;
    virtual Double evaluate() override;
    virtual Str    to_str() const override;
private:
    ExpressionContainer expr;
};


class DoubleProd : public Expression{
public:
    explicit DoubleProd(Double mult, const ExpressionContainer& expr): 
        mult{mult}, 
        expr{expr}
        {}
    virtual void   backward(Double grad_mult) override;
    virtual Double evaluate() override;
    virtual Str    to_str() const override;
private:
    Double              mult;
    ExpressionContainer expr;
};


class DoubleSum : public Expression{
public:
    explicit DoubleSum(Double add, const ExpressionContainer& expr): 
        add{add}, 
        expr{expr}
        {}
    virtual void   backward(Double grad_mult) override;
    virtual Double evaluate() override;
    virtual Str    to_str() const override;
private:
    Double              add;
    ExpressionContainer expr;
};


class ExpressionProd : public Expression{
public:
    explicit ExpressionProd(const ExpressionContainer& lexpr, const ExpressionContainer& rexpr): 
    lexpr{lexpr}, 
    rexpr{rexpr}
    {}
    virtual Double evaluate() override;
    virtual void   backward(Double grad_mult) override;
    virtual Str    to_str() const override;
private:
    Double second_argument_value; 
    ExpressionContainer lexpr;
    ExpressionContainer rexpr;
};


class ExpressionSum : public Expression{
public:
    explicit ExpressionSum(const ExpressionContainer& lexpr, const ExpressionContainer& rexpr): 
    lexpr{lexpr}, 
    rexpr{rexpr}
    {}
    virtual Double evaluate() override;
    virtual void   backward(Double grad_mult) override;
    virtual Str    to_str() const override;
private:
    Double second_argument_value; 
    ExpressionContainer lexpr;
    ExpressionContainer rexpr;
};


class PowExpression : public Expression{
public:
    PowExpression(const ExpressionContainer& expr, Double pow): 
        pow{pow}, 
        expr{expr}
        {
            if (pow == 0){
                ExpressionContainer expr(new ConstExpression(1));
                std::swap(this->expr, expr);
            }
        }
    virtual Double evaluate() override;
    virtual void   backward(Double grad_mult) override;
    virtual Str    to_str() const override;
private:
    Double pow;
    ExpressionContainer expr;
};


class VariableVector{
public:
    VariableVector(Uint size, Str name="t"): mVarVector{}, mName{name} {
        for (int i =0; i<size; i++){
            mVarVector.push_back(Variable(name + "[" + std::to_string(i) + "]"));
        }
    }
    Variable  generate_variable();
    Variable&  operator[](Uint i) { return mVarVector[i];};
    // Variable&  operator[](Uint i) { return mVarVector[i];};
    Uint      size() const { return mVarVector.size(); }
    DoubleVec get_values();
    void      set_values(const DoubleVec& sv);
private:
    Str                   mName;
    // Uint                  mCounter=0;
    std::vector<Variable> mVarVector;
};

namespace autograd{
    ExpressionContainer sin(const ExpressionContainer& expr);
    ExpressionContainer cos(const ExpressionContainer& expr);
    ExpressionContainer pow(const ExpressionContainer& expr, Double pow);
}



ExpressionContainer operator*(const ExpressionContainer&, const ExpressionContainer&);
ExpressionContainer operator*(Double mult, const ExpressionContainer& );
ExpressionContainer operator+(const ExpressionContainer& lExpr, const ExpressionContainer& rExpr);