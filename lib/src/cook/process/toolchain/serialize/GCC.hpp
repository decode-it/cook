#ifndef HEADER_cook_process_toolchain_serialize_GCC_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_serialize_GCC_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include <ostream>
#include <map>

namespace cook { namespace process { namespace toolchain { namespace serialize {

    void gcc_config(std::ostream & oss, const std::map<Language, std::string> & compilers, const std::string & linker);

} } } }

#endif


