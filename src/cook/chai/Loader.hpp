#ifndef HEADER_cook_chai_Loader_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Loader_hpp_ALREADY_INCLUDED

#include "cook/chai/Engine.hpp"
#include "cook/structure/Book.hpp"
#include "cook/Options.hpp"


namespace cook { namespace chai {
    
    class Loader
    {
    public:
        Loader(const Options &options) :options_(options) {}
        
        bool load(structure::Book & root);

    
    private:
        const Options & options_;
    };
    
} }

#endif
