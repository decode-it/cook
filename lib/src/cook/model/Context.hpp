#ifndef HEADER_cook_model_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Context_hpp_ALREADY_INCLUDED

#include "cook/model/Environment.hpp"
#include "cook/model/Book.hpp"

namespace cook { namespace model {

struct Context
{
    std::shared_ptr<Environment> environment;
    Book * root = nullptr;
};

} }

#endif
