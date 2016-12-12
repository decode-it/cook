#ifndef HEADER_cook_recipe_Loader_hpp_ALREADY_INCLUDED
#define HEADER_cook_recipe_Loader_hpp_ALREADY_INCLUDED

#include "cook/recipe/Description.hpp"
#include "cook/Codes.hpp"
#include "gubg/file/Name.hpp"
#include "gubg/mss.hpp"
#include <map>

namespace cook { namespace recipe { 

    using DescriptionPerAlias = std::map<Alias, Description>;

    class Loader
    {
        private:
            static constexpr const char *logns = "Loader";

        public:
            ReturnCode load(const gubg::file::Name &dir)
            {
                MSS_BEGIN(ReturnCode, logns);
                L("Loading recipes from " << dir);
                MSS_END();
            }

        private:
    };

} } 

#endif
