#ifndef HEADER_cook_Model_hpp_ALREADY_INCLUDED
#define HEADER_cook_Model_hpp_ALREADY_INCLUDED

#include "cook/model/Toolchain.hpp"
#include "cook/model/Library.hpp"
#include <string>

namespace cook { 

    class Model
    {
    public:
        std::string help_message;
        model::Toolchain toolchain;
        model::Library library;

    private:
    };

} 

#endif
