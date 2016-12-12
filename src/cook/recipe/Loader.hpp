#ifndef HEADER_cook_recipe_Loader_hpp_ALREADY_INCLUDED
#define HEADER_cook_recipe_Loader_hpp_ALREADY_INCLUDED

#include "cook/recipe/Description.hpp"
#include "cook/Codes.hpp"
#include "gubg/file/Name.hpp"
#include "gubg/mss.hpp"
#include "chaiscript/chaiscript.hpp"
#include "chaiscript/chaiscript_stdlib.hpp"
#include <map>
#include <memory>

namespace cook { namespace recipe { 

    using DescriptionPerAlias = std::map<Alias, Description>;

    class Loader
    {
        private:
            static constexpr const char *logns = "Loader";
            using Parser = chaiscript::parser::ChaiScript_Parser<chaiscript::eval::Noop_Tracer, chaiscript::optimizer::Optimizer_Default>;

        public:
            Loader(): chai_engine_(chaiscript::Std_Lib::library(), std::make_unique<Parser>())
            {
            }

            ReturnCode load(const gubg::file::Name &dir)
            {
                MSS_BEGIN(ReturnCode, logns);
                L("Loading recipes from " << dir);
                MSS_END();
            }

        private:
            chaiscript::ChaiScript_Basic chai_engine_;
    };

} } 

#endif
