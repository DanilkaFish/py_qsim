
#include "shape.h"
#include "thread"

template<typename E> class Conjugate; ///< Forward declaration of `Trasnpose` class
template<typename E> class Convulate; ///< Forward declaration of `Trasnpose` class

template<typename T>
class Expression{
public:
    Shape get_shape() const noexcept { return static_cast<T const *>(this)->get_shape(); }
    DataType operator[](int i) const {
		return (*static_cast<T const *>(this))[i];
	}

    Conjugate<T> conj() const noexcept {
        return Conjugate<T>(*this);
    }
};


template<typename T>
class Conjugate: public Expression<Conjugate<T>>{
public:
    Conjugate(const Expression<T>& expr): expr{&expr} { }
    Shape get_shape() const noexcept { return Shape(expr->get_shape().pos_down, expr->get_shape().pos_up); }
    DataType operator[](int i) const noexcept {  
        int nd =expr->get_shape().nu;
        int id = i & ((1<<nd) - 1); 
        // ((i-id) >> nd) + (id << nd)
        return std::conj((*expr)[i]); }
    const Expression<T>& conj() const noexcept { return (*expr); }
private:
    const Expression<T> *expr;
};


template<typename T>
class Convulate: public Expression<Convulate<T>>{
public:
    Convulate(const T& expr, const Qubits& qubs): expr{&expr}, dif{1 << qubs.size()} { 
        int qubs_int = qubs_to_int(qubs); 
        int pos_up = expr.get_shape().pos_up.msk();
        int pos_down = expr.get_shape().pos_down.msk();
        if (((pos_up | qubs_int) ^ pos_up != 0) | ((pos_down | qubs_int) ^ pos_down != 0)){
            throw QException("trying to conv by unexisted qubits");
        }
        shape = Shape(pos_up ^ qubs_int, pos_down ^ qubs_int);
        up_mask = mask(expr.get_shape().pos_up.compress(shape.pos_up));
        down_mask = mask(expr.get_shape().pos_down.compress(shape.pos_down));
    }
    Shape get_shape() const noexcept { return shape; }
    DataType operator[](int i) const { 
        DataType val=0;
        int id = ((shape.pos_up.msk() << shape.nd) + shape.pos_down.msk()) ^ i;
        for (int j=0; j < dif; j++){
            val += (*expr)[id + shape.pos_up.expand(j) << shape.nd + shape.pos_down.expand(j)];
        }
        return val;
    }
private:
    Shape shape;
    const T *expr;
    mask up_mask;
    mask down_mask;
    int dif;
};


template<typename Tl, typename Tr>
class TensorProd: public Expression<TensorProd<Tl, Tr>>{
public:
    TensorProd(const Tl& exprl, const Tr& exprr): exprl{&exprl}, 
                                                  exprr{&exprr}, 
                                                  shape{prod(exprl.get_shape(), exprr.get_shape())}
                                                { init(); }
    Shape get_shape() const noexcept { return shape; }
    DataType operator[](int i) const{
        int id = i & ( (1 << shape.nd) - 1);
        int iu = (i - id) >> shape.nd;
        int ir = (meru.expand(mciru.compress(iu)) << nrd)  + mcird.compress(id);
        int il = (mcilu.compress(iu) << nld) + meld.expand(mcild.compress(id));
        DataType val=0;
        for (int j=0; j < (dif); j++){
            val += (*exprl)[il + mejld.expand(j)] * 
                   (*exprr)[ir + (mejru.expand(j)<<nrd)];
        }
        return val;
    }
    void init(){
        nld = exprl->get_shape().nd;
        nrd = exprr->get_shape().nd;
        nlu = exprl->get_shape().nu;
        nru = exprr->get_shape().nu;
        mask mlu(exprl->get_shape().pos_up);
        mask mrd(exprr->get_shape().pos_down);
        mask mld(exprl->get_shape().pos_down);
        mask mru(exprr->get_shape().pos_up);
        int anti_intersect = ~(mld.msk()&mru.msk());

        mcilu = mask(shape.pos_up.compress(mlu.msk()));
        mcird = mask(shape.pos_down.compress(mrd.msk()));
        mcild = mask(shape.pos_down.compress(mld.msk() &anti_intersect));
        mciru = mask(shape.pos_up.compress(mru.msk()&anti_intersect));
        meld = mask(mld.compress(mld.msk() & anti_intersect));
        meru = mask(mru.compress(mru.msk() & anti_intersect));

        mejld = mask(meld.msk() ^ ( (1 << nld) - 1));
        mejru = mask(meru.msk() ^ ( (1 << nru) - 1));
        dif = 1<<(nlu + nru - shape.nu);
    }

// private:
    Shape shape;
    DataPtr dumpdptr;
    const Tl *exprl;
    const Tr *exprr;
    // const std::vector<int> dptrr;
    // const std::vector<int> dptrl;
    int dif;
    int nld;
    int nrd;
    int nlu;
    int nru;
    mask mcilu;
    mask mcird;
    mask mcild;
    mask mciru;
    mask meld;
    mask meru;
    mask mejld;
    mask mejru;
};

template<typename Tl, typename Tr>
class TensorSum: public Expression<TensorSum<Tl, Tr>>{
public:
    TensorSum(const Tl& exprl, const Tr& exprr): exprl{&exprl}, exprr{&exprr} {
        if (!(exprl.get_shape() == exprr.get_shape())) {
            std::cerr << "dif shapes" << exprl.get_shape() << exprr.get_shape();
        }
    }
    Shape get_shape() const noexcept { return exprl->get_shape(); }

    DataType operator[](int i) const{
        Shape sh = this->get_shape();
        return (*exprl)[i] 
             + (*exprr)[i];
    }
private:
    const Tl *exprl;
    const Tr *exprr;
};


template<typename Tl, typename Tr>
class TensorDiv: public Expression<TensorDiv<Tl, Tr>>{
public:
    TensorDiv(const Tl& exprl, const Tr& exprr): exprl{&exprl}, exprr{&exprr} { 
        if (!(exprl.get_shape() == exprr.get_shape())) {
            std::cerr << "dif shapes" << exprl.get_shape() << exprr.get_shape();
        }
     }
    Shape get_shape() const noexcept { return exprl->get_shape(); }

    DataType operator[](int i) const{
        Shape sh = this->get_shape();
        return (*exprl)[i] 
             - (*exprr)[i];
    }
private:
    const Tl *exprl;
    const Tr *exprr;
};


template<typename T1, typename T2>
TensorProd<Expression<T1>,Expression<T2>> operator*(const Expression<T1> &e1, const Expression<T2> &e2)
{
	return TensorProd<Expression<T1>, Expression<T2>>(e1, e2);
}


template<typename T1, typename T2>
TensorSum<Expression<T1>,Expression<T2>> operator+(const Expression<T1> &e1, const Expression<T2> &e2)
{
	return TensorSum<Expression<T1>, Expression<T2>>(e1, e2);
}


template<typename T1, typename T2>
TensorDiv<Expression<T1>, Expression<T2>> operator-(const Expression<T1> &e1, const Expression<T2> &e2)
{
	return TensorDiv<Expression<T1>, Expression<T2>>(e1, e2);
}