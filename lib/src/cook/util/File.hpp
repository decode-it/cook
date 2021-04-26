#ifndef HEADER_cook_util_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_util_File_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "cook/log/Scope.hpp"
#include "cook/model/Recipe.hpp"
#include "gubg/std/filesystem.hpp"
#include <fstream>

namespace cook { namespace util {

Result open_file(const std::filesystem::path & path, std::ofstream & ofs);

std::filesystem::path get_from_to_path(const model::Recipe & from, const model::Recipe & to); 
std::filesystem::path get_from_to_path(const model::Recipe & from, const std::filesystem::path & to); 
std::filesystem::path get_from_to_path(const std::filesystem::path & from, const model::Recipe & to); 
std::filesystem::path get_from_to_path(const std::filesystem::path & from, const std::filesystem::path & to); 

} }

#endif
