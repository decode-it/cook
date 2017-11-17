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

        bool write_structure(const model::Library &lib)
        {
            MSS_BEGIN(bool);
            os_ << ">> Static structure" << std::endl;
            MSS(write_structure_(lib));
            os_ << "<< Static structure" << std::endl;
            MSS_END();
        }

    private:
        bool write_recipe_(gubg::tree::Node &node, const model::Recipe &recipe, bool details)
        {
            MSS_BEGIN(bool);

            auto recipe_n = node.node("recipe");

            //Properties
            {
                recipe_n.attr("uri", recipe.uri_hr());
                recipe_n.attr("display_name", recipe.display_name());

                if (details)
                {
                    recipe_n.attr("script", recipe.script_filename().string());
                    recipe_n.attr("type", recipe.type());
                    recipe_n.attr("build_target", recipe.output().filename.string());
                }
            }

            if (details)
            {
                auto add_file = [&](const auto &file){
                    recipe_n.node("file").attr("type", file.type).attr("path", file.path.string());
                    return true;
                };
                MSS(recipe.each_file(add_file, model::Owner::Me));

                for (const auto &ip: recipe.include_paths())
                    recipe_n.node("include_path").attr("path", ip);

                for (const auto &p: recipe.defines())
                {
                    const auto &name = p.first;
                    const auto &value = p.second;

                    auto def_n = recipe_n.node("define");
                    def_n.attr("name", name);
                    if (!value.empty())
                        def_n.attr("value", value);
                }
            }

            MSS_END();
        };

        bool write_book_recursive_(gubg::tree::Node &node, const model::Book &book)
        {
            MSS_BEGIN(bool);
            auto book_n = node.node("book");
            book_n.attr("uri", book.uri_hr());
            book_n.attr("name", book.name());
            book_n.attr("display_name", book.display_name());

            for (const auto &fn: book.script_filenames())
            {
                book_n.node("script").attr("path", fn.string());
            }

            for (const auto &p: book.recipe_per_name())
            {
                MSS(write_recipe_(book_n, p.second, true));
            }

            for (const auto &p: book.book_per_name())
            {
                MSS(write_book_recursive_(book_n, p.second));
            }

            MSS_END();
        }

        bool write_details_(const model::RecipeDAG &dag, const std::filesystem::path &build_dir)
        {
            MSS_BEGIN(bool);

            gubg::tree::Document doc(os_);

            auto details_n = doc.node("details");
            auto wrapper = [&](const std::filesystem::path & p)
            {
                if(p.is_absolute())
                    return p;
                else
                    return build_dir/p;
            };

            auto write_recipe = [&](const auto &recipe){return write_recipe_(details_n, recipe, false);};
            MSS(dag.each_vertex<gubg::network::Direction::Backward>(write_recipe));

            MSS_END();
        }
        bool write_structure_(const model::Library &lib)
        {
            MSS_BEGIN(bool);
            gubg::tree::Document doc(os_);

            auto structure_n = doc.node("structure");

            write_book_recursive_(structure_n, lib.root());

            MSS_END();
        }
        std::ostream &os_;
    };

} } 

#endif
