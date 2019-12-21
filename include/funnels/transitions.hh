//
// Created by philipp on 12/13/19.
//

#ifndef FUNNELS_CPP_TRANSITIONS_HH
#define FUNNELS_CPP_TRANSITIONS_HH

#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

class transition_t{
public:
  transition_t()=default;
  transition_t(size_t id, size_t src_id, size_t tgt_id,
      double guard[2], double reset);
  explicit transition_t(const json &json_data);
  
  void set_guard(const double guard[2]);
  void set_reset(const double reset);
  
  [[nodiscard]] json to_json()const;
  void from_json(const json &json_data);
  
  template<class ID2STR_T, class ID2STR_S, class ID2ACTION>
  tuple<string, string, string, string> to_checker(string &P,
      ID2STR_T &id_func_t, ID2STR_S &id_func_s, ID2ACTION &id_act,
      size_t time_step, string &ctrl_clk){
    
    string str_discrete = "edge:";
    str_discrete += P + ":";
    str_discrete += id_func_s(_src_id) + ":";
    str_discrete += id_func_s(_tgt_id) + ":";
    str_discrete += id_act(_id) + ":";
    
    string str_guard = to_string(ceil(time_step*_guard[0])) + "<=" + ctrl_clk;
    str_guard += ctrl_clk + "<=" + to_string(floor(time_step*_guard[1]));
    
    string str_reset = ctrl_clk + "=" + to_string(_reset);
    
    string str_comments = "id : " + to_string(_id) + "; name : " + id_func_t(_id);
    
    return make_tuple(str_discrete, str_guard, str_reset, str_comments);
  }
  
  

protected:
  size_t _id;
  size_t _src_id;
  size_t _tgt_id;
  double _guard[2];
  double _reset;
};

#endif //FUNNELS_CPP_TRANSITIONS_HH
