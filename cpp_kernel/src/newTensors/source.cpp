#include "typedef.hpp"
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

// -----------------------specialization of mask--------------------------
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
    mask(const mask& ms): m{ms.m}, mv1{ms.mv1} {} 
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
    mask(const mask& ms): m{ms.m}, mv1{ms.mv1}, mv2{ms.mv2} {} 
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
    mask(const mask& ms): m{ms.m}, mv1{ms.mv1}, mv2{ms.mv2}, mv4{ms.mv4} {} 
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
    mask()=default;
    explicit mask(Uint m): m{m} { set_mv(); }
    mask(const mask& ms): m{ms.m}, mv1{ms.mv1}, mv2{ms.mv2}, mv4{ms.mv4}, mv8{ms.mv8}  {} 
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
        Uint _m = m;
        Uint mk = (~_m) << 1;
        Uint mp;

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mv1 = mp ^ (mp << 8) & _m; 
        mk = mk & (~mp); 

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mv2 = mp ^ (mp << 8) & _m; 
        _m = (_m ^ mv2) | (mv2 >> 2); 
        mk = mk & (~mp); 

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mv4 = mp ^ (mp << 8) & _m; 
        _m = (_m ^ mv4) | (mv4 >> 4); 
        mk = mk & (~mp);

        mp = mk ^ (mk << 1); 
        mp = mp ^ (mp << 2); 
        mp = mp ^ (mp << 4); 
        mv8 = mp ^ (mp << 8) & _m; 
        _m = (_m ^ mv8) | (mv8 >> 8); 
        mk = mk & (~mp);
    }
};


template<>
struct mask<MaxQubit::Q32>{
    mask()=default;
    explicit mask(Uint m): m{m} { set_mv(); }
    mask(const mask& ms): m{ms.m}, mv1{ms.mv1}, mv2{ms.mv2}, mv4{ms.mv4}, mv8{ms.mv8}, mv16{ms.mv16}  {} 
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