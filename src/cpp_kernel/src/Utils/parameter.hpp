#pragma once

#include "typedef.hpp"

class Parameter{
public:
    explicit Parameter(Str name="t", Double val=0): name{name}, value{val} {}
    Parameter& operator=(Double angle);
    Str    name;
    Double value;
};

class ParameterBaseExpr{
public:
    virtual Double eval() const { return 0; }
};


class ParameterExpr: public ParameterBaseExpr{
public:
    ParameterExpr(const ParameterPtr& pptr): parameterPtr{pptr} {}
    virtual Double eval() const override { return parameterPtr->value; }
private:
    ParameterPtr parameterPtr;
};


class ParameterConst: public ParameterBaseExpr{
public:
    explicit ParameterConst(Double val): val{val} {}
    virtual Double eval() const override { return val; }
private:
    Double val;
};


class ParameterProd: public ParameterBaseExpr{
public:
    explicit ParameterProd(Double mult, ParameterExpr pexpr): mult{mult}, pexpr{pexpr} {}
    virtual Double eval() const override { return mult * (pexpr.eval()); }
private:
    Double        mult;
    ParameterExpr pexpr;
};



class ParameterVector{
public:
    explicit ParameterVector(Str s="t") : mName{s} {} 

    ParameterPtr addParameter();
    ParameterPtr operator[](int i);
    DoubleVec    getValues() const;
    Uint         size() const;
    void         setValues(const std::vector<Double>& sv);
private:
    Str                mName;
    Uint               mCounter=0;
    ParameterPtrVector mParVector;
};