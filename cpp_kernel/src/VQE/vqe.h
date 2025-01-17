#ifndef _VQE
#define _VQE

#include<utility>

#include "nlopt.hpp"

#include "instruction.h"
#include "q_errors.h"


typedef std::vector<double> DoubleVec;


namespace opt{
    class Callback{
    public:
        virtual void general_write(DoubleVec, double){};
        virtual void sub_write(DoubleVec, double){};
    };

    class EmptyCallback : public Callback{
    public:
        EmptyCallback(){}
    };


    class Optimizer{
    public:
        // Optimizer(MetaProvider& mp, const ParameterVector& pv):  mp{mp}, pv{pv} { cb=EmptyCallback{}; }
        Optimizer(MetaProvider& mp, ParameterVector& pv, Callback cb): cb{cb}, mp{mp}, pv{pv} {}
        
        virtual std::pair<DoubleVec, double> optimize(int max_iter){
            int i = 0;
            SetUp();
            auto res = _update();
            while((!is_to_stop(res)) & (i<max_iter)){
                pv.set_row_values(res.first);
                cb.general_write(res.first, res.second);
                i++;
                res = _update();
                pv.set_row_values(res.first);
            }
            return res;
        };

        virtual ~Optimizer() {}
    protected:
        virtual std::pair<DoubleVec, double> _update() = 0;
        virtual void SetUp() = 0;
        virtual bool is_to_stop(std::pair<DoubleVec, double> data) = 0;
        Callback cb;
        MetaProvider& mp;
        ParameterVector& pv;
    };


    struct quantum_obj_data{
        MetaProvider& mp;
        ParameterVector& pv;
        DoubleVec res_array{};
        static double eval_cost(const std::vector<double> &x, std::vector<double> &grad, void *data){
            quantum_obj_data *d = reinterpret_cast<quantum_obj_data *>(data);
            d->pv.set_row_values(x);
            double r = d->mp.evaluate_cost();
            d->res_array.push_back(r);
            // std::cout << "value_est: " << r << std::endl;
            return r;
        }
    };


    class Nlopt_Optimizer{
    public:
        Nlopt_Optimizer(const nlopt::opt& opt, MetaProvider& mp, ParameterVector& pv, int maxeval=10, double tol=1e-6): opt{opt}, qod{mp,pv}{
            this->opt.set_min_objective(quantum_obj_data::eval_cost, &qod);
            this->opt.set_ftol_rel(tol);
            this->opt.set_maxeval(maxeval);
            // this->opt. = 1;
        }
        nlopt::result optimize(std::vector<double>& init, double& res){
            qod.res_array = DoubleVec{};
            return this->opt.optimize(init, res);
        }
        DoubleVec get_res_array(){
            return qod.res_array;
        }
        nlopt::opt opt;
    private:
        quantum_obj_data qod;
    };
};
#endif