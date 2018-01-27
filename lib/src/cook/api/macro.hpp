#ifndef HEADER_cook_api_macro_hpp_ALREADY_DEFINED
#define HEADER_cook_api_macro_hpp_ALREADY_DEFINED

#include "gubg/stream.hpp"
#include <stdexcept>

#define CA_ASSERT_STRINGIFY_HELPER(x) #x
#define CA_ASSERT_STRINGIFY(x) CA_ASSERT_STRINGIFY_HELPER(x)
#define CA_ASSERT_STRING(cond) gubg::stream( [](auto & os) { os << "Fatal error: \"" << cond << "\" in file " << __FILE__ << ", line " << CA_ASSERT_STRINGIFY(__LINE__); })

#define FATAL(cond, error_type) if (cond) {} else { throw error_type(CA_ASSERT_STRING(#cond));  } do {} while (0)

#define RUNTIME_FATAL(cond) FATAL(cond, std::runtime_error)
#define LOGIC_FATAL(cond) FATAL(cond, std::logic_error)

#endif
