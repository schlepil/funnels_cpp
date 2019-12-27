//
// Created by philipp on 12/13/19.
//

#ifndef FUNNELS_CPP_FUNNELS_HH
#define FUNNELS_CPP_FUNNELS_HH

#include <vector>
#include <deque>

#include "Eigen/Dense"

#include "transitions.hh"
#include "funnel_computations.hh"

namespace funnels {
  
  template<class LYAP, class DYN>
  class funnel_t : public LYAP {
  public:
    using lyap_t = LYAP;
    using dyn_t = DYN;
    using fun_t = funnel_t<LYAP, DYN>;
    using fun_ptr_t = std::shared_ptr<fun_t>;
    
    using matrix_t = Eigen::MatrixXd;
    using vector_t = Eigen::VectorXd;
    using matrix_ptr_t = std::shared_ptr<matrix_t>;
    using vector_ptr_t = std::shared_ptr<vector_t>;
  
    void set_parent(const fun_ptr_t &other){
      // todo : some asserts
      _parent = other;
      other->_child = fun_ptr_t(this);
      _x = other->_x;
      _u = other->_u;
      _t = other->_t;
    }
  
    static void set_parent(fun_ptr_t &parent, fun_ptr_t &child){
      // todo : some asserts
      parent->_child = child;
      child->_parent = parent;
      child->_x = parent->_x;
      child->_u = parent->_u;
      child->_t = parent->_t;
    }
    
    template<class ...ARGS>
    funnel_t(size_t n, const std::string & name, const process_t &proc,
        const DYN &dyn, ARGS &&...args):
        _dimx(dyn._dimx), _dimu(dyn._dimu),
        _loc(utils_ext::loc_map.create_and_get(name, proc)), _n(n), _dyn(dyn),
        LYAP(args...) {};
    
    matrix_t & x(){
      return *_x;
    }
    vector_t & t(){
      return *_t;
    }
    vector_t & u(){
      return *_u;
    }
    const location_t &loc(){
      return _loc;
    }
    
    void set_x(matrix_ptr_t &x_ptr){
      assert((*x_ptr).rows()==_dimx);
      assert((*x_ptr).cols()==_n);
      _x =x_ptr;
    }
    void set_u(vector_ptr_t &u_ptr){
      assert((*u_ptr).size()==_dimu);
      _u =u_ptr;
    }
    void set_t(vector_ptr_t &t_ptr){
      assert((*t_ptr).size()==_n);
      _t =t_ptr;
    }
  
    void compute(const Eigen::VectorXd &x0, double t0, double t1,
        const Eigen::VectorXd &u_in){
  
      _x = matrix_ptr_t( new matrix_t(_dimx, _n));
      x().col(0) = x0;
      _u = vector_ptr_t( new vector_t(_dimu));
      u() = u_in;
      _t = vector_ptr_t( new vector_t(_n));
      t() = Eigen::VectorXd::LinSpaced(_n, t0, t1);
  
      // Compute the trajectory
      _dyn.compute(x(), u(), t());
    }
    
    bool operator==(const fun_t & other)const{
      return _loc == other._loc;
    }
  
    bool operator==(const fun_ptr_t & other)const{
      return _loc == other->_loc;
    }
    
    bool in_family(const fun_ptr_t &other)const{
      if (this->operator==(other)){
        return true;
      }
      fun_ptr_t this_fun = this->_child;
      while (this_fun!= nullptr){
        if(this_fun->operator==(other)){
          return true;
        }
        this_fun = this_fun->_child;
      }
      this_fun = this->_parent;
      while (this_fun!= nullptr){
        if(this_fun->operator==(other)){
          return true;
        }
        this_fun = this_fun->_parent;
      }
      return false;
    }

  public:
    std::deque<edge_t> _edges;
  
  protected:
    const size_t _dimx;
    const size_t _dimu;
    const size_t _n;
    
    const location_t &_loc;
    fun_ptr_t _parent;
    fun_ptr_t _child;
    
    matrix_ptr_t _x;
    vector_ptr_t _u;
    vector_ptr_t _t;
    const dyn_t &_dyn;
  };
  
  template <class FUNNEL>
  class funnel_sys_t{
    
    using fun_t = typename FUNNEL::fun_t;
    using fun_ptr_t = typename FUNNEL::fun_ptr_t;
    
    funnel_sys_t(const clock_ta_t &ctrl_clk, const clock_ta_t &lcl_clk):
        _ctrl_clk(ctrl_clk), _lcl_clk(lcl_clk){};
    
    void add_funnel(fun_ptr_t &funnel,
        bool is_parent=false){
      _all_funnels.push_back(funnel);
      if(is_parent){
        _parent_funnels.push_back(funnel);
      }
    }
    
    void generate_transitions(const size_t t_step,
        bool do_converging, bool do_lcl_switch){
      // Generate all internal transitions
      // only switching and converging transitions are taken
      // into account
      // But first delete all
      for (auto a_funnel : _all_funnels){
        a_funnel->_edges.clear();
      }
      
      // Construct the converging transitions
      if (do_converging){
        fun_ptr_t current_parent;
        fun_ptr_t current_child;
        for (size_t k=0; k<_parent_funnels.size(); ++k){
          current_parent = _parent_funnels[k];
          while (current_parent!= nullptr){
            current_child = current_parent._child;
            while (current_child!= nullptr){
              compute_converging_trans(*current_parent, *current_child,
                                        t_step, _ctrl_clk, _lcl_clk);
            }
            current_parent=current_parent._child;
          }
        }
      }
      
      // Construct switching transitions
      for (auto src_fun : _all_funnels){
        for (auto tgt_fun : _all_funnels){
          if (!do_lcl_switch && src_fun->in_family(tgt_fun)){
            // Do not compute transitions between funnels along
            // the same ref-traj
            continue;
          }
          compute_switching_trans(*src_fun, *tgt_fun,
                                  t_step, _ctrl_clk, _lcl_clk);
        }
      }
    }


  protected:
    std::deque<fun_ptr_t> _all_funnels;
    std::deque<fun_ptr_t> _parent_funnels;
    const clock_ta_t &_ctrl_clk, &_lcl_clk;
  };
}

#endif //FUNNELS_CPP_FUNNELS_HH
