#include <iostream>

#include "funnels/utils.hh"

using namespace funnels;

int main() {
  
  clock_ta_t & my_dummy = clock_map.create_and_get("");
  clock_ta_t & my_clock = clock_map.create_and_get("x");
  process_t & my_proc = process_map.create_and_get("P");
  location_t & my_loc = loc_map.create_and_get("l", my_proc);
  intvar_t & my_intvar = intvar_map.create_and_get("i", 0, 3, 1);
  
  expression_t a_expr = expression_t(my_clock, "<=", 10);
  
  std::shared_ptr<invariant_t> a_attr(new invariant_t());
  a_attr->add_expr(a_expr);
  
  my_loc.add_attr(a_attr);
  
  std::cout << my_clock.declare() << std::endl;
  std::cout << my_loc.declare() << std::endl;
  
  return 0;
}