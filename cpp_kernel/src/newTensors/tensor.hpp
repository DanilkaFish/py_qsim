#pragma once

#include <tuple>
#include <memory>
#include <iomanip>

#include "typedef.hpp"
#include "shape.hpp"

struct RawData{
    using DataPtr = std::shared_ptr<ComplexVec>;
    RawData(const std::vector<Complex>& data): dptr{new ComplexVec(data)} {}
    RawData(const DataPtr& dptr): dptr{dptr} {}
    RawData(const RawData& rd): dptr{rd.dptr} {}
    inline Complex operator[] (Uint index) const { return (*dptr)[index]; } 
    inline Complex& operator[] (Uint index) { return (*dptr)[index]; } 
    DataPtr dptr;
};


template<MaxQubit MQ>
class BaseTensor{
public:
    BaseTensor(const Shape<MQ>& shape): shape{shape} {}
    Shape<MQ> get_shape() const { return shape; }
    // int fun() { shape.}
protected:
    Shape<MQ> shape;
};


template<MaxQubit MQ>
class DiagonalTensor: public BaseTensor<MQ>{
public:
    DiagonalTensor(
        const Shape<MQ>& shape, 
        const RawData& rd, 
        std::vector<Uint> down_indexes
    ): BaseTensor<MQ>{shape}, rd{rd}, down_indexes{down_indexes} {}
    Complex operator[] (Uint i) { 
        Shape<MQ> _sh{this->get_shape()};
        Uint down_mask = ((1 << (_sh.mdown.nq + 1)) - 1);
        if (down_indexes[i & down_mask] == (i & (down_mask << _sh.mdown.nq)))
            return rd[i & down_mask];
        return 0; 
    }
private:
    using BaseTensor<MQ>::shape;
    std::vector<Uint> down_indexes;
    RawData rd;
};


template<MaxQubit MQ>
class Tensor: public BaseTensor<MQ>{
public:
    Tensor(Complex val): BaseTensor<MQ>{Shape<MQ>()}, rd{ComplexVec{val} } {}
    Tensor(const Shape<MQ>& shape, const RawData& rd): BaseTensor<MQ>{shape}, rd{rd} {
        if (!is_valid_shape()){
            std::cerr << shape.size() << "\n" << rd.dptr->size();
            throw std::runtime_error("Inconsistent shape and length of array");
        }
    } 
    Tensor(const Tensor<MQ>& data) = default;
    Uint size() const { return 1 << shape.size(); }
    Complex operator[] (Uint i) const { return rd[i]; }
    Complex& operator[] (Uint i) { return rd[i]; }
    Str str() const{
        Complex z;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << "[\n";
        for (int j=0; j < (1<< shape.mup.nq); j++){
            for(int i=0; i < (1<< shape.mdown.nq); i++){
                z = operator[]((j << shape.mdown.nq) + i);
                oss << "(" << z.real() << ' ' << z.imag() <<"i) ";
            }
            oss << "\n";
        }
        oss << "]";

        return oss.str();
    }
private:
    RawData rd;
    using BaseTensor<MQ>::shape;
    bool is_valid_shape(){
        return rd.dptr->size() == (1 << shape.size());
    }
};

template<MaxQubit MQ>
Tensor<MQ> operator*(const Tensor<MQ>&, const Tensor<MQ>&);

template<typename TENSOR_L, typename TENSOR_R>
bool operator==(const TENSOR_L&, const TENSOR_R&);

// template<MaxQubit MQ>
// Tensor<MQ> operator*(const DiagonalTensor<MQ>&, const Tensor<MQ>&);

// template<MaxQubit MQ>
// Tensor<MQ> operator*(const Tensor<MQ>&, const DiagonalTensor<MQ>&);

// template<MaxQubit MQ>
// DiagonalTensor<MQ> operator*(const DiagonalTensor<MQ>&, const DiagonalTensor<MQ>&);