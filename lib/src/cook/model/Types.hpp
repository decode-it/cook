#ifndef HEADER_cook_Types_hpp_ALREADY_INCLUDED
#define HEADER_cook_Types_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>

#include "cook/File.hpp"
#include <functional>

namespace cook { 

using FileFilterPredicate = std::function<bool (File &)>;

} 

#endif
