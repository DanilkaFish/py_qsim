#include "typedef.hpp"
#include "tensor.hpp"
#include "shape.hpp"


Uint index_sum(Uint val){
    int i = 0;
    while (val >0){
        i += val & 1;
        val = val >> 1;
    }
    return i;
}
// template Shape<MaxQubit::Q1> prod(const Shape<MaxQubit::Q1>&, const Shape<MaxQubit::Q1>&);
// template Shape<MaxQubit::Q2> prod(const Shape<MaxQubit::Q2>&, const Shape<MaxQubit::Q2>&);
// template Shape<MaxQubit::Q4> prod(const Shape<MaxQubit::Q4>&, const Shape<MaxQubit::Q4>&);
// template Shape<MaxQubit::Q8> prod(const Shape<MaxQubit::Q8>&, const Shape<MaxQubit::Q8>&);
// template Shape<MaxQubit::Q16> prod(const Shape<MaxQubit::Q16>&, const Shape<MaxQubit::Q16>&);
// template Shape<MaxQubit::Q32> prod(const Shape<MaxQubit::Q32>&, const Shape<MaxQubit::Q32>&);

// -----------------------tensor product implementation ---------------------------

template<MaxQubit MQ>
Tensor<MQ> simple_prod(Uint com_size,
                       const Shape<MQ>& new_shape, 
                       const Tensor<MQ>& lt, 
                       const Tensor<MQ>& rt, 
                       const std::vector<Uint>& expand,
                       const mask<MQ>& mask4uncom_r
                    ){
    Uint new_size = 1 << (new_shape.size());
    RawData::DataPtr dptr{new ComplexVec(new_size)};
    Complex val;
    for (int j=0; j < new_size; j++){
        Uint buffer_j = mask4uncom_r.expand(j);
        val = 0;
        for(int k=0; k < com_size; k++){
            val += lt[k] * rt[expand[k] + buffer_j];
        }
        (*dptr)[j] = val;
    }
    return Tensor<MQ>(new_shape, RawData(dptr));
}

template<MaxQubit MQ>
Tensor<MQ> buffer_prod(Uint com_size,
                       const Shape<MQ>& new_shape, 
                       const Tensor<MQ>& small_tensor, 
                       const Tensor<MQ>& buffered_tensor, 
                       const std::vector<Uint>& expand_small,
                       const std::vector<Uint>& expand_buffered,
                       const mask<MQ>& mask_small_uncom,
                       const mask<MQ>& mask_buffered_uncom,
                       const mask<MQ>& mask_small_new,
                       const mask<MQ>& mask_buffered_new
                    ){

    Uint new_size = 1 << (new_shape.size());
    RawData::DataPtr dptr{new ComplexVec(new_size)};
    Complex val;
    ComplexVec buffer(com_size);
    for (int j=0; j < (1 << mask_buffered_uncom.nq); j++){
        Uint index_buffered = mask_buffered_uncom.expand(j);
        for(int k=0; k < com_size; k++){
            buffer[k] = buffered_tensor[expand_buffered[k] + index_buffered];
        }
        for(int i=0; i < (1 << mask_small_uncom.nq); i++){
            Uint index_small = mask_small_uncom.expand(i);
            val = 0;
            for(int k=0; k < com_size; k++){
                val += small_tensor[expand_small[k] + index_small] * buffer[k];
            }
            (*dptr)[mask_small_new.expand(i) + mask_buffered_new.expand(j)] = val;
        }
    }
    return Tensor<MQ>(new_shape, RawData(dptr));
}


template<MaxQubit MQ>
Tensor<MQ> operator*(const Tensor<MQ>& lt, const Tensor<MQ>& rt){
    Shape<MQ> lsh{lt.get_shape()};
    Shape<MQ> rsh{rt.get_shape()};
    Shape<MQ> new_shape{prod(lsh, rsh)};
    mask<MQ> com(lsh.mdown.msk() & rsh.mup.msk());
    Uint l_size = 1 << lsh.size();
    Uint r_size = 1 << rsh.size();
    Uint new_size = 1 << new_shape.size();
    Uint com_size = 1 << com.nq;
    if (com_size == l_size){
        mask<MQ> mask4uncom_r((r_size - 1) & (~(rsh.mup.compress(com.msk()) << rsh.mdown.nq)));
        mask<MQ> mask4com_r((r_size - 1) & (~mask4uncom_r.msk()));
        std::vector<Uint> expandr(com_size);
        for(int k=0; k < com_size; k++){
            expandr[k] = mask4com_r.expand(k) << rsh.mdown.nq;
        }
        return simple_prod<MQ>(com_size, new_shape, lt, rt, expandr, mask4uncom_r);
    }
    if (com_size == r_size){
        mask<MQ> mask4uncom_l((l_size - 1) & (~(lsh.mdown.compress(com.msk()))));
        mask<MQ> mask4com_l((r_size - 1) & (~mask4uncom_l.msk()));
        std::vector<Uint> expandl(com_size);
        for(int k=0; k < com_size; k++){
            expandl[k] = mask4com_l.expand(k);
        }
        return simple_prod<MQ>(com_size, new_shape, rt, lt, expandl, mask4uncom_l);
    }

    mask<MQ> mask4uncom_l( (l_size - 1) & (~ (lsh.mdown.compress(com.msk()))) );
    mask<MQ> mask4uncom_r( (r_size - 1) & (~ (rsh.mup.compress(com.msk()) << rsh.mdown.nq)) );
    mask<MQ> mask4i((new_size - 1) & ((new_shape.mup.compress(lsh.mup.msk()) << new_shape.mdown.nq) + new_shape.mdown.compress(lsh.mdown.msk() & (~rsh.mup.msk()))) );
    mask<MQ> mask4j((new_size - 1) & (~mask4i.msk()) );
    std::vector<Uint> expandl(com_size);
    std::vector<Uint> expandr(com_size);

    mask<MQ> mask4com_l((l_size - 1) ^ mask4uncom_l.msk());
    mask<MQ> mask4com_r((r_size - 1) ^ mask4uncom_r.msk());
    for(int k=0; k < com_size; k++){
        expandl[k] = mask4com_l.expand(k);
        expandr[k] = mask4com_r.expand(k);
    }

    if (lsh.size() <= rsh.size()){
        return buffer_prod<MQ>(com_size, new_shape, lt, rt, expandl, expandr, mask4uncom_l, mask4uncom_r, mask4i, mask4j);
    }else{
        return buffer_prod<MQ>(com_size, new_shape, rt, lt, expandr, expandl, mask4uncom_r, mask4uncom_l, mask4j, mask4i);
    }
}

template<typename TENSOR_L, typename TENSOR_R>
bool operator==(const TENSOR_L& tl, const TENSOR_R& tr){
    if (tl.get_shape() != tr.get_shape()){
        return false;
    } 
    for(int i=0; i < tl.size(); i++){
        if (tl[i] != tr[i]){
            return false;
        }
    }
    return true;
}

template<MaxQubit MQ>
Tensor<MQ> operator*(const DiagonalTensor<MQ>& lt, const Tensor<MQ>& rt){
    Shape<MQ> new_shape = prod(lt.shape, rt.shape);
    RawData::DataPtr dptr{new ComplexVec(1 << new_shape.size())};
    // product implementation

    return Tensor<MQ>(new_shape, RawData(dptr));
}

template<MaxQubit MQ>
Tensor<MQ> operator*(const Tensor<MQ>& lt, const DiagonalTensor<MQ>& rt){
    Shape<MQ> new_shape = prod(lt.shape, rt.shape);
    RawData::DataPtr dptr{new ComplexVec(1 << new_shape.size())};
    // product implementation

    return Tensor<MQ>(new_shape, RawData(dptr));
}

// template<MaxQubit MQ>
// DiagonalTensor<MQ> operator*(const DiagonalTensor<MQ>&, const DiagonalTensor<MQ>&){

// }






template Tensor<MaxQubit::Q1> operator*(const Tensor<MaxQubit::Q1>&, const Tensor<MaxQubit::Q1>&);
template Tensor<MaxQubit::Q2> operator*(const Tensor<MaxQubit::Q2>&, const Tensor<MaxQubit::Q2>&);
template Tensor<MaxQubit::Q4> operator*(const Tensor<MaxQubit::Q4>&, const Tensor<MaxQubit::Q4>&);
template Tensor<MaxQubit::Q8> operator*(const Tensor<MaxQubit::Q8>&, const Tensor<MaxQubit::Q8>&);
template Tensor<MaxQubit::Q16> operator*(const Tensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q32> operator*(const Tensor<MaxQubit::Q32>&, const Tensor<MaxQubit::Q32>&);

template bool operator==(const Tensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);
// template Tensor<MaxQubit::Q1> operator*(const  DiagonalTensor<MaxQubit::Q1>&, const Tensor<MaxQubit::Q1>&);
// template Tensor<MaxQubit::Q2> operator*(const  DiagonalTensor<MaxQubit::Q2>&, const Tensor<MaxQubit::Q2>&);
// template Tensor<MaxQubit::Q4> operator*(const  DiagonalTensor<MaxQubit::Q4>&, const Tensor<MaxQubit::Q4>&);
// template Tensor<MaxQubit::Q8> operator*(const  DiagonalTensor<MaxQubit::Q8>&, const Tensor<MaxQubit::Q8>&);
// template Tensor<MaxQubit::Q16> operator*(const DiagonalTensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);
// template Tensor<MaxQubit::Q32> operator*(const DiagonalTensor<MaxQubit::Q32>&, const Tensor<MaxQubit::Q32>&);

// template Tensor<MaxQubit::Q1> operator*(const Tensor<MaxQubit::Q1>&, const DiagonalTensor<MaxQubit::Q1>&);
// template Tensor<MaxQubit::Q2> operator*(const Tensor<MaxQubit::Q2>&, const DiagonalTensor<MaxQubit::Q2>&);
// template Tensor<MaxQubit::Q4> operator*(const Tensor<MaxQubit::Q4>&, const DiagonalTensor<MaxQubit::Q4>&);
// template Tensor<MaxQubit::Q8> operator*(const Tensor<MaxQubit::Q8>&, const DiagonalTensor<MaxQubit::Q8>&);
// template Tensor<MaxQubit::Q16> operator*(const Tensor<MaxQubit::Q16>&, const DiagonalTensor<MaxQubit::Q16>&);
// template Tensor<MaxQubit::Q32> operator*(const Tensor<MaxQubit::Q32>&, const DiagonalTensor<MaxQubit::Q32>&);

// template DiagonalTensor<MaxQubit::Q1> operator*(const DiagonalTensor<MaxQubit::Q1>&, const DiagonalTensor<MaxQubit::Q1>&);
// template DiagonalTensor<MaxQubit::Q2> operator*(const DiagonalTensor<MaxQubit::Q2>&, const DiagonalTensor<MaxQubit::Q2>&);
// template DiagonalTensor<MaxQubit::Q4> operator*(const DiagonalTensor<MaxQubit::Q4>&, const DiagonalTensor<MaxQubit::Q4>&);
// template DiagonalTensor<MaxQubit::Q8> operator*(const DiagonalTensor<MaxQubit::Q8>&, const DiagonalTensor<MaxQubit::Q8>&);
// template DiagonalTensor<MaxQubit::Q16> operator*(const DiagonalTensor<MaxQubit::Q16>&, const DiagonalTensor<MaxQubit::Q16>&);
// template DiagonalTensor<MaxQubit::Q32> operator*(const DiagonalTensor<MaxQubit::Q32>&, const DiagonalTensor<MaxQubit::Q32>&);

// -----------------------specialization of mask--------------------------


// template struct mask<MaxQubit::Q1>;
// template struct mask<MaxQubit::Q2>;
// template struct mask<MaxQubit::Q4>;
// template struct mask<MaxQubit::Q8>;
// template struct mask<MaxQubit::Q16>;
// template struct mask<MaxQubit::Q32>;