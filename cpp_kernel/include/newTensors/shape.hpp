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
    mask();
    explicit mask(Uint m);
    mask(const mask& ms);
    explicit mask(const Qubits& qubs);
    Uint compress(Uint x) const;
    Uint expand(Uint x) const;
    Uint msk() const;
    Uint nq;
};

// TODO set_mv
template<>
struct mask<MaxQubit::Q1>{
    mask()=default;
    explicit mask(Uint m): m{m}, nq{m} { }
    mask(const mask& ms): m{ms.m} {} 
    Uint compress(Uint x) const { return x & m; }
    Uint expand(Uint x)   const { return x & m; }
    Uint msk() const { return m; }
    Uint nq;
private:
    Uint m;
};

template<>
struct mask<MaxQubit::Q2>{
    mask()=default;
    explicit mask(Uint m): m{m} { set_mv(); }
    mask(const mask& ms): m{ms.m}, nq{ms.nq}, mv1{ms.mv1} {} 
    Uint compress(Uint x) const{
        x = x & m;
        Uint t = x & mv1; 
        x = (x ^ t) | (t >> 1);
        return x;
    }
    Uint expand(Uint x) const {
        x = (((x << 1) ^ x) & mv1) ^ x;
        return  x & m;
    }
    Uint msk() const { return m; }
    Uint nq;
private:
    Uint m;
    Uint mv1 = 0;
    void set_mv(){
        nq = index_sum(m);
        if (m == 2){
            mv1 = 2;
        }
    }
};

template<>
struct mask<MaxQubit::Q4>{
    mask()=default;
    explicit mask(Uint m): m{m} { set_mv(); }
    mask(const mask& ms): m{ms.m}, nq{ms.nq}, mv1{ms.mv1}, mv2{ms.mv2} {} 
    Uint compress(Uint x) const{
        x = x & m;
        Uint t;
        t = x & mv1; 
        x = (x ^ t) | (t >> 1);
        t = x & mv2; 
        x = (x ^ t) | (t >> 2);
        return x;
    }
    Uint expand(Uint x) const {
        x = (((x << 2) ^ x) & mv2) ^ x;
        x = (((x << 1) ^ x) & mv1) ^ x;
        return  x & m;
    }
    Uint msk() const { return m; }
    Uint nq;
private:
    Uint m;
    Uint mv1;
    Uint mv2;
    void set_mv(){
        nq = index_sum(m);
        Uint _m = m;
        Uint mk = (~_m) << 1;
        Uint mp;
        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mv1 = mp & _m; 
        _m = (_m ^ mv1) | (mv1 >> 1); 

        mk = mk & (~mp); 
        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mv2 = mp & _m; 
        _m = (_m ^ mv2) | (mv2 >> 2); 
        mk = mk & (~mp); 
    }
};

template<>
struct mask<MaxQubit::Q8>{
    mask()=default;
    explicit mask(Uint m): m{m} { set_mv(); }
    mask(const mask& ms): m{ms.m}, nq{ms.nq}, mv1{ms.mv1}, mv2{ms.mv2}, mv4{ms.mv4} {} 
    Uint compress(Uint x) const{
        x = x & m;
        Uint t;
        t = x & mv1; 
        x = (x ^ t) | (t >> 1);
        t = x & mv2; 
        x = (x ^ t) | (t >> 2);
        t = x & mv4; 
        x = (x ^ t) | (t >> 4);
        return x;
    }
    Uint expand(Uint x) const {
        x = (((x << 4) ^ x) & mv4) ^ x;
        x = (((x << 2) ^ x) & mv2) ^ x;
        x = (((x << 1) ^ x) & mv1) ^ x;
        return  x & m;
    }
    Uint msk() const { return m; }
    Uint nq;
private:
    Uint m;
    Uint mv1;
    Uint mv2;
    Uint mv4;
    void set_mv(){
        nq = index_sum(m);
        Uint _m = m;
        Uint mk = (~_m) << 1;
        Uint mp;

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mv1 = mp ^ (mp << 4) & _m; 
        mk = mk & (~mp); 

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mv2 = mp ^ (mp << 4) & _m; 
        _m = (_m ^ mv2) | (mv2 >> 2); 
        mk = mk & (~mp); 

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mv4 = mp ^ (mp << 4) & _m; 
        _m = (_m ^ mv4) | (mv4 >> 4); 
        mk = mk & (~mp);
    }
};

template<>
struct mask<MaxQubit::Q16>{
    // mask()=default;
    explicit mask(Uint m): m{m} { set_mv(); }
    mask(const mask& ms): m{ms.m}, nq{ms.nq}, mv1{ms.mv1}, mv2{ms.mv2}, mv4{ms.mv4}, mv8{ms.mv8}  {} 
    Uint compress(Uint x) const{
        x = x & m;
        Uint t;
        t = x & mv1; 
        x = (x ^ t) | (t >> 1);
        t = x & mv2; 
        x = (x ^ t) | (t >> 2);
        t = x & mv4; 
        x = (x ^ t) | (t >> 4);
        t = x & mv8; 
        x = (x ^ t) | (t >> 8);
        return x;
    }
    Uint expand(Uint x) const {
        x = (((x << 8) ^ x) & mv8) ^ x;
        x = (((x << 4) ^ x) & mv4) ^ x;
        x = (((x << 2) ^ x) & mv2) ^ x;
        x = (((x << 1) ^ x) & mv1) ^ x;
        return  x & m;
    }
    Uint msk() const { return m; }
    Uint nq;
private:
    Uint m;
    Uint mv1;
    Uint mv2;
    Uint mv4;
    Uint mv8;
    void set_mv(){
        nq = index_sum(m);

        // int _m = m;
        // int mk = (~_m) << 1; 
        // int mp;
        // mv[ld_bits] = ld_bits;
        // for (int i = 0; i < ld_bits; i++) {
        //     mp = mk ^ (mk << 1); // Parallel prefix.
        //     mp = mp ^ (mp << 2);
        //     mp = mp ^ (mp << 4);
        //     mp = mp ^ (mp << 8);
        //     mp = mp ^ (mp << 16);
        //     mv[i] = mp & _m; // Bits to move.
        //     _m = (_m ^ mv[i]) | (mv[i] >> (1 << i)); // Compress m.
        //     mk = mk & (~mp);
        //     if (mk == 0){
        //         mv[ld_bits] = i + 1;
        //         break;
        //     }
        // }

        Uint _m = m;
        Uint mk = (~_m) << 1;
        Uint mp;

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mp = mp ^ (mp << 8); 
        mv1 = mp & _m; 
        _m = (_m ^ mv1) | (mv1 >> 1);
        mk = mk & (~mp); 

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mp = mp ^ (mp << 8); 
        mv2 = mp & _m; 
        _m = (_m ^ mv2) | (mv2 >> 2);
        mk = mk & (~mp); 

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mp = mp ^ (mp << 8); 
        mv4 = mp & _m; 
        _m = (_m ^ mv4) | (mv4 >> 4);
        mk = mk & (~mp);

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mp = mp ^ (mp << 8); 
        mv8 = mp & _m; 
    }
};


template<>
struct mask<MaxQubit::Q32>{
    mask()=default;
    explicit mask(Uint m): m{m} { set_mv(); }
    mask(const mask& ms): m{ms.m}, nq{ms.nq}, mv1{ms.mv1}, mv2{ms.mv2}, mv4{ms.mv4}, mv8{ms.mv8}, mv16{ms.mv16}  {} 
    Uint compress(Uint x) const{
        x = x & m;
        Uint t;
        t = x & mv1; 
        x = (x ^ t) | (t >> 1);
        t = x & mv2; 
        x = (x ^ t) | (t >> 2);
        t = x & mv4; 
        x = (x ^ t) | (t >> 4);
        t = x & mv8; 
        x = (x ^ t) | (t >> 8);
        t = x & mv16; 
        x = (x ^ t) | (t >> 16);
        return x;
    }
    Uint expand(Uint x) const {
        x = (((x << 16) ^ x) & mv16) ^ x;
        x = (((x << 8) ^ x) & mv8) ^ x;
        x = (((x << 4) ^ x) & mv4) ^ x;
        x = (((x << 2) ^ x) & mv2) ^ x;
        x = (((x << 1) ^ x) & mv1) ^ x;
        return  x & m;
    }
    Uint msk() const { return m; }
    Uint nq;
private:
    Uint m;
    Uint mv1;
    Uint mv2;
    Uint mv4;
    Uint mv8;
    Uint mv16;
    void set_mv(){
        nq = index_sum(m);
        Uint _m = m;
        Uint mk = (~_m) << 1;
        Uint mp;

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mp = mp ^ (mp << 8); 
        mv1 = mp ^ (mp << 16) & _m; 
        mk = mk & (~mp); 

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mp = mp ^ (mp << 8); 
        mv2 = mp ^ (mp << 16) & _m; 
        _m = (_m ^ mv2) | (mv2 >> 2); 
        mk = mk & (~mp); 

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mp = mp ^ (mp << 8); 
        mv4 = mp ^ (mp << 16) & _m; 
        _m = (_m ^ mv4) | (mv4 >> 4); 
        mk = mk & (~mp);

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mp = mp ^ (mp << 8); 
        mv8 = mp ^ (mp << 16) & _m; 
        _m = (_m ^ mv8) | (mv8 >> 8); 
        mk = mk & (~mp);

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mp = mp ^ (mp << 8); 
        mv16 = mp ^ (mp << 16) & _m; 
        _m = (_m ^ mv8) | (mv16 >> 16); 
        mk = mk & (~mp);
    }
};

template<MaxQubit MQ>
class Shape{
public:
    Shape(): mup{0}, mdown{0}{}
    Shape(const mask<MQ>& up, const mask<MQ>& down): mup{up}, mdown{down} { }
    Shape(const Qubits& up, const Qubits& down): mup{up.get_qubits()}, mdown{down.get_qubits()} { }
    Shape(const Shape& sh): Shape(sh.mup, sh.mdown) {}
    size_t size() const { return mup.nq + mdown.nq; }
    std::string str() const {
        Str str;
        Uint up = mup.msk();
        Uint i = 0;
        str += "up  : ";
            while (up > 0){
                if ((up&1) == 1)
                    str += std::to_string(i) + " ";
                i++;
                up=up>>1;
            }
        i = 0;
        up = mdown.msk();
        str += "\ndown: ";
            while (up > 0){
                if ((up&1) == 1)
                    str +=  std::to_string(i) + " ";
                i++;
                up=up>>1;
            }
        str += "\n";
        return str;
    }
    mask<MQ> mup;
    mask<MQ> mdown;
};

// template<> 
// class Shape<MaxQubit::Q1>;
// template<> 
// class Shape<MaxQubit::Q2>;
// template<> 
// class Shape<MaxQubit::Q4>;
// template<> 
// class Shape<MaxQubit::Q8>;
// template<> 
// class Shape<MaxQubit::Q16>;
// template<> 
// class Shape<MaxQubit::Q32>;

template<MaxQubit MQ>
Shape<MQ> prod(const Shape<MQ>& shl, const Shape<MQ> & shr){
    int intersect = ( shl.mdown.msk() & shr.mup.msk() );
    return Shape<MQ>(mask<MQ> (shl.mup.msk() | (shr.mup.msk() ^ intersect)), 
                 mask<MQ> (shr.mdown.msk() | (shl.mdown.msk() ^ intersect)));
}


template<MaxQubit MQ>
bool operator==(const Shape<MQ>& sl, const Shape<MQ>& sr){
    return sl.mup.msk() == sr.mup.msk() & sl.mdown.msk() == sr.mdown.msk();
}

template<MaxQubit MQ>
bool operator!=(const Shape<MQ>& sl, const Shape<MQ>& sr){
    return sl.mup.msk() != sr.mup.msk() or sl.mdown.msk() != sr.mdown.msk();
}

template<MaxQubit MQ>
std::ostream& operator<<(std::ostream& os, const Shape<MQ>& shape){
    return os << shape.str();
}


#endif