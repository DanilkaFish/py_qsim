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
    Uint size() const { return 1 << shape.size(); }
protected:
    Shape<MQ> shape;
};


template<MaxQubit MQ>
class DiagonalTensor: public BaseTensor<MQ>{
public:
    DiagonalTensor(
        const Qubits& qubs, 
        const RawData& rd, 
        std::vector<Uint> down_indexes
    ): BaseTensor<MQ>{Shape<MQ>(qubs, qubs)}, rd{rd}, down_indexes{down_indexes} {}

    Complex operator[] (Uint i) const { 
        Shape<MQ> _sh{this->get_shape()};
        Uint down_mask = ((1 << (_sh.mdown.nq)) - 1) ;
        // std::cerr << (i); 
        if (down_indexes[i >> _sh.mdown.nq] == (i & down_mask ))
            return rd[i >> _sh.mdown.nq];
        return 0; 
    }
    Complex get_by_index(Uint i) const {
        return rd[i];
    }
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
    Uint get_down_index(Uint i) const { return down_indexes[i]; }
    using BaseTensor<MQ>::size;
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
    // Tensor(DiagonalTensor)
    Tensor(const Tensor<MQ>& data) = default;
    Complex operator[] (Uint i) const { return rd[i]; }
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

// template<MaxQubit MQ>
// Tensor<MQ> operator*(Complex, const Tensor<MQ>&);

// template<MaxQubit MQ>
// Tensor<MQ> operator*(const Tensor<MQ>&, Complex);

template<typename TENSOR_L, typename TENSOR_R>
bool operator==(const TENSOR_L&, const TENSOR_R&);

template<MaxQubit MQ>
Tensor<MQ> operator*(const DiagonalTensor<MQ>&, const Tensor<MQ>&);

template<MaxQubit MQ>
Tensor<MQ> operator*(const Tensor<MQ>&, const DiagonalTensor<MQ>&);
// template<MaxQubit MQ>
// Tensor<MQ> operator*(const Tensor<MQ>&, const DiagonalTensor<MQ>&);

template<MaxQubit MQ>
DiagonalTensor<MQ> operator*(const DiagonalTensor<MQ>&, const DiagonalTensor<MQ>&);