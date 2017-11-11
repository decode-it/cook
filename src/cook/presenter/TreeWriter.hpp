#ifndef HEADER_cook_presenter_TreeWriter_hpp_ALREADY_INCLUDED
#define HEADER_cook_presenter_TreeWriter_hpp_ALREADY_INCLUDED

#include "gubg/tree/Document.hpp"

namespace cook { namespace presenter { 

    class TreeWriter
    {
    public:
        TreeWriter(std::ostream &os): os_(os) { }

        bool write_details(const model::RecipeDAG &dag, const std::filesystem::path &build_dir)
        {
            MSS_BEGIN(bool);
            os_ << ">> Details" << std::endl;
            MSS(write_details_(dag, build_dir));
            os_ << "<< Details" << std::endl;
            MSS_END();
        }

        bool write_static_structure()
        {
            MSS_BEGIN(bool);
            os_ << ">> Static structure" << std::endl;
            os_ << "<< Static structure" << std::endl;
            MSS_END();
        }

    private:
        bool write_details_(const model::RecipeDAG &dag, const std::filesystem::path &build_dir)
        {
            MSS_BEGIN(bool);

            gubg::tree::Document doc(os_);

            auto n = doc.node("details");
            auto wrapper = [&](const std::filesystem::path & p)
            {
                if(p.is_absolute())
                    return p;
                else
                    return build_dir/p;
            };

            auto write_recipe = [&](){
            };
#if 0

            const structure::Recipe *active_recipe = nullptr;
            for (auto * recipe: recipes)
            {
                MSS(write_recipe_(n, *recipe, recipes, true));
                if (recipe->uri() == uri)
                    active_recipe = recipe;
            }

            {
                auto nn = n.open("active");
                MSS(!!active_recipe);
                nn.attr("uri", active_recipe->uri());
            }
#endif

            MSS_END();
        }
        std::ostream &os_;
    };

} } 

#endif
