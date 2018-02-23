#ifndef HEADER_cook_algo_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_algo_Book_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"

namespace cook { namespace algo {

void list_all_recipes(model::Book * book, std::list<model::Recipe *> & result);

} }

#endif
