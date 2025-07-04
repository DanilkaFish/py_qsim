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
    
    std::vector<Uint> small_new_indexes((1 << mask_small_uncom.nq));
    std::vector<Uint> small_uncom_indexes((1 << mask_small_uncom.nq));
    for(int i=0; i < (1 << mask_small_uncom.nq); i++){
        small_new_indexes[i] = mask_small_new.expand(i);
        small_uncom_indexes[i] = mask_small_uncom.expand(i);
    }

    for (int j=0; j < (1 << mask_buffered_uncom.nq); j++){
        Uint buffered_index = mask_buffered_uncom.expand(j);
        Uint buffered_new_index = mask_buffered_new.expand(j);
        for(int k=0; k < com_size; k++){
            buffer[k] = buffered_tensor[expand_buffered[k] + buffered_index];
        }
        for(int i=0; i < (1 << mask_small_uncom.nq); i++){
            val = 0;
            for(int k=0; k < com_size; k++){
                val += small_tensor[expand_small[k] + small_uncom_indexes[i]] * buffer[k];
            }
            (*dptr)[small_new_indexes[i] + buffered_new_index] = val;
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
    Shape<MQ> lsh{lt.get_shape()};
    Shape<MQ> rsh{rt.get_shape()};
    Shape<MQ> new_shape{prod(lsh, rsh)};
    mask<MQ> com(lsh.mdown.msk() & rsh.mup.msk());
    if (com.msk() != lsh.mdown.msk()){
        throw std::runtime_error("Diagonal works only with full overlap of indexes");
    }
    Uint l_size = 1 << lsh.size();
    Uint r_size = 1 << rsh.size();
    Uint new_size = 1 << new_shape.size();
    Uint com_size = 1 << com.nq;
    mask<MQ> uncom((r_size - 1) & (~ (rsh.mup.compress(com.msk()) << rsh.mdown.nq)) );
    mask<MQ> comr(rsh.mup.compress(com.msk()) << rsh.mdown.nq);
    mask<MQ> newl((new_size - 1) & (new_shape.mup.compress(lsh.mup.msk()) << new_shape.mdown.nq) );
    mask<MQ> newr((new_size - 1) & (~newl.msk()) );
    RawData::DataPtr dptr{new ComplexVec(1 << new_shape.size())};
    for (int i =0; i < (1 << lt.get_shape().mup.nq); i++){
        Uint j = comr.expand(lt.get_down_index(i));
        Uint newj = newl.expand(i);
        for (int k=0; k < r_size / com_size; k++){
            (*dptr)[newj + newr.expand(k)] = lt.get_by_index(i) * rt[j + uncom.expand(k)];
        }
    }
    return Tensor<MQ>(new_shape, RawData(dptr));
}

template<MaxQubit MQ>
Tensor<MQ> operator*(const Tensor<MQ>& lt, const DiagonalTensor<MQ>& rt){
    Shape<MQ> lsh{lt.get_shape()};
    Shape<MQ> rsh{rt.get_shape()};
    Shape<MQ> new_shape{prod(lsh, rsh)};
    mask<MQ> com(lsh.mdown.msk() & rsh.mup.msk());
    if (com.msk() != rsh.mup.msk()){
        throw std::runtime_error("Diagonal works only with full overlap of indexes");
    }
    Uint l_size = 1 << lsh.size();
    Uint r_size = 1 << rsh.size();
    Uint new_size = 1 << new_shape.size();
    Uint com_size = 1 << com.nq;

    mask<MQ> uncom((l_size - 1) & ( ~(lsh.mdown.compress(com.msk()))));
    mask<MQ> coml(lsh.mdown.compress(com.msk()));
    mask<MQ> newr((new_size - 1) & (new_shape.mdown.compress(rsh.mdown.msk())) );
    mask<MQ> newl((new_size - 1) & (~newr.msk()) );
    RawData::DataPtr dptr{new ComplexVec(1 << new_shape.size())};
    for (int i =0; i < (1 << rt.get_shape().mup.nq); i++){
        Uint j = coml.expand(i);
        Uint newj = newr.expand(rt.get_down_index(i));
        for (int k=0; k < l_size / com_size; k++){
            (*dptr)[newl.expand(k) + newj] = lt[j + uncom.expand(k)] * rt.get_by_index(i);
        }
    }
    return Tensor<MQ>(new_shape, RawData(dptr));
}

// template<MaxQubit MQ>
// Tensor<MQ> operator*(const Tensor<MQ>& lt, const DiagonalTensor<MQ>& rt){
//     Shape<MQ> new_shape = prod(lt.shape, rt.shape);
//     RawData::DataPtr dptr{new ComplexVec(1 << new_shape.size())};
//     // product implementation

//     return Tensor<MQ>(new_shape, RawData(dptr));
// }

template<MaxQubit MQ>
DiagonalTensor<MQ> operator*(const DiagonalTensor<MQ>& lt, const DiagonalTensor<MQ>& rt){
    Shape<MQ> lsh{lt.get_shape()};
    Shape<MQ> rsh{rt.get_shape()};
    Shape<MQ> new_shape{prod(lsh, rsh)};
    mask<MQ> com(lsh.mdown.msk() & rsh.mup.msk());

    RawData::DataPtr dptr{new ComplexVec(1 << new_shape.mup.nq)};
    std::vector<Uint> poss(1 << new_shape.mup.nq);
    for (int iim=0; iim < (1 << new_shape.mup.nq); iim++){
        Uint iim_expand = new_shape.mup.expand(iim);
        Uint ii = lsh.mup.compress(iim_expand);
        Uint jm_l_expand = lsh.mdown.expand(lt.get_down_index(ii));
        Uint jm_r = rsh.mup.compress(jm_l_expand + (iim_expand & (~lsh.mup.msk())));
        Uint kmk = new_shape.mdown.compress(rsh.mdown.expand(rt.get_down_index(jm_r)) + (jm_l_expand & (~ rsh.mup.msk())));
        (*dptr)[iim] = lt.get_by_index(ii) * rt.get_by_index(jm_r);
        poss[iim] = kmk;
    }
    return DiagonalTensor<MQ>(Qubits(Qubs(new_shape.mup.msk())), RawData(dptr), poss);
}

template<MaxQubit MQ, typename TENSOR_L, typename TENSOR_R>
Tensor<MQ> operator+(const TENSOR_L& lt, const TENSOR_R& rt){
    if (lt.get_shape() != rt.get_shape()){
        throw std::runtime_error("addition of tensors with different sizes");
    }
    RawData::DataPtr dptr{new ComplexVec(rt.size())};
    for(int i=0; i<lt.size(); i++){
        (*dptr)[i] = lt[i] + rt[i];
    }
    return Tensor<MQ>(lt.get_shape(), dptr);
}

template<MaxQubit MQ, typename TENSOR_L, typename TENSOR_R>
Tensor<MQ> operator-(const TENSOR_L& lt, const TENSOR_R& rt){
    if (lt.get_shape() != rt.get_shape()){
        throw std::runtime_error("addition of tensors with different sizes");
    }
    RawData::DataPtr dptr{new ComplexVec(rt.size())};
    for(int i=0; i<lt.size(); i++){
        (*dptr)[i] = lt[i] - rt[i];
    }
    return Tensor<MQ>(lt.get_shape(), dptr);
}

template<MaxQubit MQ>
Tensor<MQ> operator*(Complex val, const Tensor<MQ>& t){
    RawData::DataPtr dptr{new ComplexVec(t.size())};
    for(int i=0; i<t.size(); i++){
        (*dptr)[i] = val * t[i];
    }
    return Tensor<MQ>(t.get_shape(), dptr);
}

template<MaxQubit MQ>
Tensor<MQ> operator*(const Tensor<MQ>& t, Complex val){
    RawData::DataPtr dptr{new ComplexVec(t.size())};
    for(int i=0; i<t.size(); i++){
        (*dptr)[i] = val * t[i];
    }
    return Tensor<MQ>(t.get_shape(), dptr);
}

template<MaxQubit MQ>
DiagonalTensor<MQ> operator*(const DiagonalTensor<MQ>& t, Complex val) {
    Uint size = 1 << t.get_shape().mdown.nq;
    RawData::DataPtr dptr{new ComplexVec(size)};
    for(int i=0; i<t.size(); i++){
        (*dptr)[i] = val * t.get_by_index(i);
    }
    return DiagonalTensor<MQ>(t.get_shape(), dptr, t.get_down_indexes());
}

template<MaxQubit MQ>
DiagonalTensor<MQ> operator*(Complex val, const DiagonalTensor<MQ>& t) {
    Uint size = 1 << t.get_shape().mdown.nq;
    RawData::DataPtr dptr{new ComplexVec(size)};
    for(int i=0; i<t.size(); i++){
        (*dptr)[i] = val * t.get_by_index(i);
    }
    return DiagonalTensor<MQ>(t.get_shape(), dptr, t.get_down_indexes());
}

template Tensor<MaxQubit::Q1> operator*(const Tensor<MaxQubit::Q1>&, const Tensor<MaxQubit::Q1>&);
template Tensor<MaxQubit::Q2> operator*(const Tensor<MaxQubit::Q2>&, const Tensor<MaxQubit::Q2>&);
template Tensor<MaxQubit::Q4> operator*(const Tensor<MaxQubit::Q4>&, const Tensor<MaxQubit::Q4>&);
template Tensor<MaxQubit::Q8> operator*(const Tensor<MaxQubit::Q8>&, const Tensor<MaxQubit::Q8>&);
template Tensor<MaxQubit::Q16> operator*(const Tensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q32> operator*(const Tensor<MaxQubit::Q32>&, const Tensor<MaxQubit::Q32>&);

template bool operator==(const Tensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);
template bool operator==(const DiagonalTensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);
// template Tensor<MaxQubit::Q1> operator*(const  DiagonalTensor<MaxQubit::Q1>&, const Tensor<MaxQubit::Q1>&);
// template Tensor<MaxQubit::Q2> operator*(const  DiagonalTensor<MaxQubit::Q2>&, const Tensor<MaxQubit::Q2>&);
// template Tensor<MaxQubit::Q4> operator*(const  DiagonalTensor<MaxQubit::Q4>&, const Tensor<MaxQubit::Q4>&);
// template Tensor<MaxQubit::Q8> operator*(const  DiagonalTensor<MaxQubit::Q8>&, const Tensor<MaxQubit::Q8>&);
template Tensor<MaxQubit::Q16> operator*(const DiagonalTensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q16> operator*(const Tensor<MaxQubit::Q16>&, const DiagonalTensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q16> operator*(Complex, const Tensor<MaxQubit::Q16>&);
template DiagonalTensor<MaxQubit::Q16> operator*(Complex, const DiagonalTensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q16> operator*(const Tensor<MaxQubit::Q16>&, Complex);
template DiagonalTensor<MaxQubit::Q16> operator*(const DiagonalTensor<MaxQubit::Q16>&, Complex);

template Tensor<MaxQubit::Q16> operator+(const Tensor<MaxQubit::Q16>&, const DiagonalTensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q16> operator+(const DiagonalTensor<MaxQubit::Q16>&, const DiagonalTensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q16> operator+(const DiagonalTensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q16> operator+(const Tensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);

template Tensor<MaxQubit::Q16> operator-(const Tensor<MaxQubit::Q16>&, const DiagonalTensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q16> operator-(const DiagonalTensor<MaxQubit::Q16>&, const DiagonalTensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q16> operator-(const DiagonalTensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);
template Tensor<MaxQubit::Q16> operator-(const Tensor<MaxQubit::Q16>&, const Tensor<MaxQubit::Q16>&);

template DiagonalTensor<MaxQubit::Q16> operator*(const DiagonalTensor<MaxQubit::Q16>&, const DiagonalTensor<MaxQubit::Q16>&);
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