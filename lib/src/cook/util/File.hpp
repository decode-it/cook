#ifndef HEADER_cook_util_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_util_File_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "cook/log/Scope.hpp"
#include "cook/model/Recipe.hpp"
#include "gubg/std/filesystem.hpp"
#include <fstream>

namespace cook { namespace util {

Result open_file(const std::filesystem::path & path, std::ofstream & ofs);
std::filesystem::path make_recipe_adj_path(const model::Recipe & old_recipe, const model::Recipe & new_recipe, const std::filesystem::path & project_dir);
std::filesystem::path make_recipe_adj_path(const model::Recipe & recipe, const std::filesystem::path & project_dir);
std::filesystem::path make_local_to_recipe(const std::filesystem::path & adj_path, const std::filesystem::path & project_dir, const std::filesystem::path & path);
ingredient::File make_local_to_recipe(const std::filesystem::path & adj_path, const std::filesystem::path & project_dir, const ingredient::File & file);
std::filesystem::path make_global_from_recipe(const model::Recipe & recipe, const std::filesystem::path & project_dir, const std::filesystem::path & rel_or_abs_fn);

} }

#endif
