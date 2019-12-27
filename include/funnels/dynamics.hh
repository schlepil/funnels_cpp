//
// Created by philipp on 26.12.19.
//

#ifndef FUNNELS_CPP_DYNAMICS_HH
#define FUNNELS_CPP_DYNAMICS_HH

#include <Eigen/Core>
#include <cmath>
#include <cassert>

using namespace Eigen;

class kinematic_2d_sys_t{
public:
  constexpr static const size_t _dimx=4, _dimv=2, _dimu=2;
  
  static void compute(MatrixXd &x, const VectorXd &u, const VectorXd &t){
    assert(x.cols()==t.size());
    assert(u.rows()==_dimu && x.rows()==_dimx && u.rows()==_dimv);
    // State vector is [x,y,dx,dy]
    const Vector2d x0 = x.block<_dimx-_dimv,1>(0,0);
    size_t n = t.size();
    // Set velocity
    // For a kinematic sys, the velocity can change instantly
    x.block(_dimx-_dimv,0,_dimv,n).colwise() = u;
    // Compute state
    for (size_t i=0; i<n; i++){
      x.block<_dimx-_dimv,1>(0,i) = x0 + u*t(i);
    }
    // Done
  }
};
#endif //FUNNELS_CPP_DYNAMICS_HH
