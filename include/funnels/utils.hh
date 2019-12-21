//
// Created by philipp on 12/16/19.
//

#ifndef FUNNELS_CPP_UTILS_HH
#define FUNNELS_CPP_UTILS_HH

#include <string>
#include <cassert>
#include <unordered_map>
#include <deque>
#include <vector>
#include <memory>

namespace funnels{
  
  ////////////////////////
  
  class named_var_t{
  public:
    named_var_t(size_t id, const std::string &name);
    
    size_t id() const;
    
    const std::string& name() const;
    
    [[nodiscard]] virtual std::string declare() const;
  
  protected:
    const size_t _id;
    const std::string _name;
  };
  
  ////////////////////////
  
  class expression_t{
  public:
    expression_t(const named_var_t &lhs, std::string act,
        size_t _value=0,
        std::vector<std::pair<std::string, named_var_t&>> rhs=
        std::vector<std::pair<std::string, named_var_t&>>());
    
    expression_t(const expression_t &expr)=default;
    expression_t(expression_t &&expr)=default;
    
    void add_rhs(std::string act, named_var_t &rhs);
    
    std::string eval_str() const;
  
  protected:
    const named_var_t &_lhs;
    const std::string _act;
    const size_t _value;
    std::vector<std::pair<std::string, named_var_t&>> _rhs;
  };
  
  ////////////////////////
  
  class attribute_t{
  public:
    void virtual add_expr(const expression_t &expr){
      throw std::runtime_error("Virtual");
    };
    [[nodiscard]] virtual std::string eval_str() const{
      throw std::runtime_error("Virtual");
    }
  };
  
  ////////////////////////
  
  class initial_t final: public attribute_t{
  public:
    std::string eval_str() const{
      return "initial::";
    }
  };
  
  ////////////////////////
  
  class commited_t final: public attribute_t{
  public:
    std::string eval_str() const{
      return "commited::";
    }
  };
  
  ////////////////////////
  
  class urgent_t final: public attribute_t{
  public:
    std::string eval_str() const{
      return "urgent::";
    }
  };
  
  ////////////////////////
  
  class labels_t final: public attribute_t{
  public:
    labels_t(const std::vector<std::string> labels);
  
    std::string eval_str() const;

  protected:
    const std::vector<std::string> _labels;
  };
  
  ////////////////////////
  
  class expr_list_attr_t: public attribute_t{
  public:
    expr_list_attr_t(){};
    expr_list_attr_t(std::vector<expression_t> expr);
    
    void add_expr(const expression_t &expr);
    [[nodiscard]] std::string eval_str(const std::string &id_string) const;
  
  protected:
    std::vector<expression_t> _expr;
  };
  
  ////////////////////////
  
  class invariant_t final: public expr_list_attr_t{
  public:
    invariant_t(){};
    invariant_t(std::vector<expression_t> expr):
    expr_list_attr_t(expr){};
    
    std::string eval_str() const{
      return expr_list_attr_t::eval_str(_id_str);
    };
  protected:
    static const std::string _id_str;
  };
  
  ////////////////////////
  
  class provided_t final: public expr_list_attr_t{
  public:
    provided_t(){};
    provided_t(std::vector<expression_t> expr):
        expr_list_attr_t(expr){};
    
    std::string eval_str() const{
      return expr_list_attr_t::eval_str(_id_str);
    };
  protected:
    static const std::string _id_str;
  };
  
  ////////////////////////
  
  class event_t final:public named_var_t{
  public:
    event_t(size_t id, const std::string &name);
    
    [[nodiscard]] std::string declare() const;
  };

  ////////////////////////
  
  class process_t final:public named_var_t{
  public:
    process_t(size_t id, const std::string &name);
    
    [[nodiscard]] std::string declare() const;
  };
  
  ////////////////////////
  
  class location_t final:public named_var_t{
  public:
    location_t(size_t id, const std::string &name,
        const process_t & process);
    
    [[nodiscard]] const std::string &proc_name()const;
    void add_attr(std::shared_ptr<attribute_t> attr_ptr);
  
    [[nodiscard]] std::string declare() const;

  protected:
    const process_t & _process;
    std::vector<std::shared_ptr<attribute_t>> _attr;
  };
  
  ////////////////////////
  
  class clock_ta_t final:public named_var_t{
  public:
    clock_ta_t(size_t id, const std::string &name);
  
    [[nodiscard]] std::string declare() const{
      return "clock:1:"+name();
    }
  };
  
  ////////////////////////
  
  class intvar_t final:public named_var_t{
  public:
    intvar_t(size_t id, const std::string &name,
        size_t min_v, size_t max_v, size_t init);
  
    [[nodiscard]] std::string declare() const{
      return "int:1:"+std::to_string(_min_v)+":"+std::to_string(_max_v)+
        ":"+std::to_string(_init)+":"+name();
    }

  protected:
    const size_t _min_v, _max_v, _init;
  };
  
  ////////////////////////
  
  template <class T>
  class named_var_map_t{
  public:
    T &operator[](const std::string &name){
      return *_name2elem.at(name);
    }
    T &operator[](size_t id){
      return *_id2elem.at(id);
    }
    
    template <class ... ARGS>
    T & create_and_get(const std::string &name, const ARGS & ...args){
      assert(_name2elem.find(name) == _name2elem.end() && "Not allowed to exist");
      size_t this_id = _next_id;
      
      _objs.emplace_back(this_id, name, args ...);
      _name2elem[name] = &_objs.back();
      _id2elem[this_id] = &_objs.back();
      
      _next_id++;
      
      return _objs.back();
    }

  protected:
    std::unordered_map<size_t, T*> _id2elem;
    std::unordered_map<std::string, T*> _name2elem;
    std::deque<T> _objs;
    size_t _next_id = 0;
  };
  
  ////////////////////////
  
  extern named_var_map_t<clock_ta_t> clock_map;
  extern named_var_map_t<location_t> loc_map;
  extern named_var_map_t<intvar_t> intvar_map;
  extern named_var_map_t<process_t> process_map;
  
  
  
  
} //funnels

#endif //FUNNELS_CPP_UTILS_HH
