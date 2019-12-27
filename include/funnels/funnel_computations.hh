//
// Created by phili on 25.12.2019.
//

#ifndef FUNNELS_CPP_FUNNEL_COMPUTATIONS_HH
#define FUNNELS_CPP_FUNNEL_COMPUTATIONS_HH

#include <cmath>
#include <cassert>

#include "lyapunov.hh"
#include "transitions.hh"

using namespace funnels;
using namespace lyapunov;

template<class FUNNEL>
size_t compute_converging_trans_fixed_ellip(
    FUNNEL &src, const FUNNEL &tgt, size_t t_step,
    const clock_ta_t &lcl_clk){
  
  assert((src.gamma()>=0.) && (tgt.gamma()>=0.) && "Fixed size funnels "
                                                   "need to converge");
  
  // Only one transition necessary, convergence time is independent of
  // the trajectory and the ctrl_clk
  
  // Get projected radius
  double r_proj = lyapunov::projected_max_radius(src.C(), tgt.C());
  if (r_proj>=1.0){
    return 0; // No sense to switch to larger
  }
  
  double t_min = 1./src.gamma()*std::log(1./r_proj);
  
  // Create a new corresponding to
  // lcl_clk>=t_min; lcl_clk:=0
  // todo: is_exact hard-coded to true
  src._edges.push_back(converging_transition(src.loc(), tgt.loc(),
      utils_ext::event_map[""], lcl_clk, std::ceil(t_min*t_step), true));
  
  return 1;
}


template<class FUNNEL>
size_t compute_converging_trans(FUNNEL &src, const FUNNEL& tgt,
    size_t t_step, const clock_ta_t &ctrl_clk, const clock_ta_t &lcl_clk){
  using lyap_t = typename FUNNEL::lyap_t;
  using dist_t = typename lyap_t::dist_t;
  
  if (std::is_same<lyap_t, lyapunov::fixed_ellipsoidal_lyap_t<dist_t>>::value){
    return compute_converging_trans_fixed_ellip(src, tgt, t_step, lcl_clk);
  }else{
    throw std::runtime_error("Converging not implemented");
  }
}



/////////////////////////////////////////////////

inline size_t find_next_high(size_t start,
    const Eigen::Matrix<bool, Eigen::Dynamic, 1> vec){
  
  size_t idx = start;
  while(!vec(idx) && (idx<vec.size()-1)){
    idx++;
  }
  return idx;
}


inline size_t find_next_low(size_t start,
    const Eigen::Matrix<bool, Eigen::Dynamic, 1> vec){
  
  size_t idx = start;
  while(vec(idx && (idx<vec.size()-1))){
    idx++;
  }
  return idx;
}

/////////////////////////
template <class FUNNEL>
size_t compute_converging_trans_fixed_ellip(
    FUNNEL &src, const FUNNEL &tgt, size_t t_step,
    const clock_ta_t &ctrl_clk, const clock_ta_t &lcl_clk) {
  
  // Loop over each time-point in the source trajectory and
  // compute the corresponding target interval
  // todo find a way to minimize the number of transitions
  using fun_t = FUNNEL;
  using matrix_t = typename fun_t::matrix_t;
  using vector_t = typename fun_t::vector_t;
  
  const vector_t &t_src = src.t();
  const vector_t &t_tgt = tgt.t();
  const matrix_t & x_src = src.x();
  const matrix_t & x_tgt = tgt.x();
  Eigen::Matrix<bool, Eigen::Dynamic, 1> is_covered;
  
  size_t trans_count = 0;
  size_t n_alpha, n_beta, n_zeta;
  bool in_interval=false;
  
  double r_diff_square = lyapunov::projected_max_radius(tgt.C(), src.C());
  r_diff_square *= r_diff_square;
  r_diff_square = 1. -r_diff_square;
  
  // Scan over all tgt path for each point in src
  for (size_t i_in=0; i_in<t_tgt.size(); i_in++){
    n_zeta = i_in;
    is_covered = (tgt.C*(tgt.dist().cp_Mv(*x_src,VectorXd((*x_tgt).col(i_in)))))
        .colwise().squaredNorm()<=r_diff_square;
  
    n_alpha=0; n_beta=0;
    if (is_covered(0)){
      in_interval=true;
    }else{
      in_interval = false;
    }
    while(true){
      if(in_interval){
        // Find end of interval
        n_beta = find_next_low(n_alpha, is_covered);
        // Create transition
        src._edges.push_back(switching_transition(src.loc(), tgt.loc(),
            utils_ext::event_map[""], ctrl_clk, lcl_clk,
            std::ceil(t_step*t_src[n_alpha]),std::floor(t_step*t_src[n_beta]),
            std::round(t_step*t_tgt[n_zeta])));
        // Done with this interval
        in_interval = false;
        trans_count++;
      }else{
        n_alpha = find_next_high(n_beta, is_covered);
        in_interval = true;
      }
      // end this loop through src
      if((n_alpha==is_covered.size()-1) || (n_beta==is_covered.size()-1)){
        break;
      }
    }
  } // End this tgt
  
  return trans_count; // Done
}

template<class FUNNEL>
size_t compute_switching_trans(FUNNEL &src, const FUNNEL& tgt,
                                double t_step, const clock_ta_t &ctrl_clk, const clock_ta_t &lcl_clk){
  using lyap_t = typename FUNNEL::lyap_t;
  using dist_t = typename lyap_t ::dist_t;
  
  if (std::is_same<lyap_t, lyapunov::fixed_ellipsoidal_lyap_t<dist_t>>::value){
    return compute_switching_trans_fixed_ellip(src, tgt, t_step,
        ctrl_clk, lcl_clk);
  }else{
    throw std::runtime_error("Converging not implemented");
  }
}


#endif //FUNNELS_CPP_FUNNEL_COMPUTATIONS_HH