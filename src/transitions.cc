//
// Created by philipp on 12/13/19.
//

#include "funnels/transitions.hh"

namespace funnels{
  
  edge_t::edge_t(const location_t &src, const location_t &tgt,
                 const event_t &event):
      _src(src), _tgt(tgt), _event(event){};
  
  provided_t & edge_t::guard() {
    return _guard;
  }
  
  do_t & edge_t::update() {
    return _update;
  }
  
  std::string edge_t::declare(){
    std::string temp_str = "edge:";
    temp_str += _src.name() + ":";
    temp_str += _tgt.name() + ":";
    temp_str += _event.name() + "{";
    temp_str += _guard.eval_str();
    temp_str += _update.eval_str();
    temp_str.pop_back();
    temp_str += "}";
    return temp_str;
  }
  
  edge_t switching_transition(const location_t &src, const location_t &tgt,
      const event_t &event, const clock_ta_t &ctrl_clk, const clock_ta_t &lcl_clk,
      size_t alpha, size_t beta, size_t zeta){
    
    edge_t this_edge(src, tgt, event);
    
    this_edge.guard().add_expr(expression_t(ctrl_clk, ">=", alpha));
    this_edge.guard().add_expr(expression_t(ctrl_clk, "<=", beta));
    
    this_edge.update().add_expr(expression_t(ctrl_clk, "=", zeta));
    this_edge.update().add_expr(expression_t(lcl_clk, "=", 0));
    
    return this_edge;
  }
  
  edge_t converging_transition(const location_t &src, const location_t &tgt,
                               const event_t &event, const clock_ta_t &lcl_clk,
                               size_t alpha, bool is_exact){
  
    edge_t this_edge(src, tgt, event);
    if(is_exact){
      this_edge.guard().add_expr(expression_t(lcl_clk, "==", alpha));
    }else{
      this_edge.guard().add_expr(expression_t(lcl_clk, ">=", alpha));
    }
    this_edge.update().add_expr(expression_t(lcl_clk, "=", 0));
  
    return this_edge;
  }
  
}