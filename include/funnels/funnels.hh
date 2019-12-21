//
// Created by philipp on 12/13/19.
//

#ifndef FUNNELS_CPP_FUNNELS_HH
#define FUNNELS_CPP_FUNNELS_HH

#include <vector>

#include "Eigen/Dense"

#include "transitions.hh"


template <template<size_t> class LYAP, size_t DIMX, size_t DIMU, size_t NT>
class funnel_t: public LYAP<DIMX>{
  
  funnel_t(){};
  


private:
  Eigen::Matrix<double, DIMX, NT, Eigen::ColMajor> _x;
  Eigen::Matrix<double, DIMU, NT, Eigen::ColMajor> _u;
  Eigen::Matrix<double, 1, NT, Eigen::RowMajor> _t;
};

#endif //FUNNELS_CPP_FUNNELS_HH
