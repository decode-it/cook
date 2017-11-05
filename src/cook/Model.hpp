#ifndef HEADER_cook_Model_hpp_ALREADY_INCLUDED
#define HEADER_cook_Model_hpp_ALREADY_INCLUDED

#include "cook/model/Toolchain.hpp"
#include "cook/model/Book.hpp"
#include <string>

namespace cook { 

    class Model
    {
    public:
        std::string help_message;
        model::Toolchain toolchain;
        model::BookStack book_stack;

    private:
    };

} 

#endif
