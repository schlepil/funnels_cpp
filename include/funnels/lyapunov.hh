//
// Created by philipp on 22.12.19.
//

#ifndef FUNNELS_CPP_LYAPUNOV_HH
#define FUNNELS_CPP_LYAPUNOV_HH

#include <iostream>

#include <Eigen/Core>
#include <Eigen/Cholesky>
#include <cmath>

namespace lyapunov{
  
  
  template <class DERIVED>
  inline double projected_max_radius(const Eigen::MatrixBase<DERIVED> & C0,
                                     const Eigen::MatrixBase<DERIVED> & C1){
    // todo optimize for usage of triangular shape
    // use spezialised version to compute smallest eigenvalue only
    auto Cinv = C0.inverse();
    auto P1_prime = (Cinv.transpose()*(C1.transpose()*C1)*Cinv);
    double max_rad = 1./P1_prime.eigenvalues().real().array().minCoeff();
    return std::sqrt(max_rad);
  }
  
  struct lyap_zone_t{};
  
  class lyapunov_t{
  public:
    virtual bool intersect(const lyap_zone_t & other);
    virtual bool covers(const lyap_zone_t & other);
    virtual bool is_covered(const lyap_zone_t & other);
  };
  
  struct fixed_ellipsoidal_zone_t{
    virtual const Eigen::VectorXd & x0() const {
      throw std::runtime_error("Virtual");
      return Eigen::Vector2d::Zero();
    };
    virtual const Eigen::MatrixXd & C() const {
      throw std::runtime_error("Virtual");
      return Eigen::Matrix2d::Zero();
    };
  };
  
  struct fixed_ellipsoidal_zone_copied_t: public fixed_ellipsoidal_zone_t{
  public:
    fixed_ellipsoidal_zone_copied_t( const Eigen::VectorXd & x0,
        const Eigen::MatrixXd & C);
    const Eigen::VectorXd & x0() const {
      return _x0;
    }
    const Eigen::MatrixXd & C() const {
      return _C;
    }
    // (x-x0)^T.P.(x-x0) <= 1
    // ||C.(x-x0)||_2^2 <= 1
    Eigen::VectorXd _x0;
    Eigen::MatrixXd _C;
  };
  
  struct fixed_ellipsoidal_zone_ref_t: public fixed_ellipsoidal_zone_t{
  public:
    fixed_ellipsoidal_zone_ref_t( const Eigen::VectorXd & x0,
                                     const Eigen::MatrixXd & C);
    const Eigen::VectorXd & x0() const {
      return _x0;
    }
    const Eigen::MatrixXd & C() const {
      return _C;
    }
    // (x-x0)^T.P.(x-x0) <= 1
    // ||C.(x-x0)||_2^2 <= 1
    const Eigen::VectorXd &_x0;
    const Eigen::MatrixXd &_C;
  };
  
  // Does zone0 cover zone1
  template<class DIST>
  bool covers_helper(const fixed_ellipsoidal_zone_t &zone0,
                     const fixed_ellipsoidal_zone_t &zone1,
                     DIST &dist){
  
  
    // First compute the projected distance
    auto dy = zone0.C()*(dist.cp_vv(zone1.x0(), zone0.x0()));
    double dy_norm = dy.norm(); //l2 norm
    // center out of bounds
    if (dy_norm>=1.){
      return false;
    }
  
    // Compute radius
    return dy_norm+projected_max_radius(zone0.C(), zone1.C())<=1.;
  }
  
  template<class DIST>
  class fixed_ellipsoidal_lyap_t:public lyapunov_t{
  public:
    using dist_t = DIST;
    
    fixed_ellipsoidal_lyap_t(const Eigen::MatrixXd &P,
        double gamma, DIST &dist):
        _gamma(gamma), _dist(dist){
      // Compute cholesky
      Eigen::LLT<Eigen::MatrixXd> llt_pre_comp;
      llt_pre_comp.compute(P);
      _C = llt_pre_comp.matrixU();
    }
    
    const Eigen::MatrixXd &C() const {
      return _C;
    }
    double gamma() const {
      return _gamma;
    }
    
    // Conservative intersect
    // If true, the zone may intersect with this
    // If false, the zone does definitively not intersect
    template<class DERIVED>
    bool intersect(const Eigen::MatrixBase<DERIVED> &x0,
        const fixed_ellipsoidal_zone_t &zone) {
      // First compute the projected distance
      auto dy = _C*(_dist.cp_vv(zone.x0(), x0));
      double dy_norm = dy.norm(); //l2 norm
    
      if (dy_norm <= 1.){
        return true;
      }
    
      return dy_norm>=1.+projected_max_radius(_C, zone.C());
    }
  
    // Conservative cover
    // If true, zone is definitively covered by this
    // If false, it may not be covered
    template<class DERIVED>
    bool covers(const Eigen::MatrixBase<DERIVED> &x0, const fixed_ellipsoidal_zone_t &zone) {
      fixed_ellipsoidal_zone_ref_t this_zone(x0, _C);
      return covers_helper(this_zone, zone, _dist);
    }
  
    // Conservative cover
    // If true, this is definitively covered by zone
    // If false, it may not be covered
    template<class DERIVED>
    bool is_covered(const Eigen::MatrixBase<DERIVED> &x0, const fixed_ellipsoidal_zone_t &zone) {
      fixed_ellipsoidal_zone_ref_t this_zone(x0, _C);
      return covers_helper(zone, this_zone, _dist);
    }
    
    dist_t & dist(){
      return _dist;
    }
    
  protected:
    Eigen::MatrixXd _C;
    double _gamma;
    DIST &_dist;
  };
  
}

#endif //FUNNELS_CPP_LYAPUNOV_HH
