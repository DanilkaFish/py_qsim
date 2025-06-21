#pragma once

#include <tuple>
#include <memory>

#include "typedef.hpp"
#include "shape.hpp"

struct RawData{
    using DataPtr = std::shared_ptr<std::vector<Complex>>;
    RawData(const std::vector<Complex>& data): dptr{new std::vector<Complex>(data)} {}
    RawData(const DataPtr& dptr): dptr{dptr} {}
    RawData(const RawData& rd): dptr{rd.dptr} {}
    inline Complex operator[](Uint index) { return (*dptr)[index]; } 
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
        const rd& rd, 
        std::vector<Uint> up_indexes
    ): BaseTensor{shape}, rd{rd}, up_indexes{down_indexes} {}
    Complex operator[] (Uint i) { 
        Shape<MQ> _sh{this->shape};
        Uint down_mask = ((1 << (_sh.pos_down.nq + 1)) - 1);
        if up_indexes[i & down_mask] == (i & (down_mask << _sh.pos_down.nq))
            return rd[i & down_mask]
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
    Tensor(const Shape<MQ>& shape, const rd& rd): BaseTensor{shape}, rd{rd} {} 
    Tensor(const Tensor<MQ>& data) = default;
    Complex operator[] (Uint i) { return rd[i]; }
private:
    RawData rd;
};

template<MaxQubit MQ>
Tensor<MQ> operator*(const Tensor<MQ>&, const Tensor<MQ>&);

template<MaxQubit MQ>
Tensor<MQ> operator*(const DiagonalTensor<MQ>&, const Tensor<MQ>&);

template<MaxQubit MQ>
Tensor<MQ> operator*(const Tensor<MQ>&, const DiagonalTensor<MQ>&);

template<MaxQubit MQ>
DiagonalTensor<MQ> operator*(const DiagonalTensor<MQ>&, const DiagonalTensor<MQ>&);