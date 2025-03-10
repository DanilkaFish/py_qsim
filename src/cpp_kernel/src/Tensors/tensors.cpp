
#include "tensor.h"
// #include "inc/Tensors/__init__.h"

#include <algorithm>
#include <array>


// ------------------------------------Shape---------------------------------

int index_sum(int val){
    int i = 0;
    while (val >0){
        i += val & 1;
        val = val >> 1;
    }
    return i;
}

bool operator==(const Shape& sl, const Shape& sr){
    return sl.pos_up.msk() == sr.pos_up.msk() & sl.pos_down.msk() == sr.pos_down.msk();
}

int index_shape_change(int id, const std::map<Qubit, int>& up, const std::map<Qubit, int>& down){
    int new_id=0;
    for(auto qub: down){
        if ( (id & 1) != 0){
            new_id += (1 << qub.second);
        }
        id = id >> 1;
    }
    for(auto qub: up){
        if ((id & 1) != 0){
            new_id += (1 << qub.second);
        }
        id = id >> 1;
    }
    return new_id;
}



//-------------------------------------
Shape prod(const Shape& shl, const Shape& shr){
    int intersect = ( shr.pos_up.msk() & shl.pos_down.msk() );
    return Shape(mask(shl.pos_up.msk() | (shr.pos_up.msk() ^ intersect)), 
                 mask(shr.pos_down.msk() | (shl.pos_down.msk() ^ intersect)));
}


std::ostream& operator<<(std::ostream& os, const Shape& sh){
    int up = sh.pos_up.msk();
    int i = 0;
    os << "up  : ";
        while (up > 0){
            if ((up&1) == 1)
                os << i << " ";
            i++;
            up=up>>1;
        }

    i = 0;
    up = sh.pos_down.msk();
    os << "\ndown: ";
        while (up > 0){
            if ((up&1) == 1)
                os << i << " ";
            i++;
            up=up>>1;
        }
    return os << "\n";
}

bool operator==(const Tensor& Tl, const Tensor& Tr){
    if (!(Tl.get_shape() == Tr.get_shape())){
        return false;
    }
    for (int i = 0; i < Tl.size(); i++ ){
        if (Tl[i] != Tr[i]){
            return false;
        }
    }
    return true;
}


bool compare(const Tensor& Tl, const Tensor& Tr, double pres){
    if (!(Tl.get_shape() == Tr.get_shape())){
        return false;
    }
    for (int i = 0; i < Tl.size(); i++ ){
        DataType d = Tl[i] - Tr[i];
        if (std::abs(d) > pres){
            return false;
        }
    }
    return true;
}
std::ostream& operator<<(std::ostream& os, const Tensor& T){
    os << "[ ";
    for(int i = 0; i < T.size(); i++ ){
        os << T[i] << " ";
    }
    return os << "]\n";
}


Tensor::Tensor(const Qubits& up, const Qubits& down, const Data& data): shape{up, down}, dptr{new Data(1 << (up.size() + down.size()))} {
    std::map<Qubit, int> posmapd;
    std::map<Qubit, int> posmapu;
    int pos=0;
    for (auto x: down){
        posmapu[x] = pos;
        pos++;
    }
    for (auto x: up){
        posmapd[x] = pos;
        pos++;
    }

    for (int i=0;i<dptr->size(); i++){
        (*dptr)[index_shape_change(i, posmapd, posmapu)] = data[i]; 
    }
}