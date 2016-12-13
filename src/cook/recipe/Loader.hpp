#ifndef HEADER_cook_recipe_Loader_hpp_ALREADY_INCLUDED
#define HEADER_cook_recipe_Loader_hpp_ALREADY_INCLUDED

#include "cook/recipe/Description.hpp"
#include "cook/Codes.hpp"
#include "cook/chai/Engine.hpp"
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
            Loader();

            ReturnCode load(const gubg::file::Name &fn)
            {
                MSS_BEGIN(ReturnCode, logns);
                L("Loading recipes from " << fn);
                MSS(chai_engine_.eval_file(fn));
                L("Loaded " << descriptions_.size() << " recipes");
                MSS_END();
            }

            ReturnCode get(const Description *&description, const Alias &alias) const
            {
                MSS_BEGIN(ReturnCode);
                auto p = descriptions_.find(alias);
                MSS(p != descriptions_.end());
                description = &p->second;
                MSS_END();
            }

            //Methods exposed to chai
            bool create_new_recipe(const std::string &ns, const std::string &tag, std::function<void (Description &)> callback);

        private:
            chai::Engine chai_engine_;
            DescriptionPerAlias descriptions_;
    };

} } 

#endif
