#ifndef HEADER_cook_process_souschef_PathExtraction_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_PathExtraction_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/Result.hpp"

namespace cook { namespace process { namespace souschef {

Result add_derived_path(const LanguageTypePair & key, const ingredient::File & file, model::Recipe & recipe);
Result add_derived_paths(model::Recipe & recipe, const LanguageTypePair & src_pair, const LanguageTypePair & dst_pair);



} } }

#endif
