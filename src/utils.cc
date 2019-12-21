//
// Created by philipp on 12/16/19.
//

#include "funnels/utils.hh"

#include <algorithm>

namespace funnels {
  
  const std::vector<std::string> all_expressions__(
      {"=", "+", "-", "*", "/", "%",
          "==", "!=", "<=", "<", ">", ">="
          }
      );
  
  const std::vector<std::string> all_ieq__(
      {"<=", "<", ">", ">="}
  );
  
  bool check_op__(const std::string & op_){
    return std::find(all_expressions__.cbegin(), all_expressions__.cend(),
        op_)!=all_expressions__.end();
  }
  
  bool check_ieq_op__(const std::string & op_){
    return std::find(all_ieq__.cbegin(), all_ieq__.cend(),
                     op_)!=all_ieq__.end();
  }
  
  ////////////////////////
  
  named_var_t::named_var_t(size_t id, const std::string &name) :
      _id(id), _name(name) {}
  
  size_t named_var_t::id() const {
    return _id;
  }
  
  const std::string &named_var_t::name() const {
    return _name;
  }
  
  std::string named_var_t::declare() const {
    throw std::runtime_error("Virtual");
  }
  
  ////////////////////////
  
  expression_t::expression_t(const funnels::named_var_t &lhs,
      std::string act, size_t value,
      std::vector<std::pair<std::string, named_var_t &>> rhs):
      _lhs(lhs), _act(std::move(act)), _value(value), _rhs(std::move(rhs)){
    assert(check_op__(_act));
    for (const auto & a_rhs : _rhs){
      assert(check_op__(a_rhs.first));
    }
  }
  
  void expression_t::add_rhs(std::string act, funnels::named_var_t &rhs) {
    assert(check_op__(act));
    _rhs.emplace_back(std::move(act), rhs);
  }
  
  std::string expression_t::eval_str() const {
    std::string temp_str = _lhs.name()+_act+std::to_string(_value);
    for (const auto & v: _rhs){
      temp_str += v.first + v.second.name();
    }
    return temp_str;
  }
  
  ////////////////////////
  
  labels_t::labels_t(const std::vector<std::string> labels):
      _labels(std::move(labels)){}
  
  ////////////////////////
  
  expr_list_attr_t::expr_list_attr_t(std::vector<expression_t> expr):
  _expr(std::move(expr)){}
  
  void expr_list_attr_t::add_expr(const expression_t &expr) {
    _expr.push_back(expr);
  }
  
  std::string expr_list_attr_t::eval_str(const std::string & id_string) const {
    std::string temp_str="";
    if (_expr.size()) {
      temp_str += id_string;
      for (const auto &v : _expr) {
        temp_str += v.eval_str() + "&&";
      }
      temp_str.pop_back();
      temp_str.pop_back();
      temp_str += ":";
    }
    return temp_str;
  }
  
  const std::string invariant_t::_id_str = "invariant:";
  const std::string provided_t::_id_str = "provided:";
  
  ////////////////////////
  
  std::string labels_t::eval_str() const{
    std::string temp_str = "labels:";
    for (const auto & v:_labels){
      temp_str += v+",";
    }
    temp_str.pop_back();
    return temp_str;
  }
  
  ////////////////////////
  
  event_t::event_t(size_t id, const std::string &name) :
      named_var_t(id, name) {}
  
  std::string event_t::declare() const {
    return "event:" + name();
  }
      
  ////////////////////////
  
  process_t::process_t(size_t id, const std::string &name) :
      named_var_t(id, name) {}
  
  std::string process_t::declare() const {
   return "process:"+name();
  }
  
  ////////////////////////
  
  location_t::location_t(size_t id, const std::string &name,
                         const process_t &process) :
      named_var_t(id, name),
      _process(process) {}
  
  const std::string &location_t::proc_name() const{
    return _process.name();
  }
  
  void location_t::add_attr(std::shared_ptr<attribute_t> attr_ptr) {
    _attr.push_back(attr_ptr);
  }
  
  std::string location_t::declare() const{
    std::string temp_string = "location:"+proc_name()+":"+name()+"{";
    for (const auto & v:_attr){
      temp_string += v->eval_str();
    }
    temp_string.pop_back();
    temp_string += "}";
    return temp_string;
  }
  
  ////////////////////////
  
  clock_ta_t::clock_ta_t(size_t id, const std::string &name) :
      named_var_t(id, name) {}
  
  ////////////////////////
  
  intvar_t::intvar_t(size_t id, const std::string &name,
                     size_t min_v, size_t max_v, size_t init) :
      named_var_t(id, name),
      _min_v(min_v), _max_v(max_v), _init(init) {
    assert(_min_v<=_init && _init<=_max_v && "out of range");
  }
  
  ////////////////////////
  
  named_var_map_t<clock_ta_t> clock_map;
  named_var_map_t<location_t> loc_map;
  named_var_map_t<intvar_t> intvar_map;
  named_var_map_t<process_t> process_map;

}


