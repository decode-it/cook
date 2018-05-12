#ifndef HEADER_cook_chai_module_EnumHelper_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_module_EnumHelper_hpp_ALREADY_INCLUDED

#include <sstream>
#include "chaiscript/chaiscript_basic.hpp"

namespace cook { namespace chai { namespace module {

#define CREATE_WRAPPER_TYPE(TYPE) namespace wrapper { namespace { struct TYPE##_t {}; static const TYPE##_t s_##TYPE = {};  } }
#define EXPOSE_TYPE(TYPE) \
    ptr->add(chaiscript::user_type<wrapper::TYPE##_t>(), #TYPE); \
    ptr->add(chaiscript::user_type<TYPE>(), #TYPE); \
    ptr->add(chaiscript::fun([](const wrapper::TYPE##_t &) {return std::string(#TYPE); }), "to_string"); \
    ptr->add_global_const(chaiscript::const_var(wrapper::s_##TYPE), #TYPE); \
    ptr->add(chaiscript::fun([](TYPE v) { std::ostringstream oss; oss << v; return oss.str(); }), "to_string"); \
    ptr->add(chaiscript::fun([](TYPE lhs, TYPE rhs) { return lhs == rhs; }), "=="); \
    ptr->add(chaiscript::fun([](TYPE lhs, TYPE rhs) { return lhs != rhs; }), "!="); 
    
    
#define EXPOSE_VALUE(TYPE, VAL) ptr->add(chaiscript::fun([](const wrapper::TYPE##_t &) { return TYPE::VAL; }), #VAL); 

} } }

#endif

