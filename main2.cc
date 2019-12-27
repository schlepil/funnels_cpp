//
// Created by phili on 26.12.2019.
//

#define _USE_MATH_DEFINES
#include <iostream>
#include <Eigen/Dense>
#include <cmath>

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

using namespace Eigen;

double my_mod(double x){
  return std::fmod(x, 2.5);
}

int main(){
  
  VectorXd v(3);
  MatrixXd M(3,3);
  MatrixXd M2(6,6);
  
  Vector3d v3;
  
  v(0) = 0.;
  v(1) = 1.;
  v(2) = 2.;
  M(0,0) = 10.;
  M(1,1) = 20.;
  M(2,2) = 33.;
  
  std::cout << v << std::endl;
  std::cout << M << std::endl;
  
  auto res = -M;
  M.colwise() += v;
  
  auto res2 = (-M).colwise() + v;
  res2.row(1).unaryExpr(&my_mod);
  
  std::cout << res << std::endl;
  std::cout << res2 << std::endl;
  
  M2.block(3,3,3,3).colwise() = v;
  
  std::cout << M2 << std::endl;
  
  v3 = M2.block(1,2,3,1);
  
  MatrixXd AA = v3.colwise()-v;
  
  std::cout << v3 << std::endl;
  std::cout << M_PI << std::endl;
  std::cout << AA << std::endl;
  
  return 0;
}