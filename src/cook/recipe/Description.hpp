#ifndef HEADER_cook_recipe_Description_hpp_ALREADY_INCLUDED
#define HEADER_cook_recipe_Description_hpp_ALREADY_INCLUDED

#include "cook/recipe/Alias.hpp"

namespace cook { namespace recipe { 

    class Description
    {
        public:
            const Alias &alias() const {return alias_;}

        private:
            Alias alias_;
    };

} } 

#endif
