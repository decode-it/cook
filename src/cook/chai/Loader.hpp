#ifndef HEADER_cook_chai_Loader_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Loader_hpp_ALREADY_INCLUDED

#include "cook/structure/Book.hpp"
#include "cook/view/Options.hpp"

namespace cook { namespace chai {
    
    class Loader
    {
    public:
        Loader(const view::Options &options) :options_(options) {}
        
        bool load(structure::Book & root);
    
    private:
        const view::Options & options_;
    };
    
} }

#endif
