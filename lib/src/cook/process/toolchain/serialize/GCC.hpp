#ifndef HEADER_cook_process_toolchain_serialize_GCC_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_serialize_GCC_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include <ostream>
#include <vector>

namespace cook { namespace process { namespace toolchain { namespace serialize {

    void gcc_config(std::ostream & oss, const std::vector<Language> &languages, const std::string & compiler, const std::string &archiver, const std::string & linker);

} } } }

#endif


