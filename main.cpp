#include <iostream>
#include <vector>

#include <Eigen/Dense>

#include "funnels/utils.hh"

#include "funnels/distances.hh"
#include "funnels/dynamics.hh"
#include "funnels/funnels.hh"

using namespace std;
using namespace Eigen;
using namespace funnels;
using namespace lyapunov;

void test_dist(){
  
  Matrix3d M;
  M(0,0) = 1.;
  M(1,1) = 2.;
  M(2,2) = 3.;
  
  VectorXd v(3);
  v(0)=11.;
  v(1)=12.;
  v(2)=13.;
  
  partial_so2_dist_t my_dist;
  std::cout << my_dist.cp_Mv(M, v) << std::endl;
  std::cout << my_dist.cp_vM(v, M) << std::endl;
  std::cout << my_dist.cp_vv(v, v) << std::endl;
  std::cout << my_dist.cp_MM(M, M) << std::endl;
}

void test_dyn(){
  
  kinematic_2d_sys_t my_sys;
  VectorXd u(2);
  VectorXd t(10);
  MatrixXd x(4,10);
  
  t.setLinSpaced(10, 0., 10.);
  u(0)=1.;
  u(1)=1.33;
  x(0,0) = -5.55;
  x(1,0) = 6.55;
  
  
  my_sys.compute(x,u,t);
  
  std::cout << x << std::endl;
  std::cout << u << std::endl;
  std::cout << t << std::endl;
  
}

void test_funnel1(){
  
  std::cout << "funnel 1" << std::endl;
  
  clock_ta_t &ctrl_clk = utils_ext::clock_map.create_and_get("c_t");
  clock_ta_t &lcl_clk = utils_ext::clock_map.create_and_get("c_h");

  kinematic_2d_sys_t my_dyn;
  partial_so2_dist_t my_dist;

  MatrixXd P(my_dyn._dimx, my_dyn._dimx);
  for (size_t i=0; i<my_dyn._dimx; i++){
    P(i,i) = (double) i+1.;
  }

  fixed_ellipsoidal_lyap_t my_lyap(P, 0.5, my_dist);
  
  process_t &my_proc = utils_ext::process_map.create_and_get("proc_0");
  
  // Init
  Vector4d x0;
  Vector2d u0;
  x0(0) = 0.;
  x0(1) = 2.;
  u0(0) = 1.1;
  u0(1) = 0.66;
  const double t0=0., t1=10.;
  
  funnel_t<fixed_ellipsoidal_lyap_t<partial_so2_dist_t>, kinematic_2d_sys_t>
      my_fun(100, "fun_0", my_proc, my_dyn, P, 0.5, my_dist);
  
  my_fun.compute(x0, t0, t1, u0);
  
  std::cout << my_fun.x() << std::endl;
  std::cout << my_fun.u() << std::endl;
  std::cout << my_fun.t() << std::endl;
  
}

void test_funnel_sys(){
  
  std::cout << "funnel sys" << std::endl;
  
  using fun_t = funnel_t<fixed_ellipsoidal_lyap_t<partial_so2_dist_t>,
      kinematic_2d_sys_t>;
  
  using fun_ptr_t = shared_ptr<fun_t>;
  
  clock_ta_t &ctrl_clk = utils_ext::clock_map["c_t"];
  clock_ta_t &lcl_clk = utils_ext::clock_map["c_h"];
  
  kinematic_2d_sys_t my_dyn;
  partial_so2_dist_t my_dist;
  
  MatrixXd P(my_dyn._dimx, my_dyn._dimx);
  for (size_t i=0; i<my_dyn._dimx; i++){
    P(i,i) = (double) 1.;
  }
  
  fixed_ellipsoidal_lyap_t my_lyap(P, 0.5, my_dist);
  
  process_t &my_proc = utils_ext::process_map["proc_0"];
  
  // Init
  Vector4d x0, x1;
  Vector2d u0, u1;
  x0(0) = -1.; x1(0) =  0.;
  x0(1) =  0.; x1(1) = -1.;
  u0(0) = 1.; u0(0) = 1.1;
  u0(1) = 0.66; u0(1) = 0.66;
  const double t0=0., t1=10.;
  
  fun_ptr_t my_fun0_0 = make_shared<fun_t>(100, "fun_0_0", my_proc, my_dyn, P,
      0.5,
      my_dist);
  
  fun_ptr_t my_fun1_0 = make_shared<fun_t>(100, "fun_1_0", my_proc, my_dyn,
      2.*P,
      0.5, my_dist);
  
  fun_ptr_t my_fun1_1 = make_shared<fun_t>(100, "fun_1_1", my_proc, my_dyn,
      4.*P, 0.5, my_dist);
  
  my_fun0_0->compute(x0, t0, t1, u0);
  my_fun1_0->compute(x1, t0, t1, u1);
  
  fun_t::set_parent(my_fun1_0, my_fun1_1);
  
  std::cout << my_fun0_0.use_count() << " ; " << my_fun1_0.use_count()
      << " ; " << my_fun1_1.use_count() << std::endl;
  
  
  
}




int main() {
  
  test_dist();
  test_dyn();
  test_funnel1();
  test_funnel_sys();
  
  return 0;
}