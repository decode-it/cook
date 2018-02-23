#ifndef HEADER_cook_algo_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Recipe.hpp"
#include "cook/Result.hpp"

namespace cook { namespace algo {

Result resolve_dependency(model::Recipe *&result, const model::Uri & uri, model::Book * current_book, model::Book * root_book);


} }

#endif
