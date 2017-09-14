#ifndef HEADER_cook_structure_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_Book_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"
#include "cook/structure/Namespace.hpp"
#include <functional>

namespace cook { namespace structure {
    
    struct Recipe;
    
    struct Book
    {
        std::filesystem::path base;
        std::filesystem::path fn;
        Namespace ns;
        
        bool create_book(const std::string & ns, std::function<void (Book &)> callback);
        bool add_recipe(const std::string & tag, std::function<void (Recipe &)> callback);       
    };
} }

#endif
