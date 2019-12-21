//
// Created by philipp on 12/13/19.
//

#include "funnels/transitions.hh"

transition_t::transition_t(size_t id, size_t src_id, size_t tgt_id,
    double *guard, double reset) :
    _id(id), _src_id(src_id), _tgt_id(tgt_id),
    _reset(reset){
  set_guard(guard);
}

transition_t::transition_t(const json &json_data) {
  from_json(json_data);
}

void transition_t::set_guard(const double guard[2]) {
  _guard[0]=guard[0];
  _guard[1]=guard[1];
}

void transition_t::set_reset(const double reset){
  _reset=reset;
}

json transition_t::to_json() const{
  json json_data = {
      {"id",_id},
      {"src_id", _src_id},
      {"tgt_id", _tgt_id},
      {"guard", {_guard[0], _guard[1]}},
      {"reset", _reset}
  };
  return json_data;
}

void transition_t::from_json(const json &json_data) {
  _id = (size_t) json_data["id"];
  _src_id = (size_t) json_data["src_id"];
  _tgt_id = (size_t) json_data["tgt_id"];
  _guard[0] = (double) json_data["guard"][0];
  _guard[1] = (double) json_data["guard"][1];
  _reset = (double) json_data["reset"];
}