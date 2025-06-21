#ifndef _SHAPE
#define _SHAPE
#include <algorithm>
#include <map>
#include <array>
#include "typedef.hpp"
#include "Utils/qubits.hpp"

enum class MaxQubit{
    Q1,
    Q2,
    Q4,
    Q8,
    Q16,
    Q32
};

constexpr Uint ld_bits = 5;

Uint index_sum(Uint val);

template<MaxQubit MQ>
struct mask{
    mask()=default;
    explicit mask(Uint m): m{m} { set_mv(); }
    mask(const mask& ms): m{ms.m}, mv{ms.mv} {} 
    explicit mask(const Qubits& qubs): m{0}{
        m = qubs.get_qubits();
        set_mv();
    }

    Uint compress(Uint x) const{
        x = x & m;
        Uint t;
        for (Uint i = 0; i < mv[ld_bits]; i++) {
            t = x & mv[i]; 
            x = (x ^ t) | (t >> (1 << i));
        }
        return x;
    }
    Uint expand(Uint x) const {
        for (Uint i = mv[ld_bits] - 1; i >=0 ; i--) {
            x = (((x << (1 << i)) ^ x) & mv[i]) ^ x;
        }

        return  x & m;
    }
    Uint msk() const { return m; }
    Uint nq;
private:
    Uint m;
    void set_mv(){
        nq = index_sum(m);
        Uint _m = m;
        Uint mk = (~_m) << 1; 
        Uint mp;
        mv[ld_bits] = ld_bits;
        for (Uint i = 0; i < ld_bits; i++) {
            mp = mk ^ (mk << 1); // Parallel prefix. 
            mp = mp ^ (mp << 2); 
            mp = mp ^ (mp << 4); 
            mp = mp ^ (mp << 8);
            mp = mp ^ (mp << 16);
            mv[i] = mp & _m; // Bits to move.
            _m = (_m ^ mv[i]) | (mv[i] >> (1 << i)); 
            mk = mk & (~mp); 
            if (mk == 0){
                mv[ld_bits] = i + 1;
                break;
            }
        }
    }
    std::array<Uint,ld_bits + 1> mv;
};


template<MaxQubit MQ>
class Shape{
public:
    Shape(): pos_up{0}, pos_down{0}{}
    Shape(const mask<MQ>& up, const mask<MQ>& down): pos_up{up}, pos_down{down} { }
    Shape(const Qubits& up, const Qubits& down): pos_up{up.get_qubits()}, pos_down{down.get_qubits()} { }
    Shape(const Shape& sh): Shape(sh.pos_up, sh.pos_down) {}
    size_t size() const { return pos_up.nq + pos_down.nq; }
    mask<MQ> pos_up;
    mask<MQ> pos_down;
};


template<MaxQubit MQ>
Shape<MQ> prod(const Shape<MQ>& shl, const Shape<MQ> & shr){
    int intersect = ( shr.pos_up.msk() & shl.pos_down.msk() );
    return Shape<MQ>(mask<MQ> (shl.pos_up.msk() | (shr.pos_up.msk() ^ intersect)), 
                 mask<MQ> (shr.pos_down.msk() | (shl.pos_down.msk() ^ intersect)));
}


template<MaxQubit MQ>
bool operator==(const Shape<MQ>& sl, const Shape<MQ>& sr){
    return sl.pos_up.msk() == sr.pos_up.msk() & sl.pos_down.msk() == sr.pos_down.msk();
}

template<MaxQubit MQ>
std::ostream& operator<<(std::ostream& os, const Shape<MQ>& shape){
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


#endif