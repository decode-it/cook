#ifndef HEADER_cook_generator_NaftWriter_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_NaftWriter_hpp_ALREADY_INCLUDED

#include "cook/generator/Interface.hpp"

#include "cook/model/Env.hpp"
#include "cook/model/Library.hpp"
#include "gubg/naft/Document.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace generator {

class NaftWriter
{
public:
    NaftWriter(std::ostream &os): os_(os) { }

    bool write_details(const model::Env &env, const model::RecipeDAG &dag)
    {
        MSS_BEGIN(bool);
        os_ << ">> Details" << std::endl;
        MSS(write_details_(env, dag));
        os_ << "<< Details" << std::endl;
        MSS_END();
    }

    bool write_structure(const model::Env &env, const model::Library &lib)
    {
        MSS_BEGIN(bool);
        os_ << ">> Static structure" << std::endl;
        MSS(write_structure_(env, lib));
        os_ << "<< Static structure" << std::endl;
        MSS_END();
    }

private:
    bool write_recipe_(gubg::naft::Node &node, const model::Env &env, const model::Recipe &recipe, bool details)
    {
        MSS_BEGIN(bool);

        auto add_project = [&](const auto & p)
        {
            return gubg::filesystem::combine(env.project_dir(), p);
        };

        auto recipe_n = node.node("recipe");

        //Properties
        {
            recipe_n.attr("uri", recipe.uri_hr());
            recipe_n.attr("tag", recipe.display_name());
            recipe_n.attr("display_name", recipe.display_name());

            if (details)
            {
                recipe_n.attr("type", recipe.type());
                recipe_n.attr("build_target",  (recipe.output().filename.empty() ? "" : gubg::filesystem::combine(env.output_dir(), recipe.output().filename).string() ));
            }
        }

        if (details)
        {
            auto add_file = [&](const auto &file){
                recipe_n.node("file").attr("type", file.type).attr("path", add_project(file.path).string());
                return true;
            };
            MSS(recipe.each_file(add_file, model::Owner::Me));

            for (const auto &ip: recipe.include_paths(model::Owner::Anybody))
                recipe_n.node("include_path").attr("path", add_project(ip).string());

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

    bool write_book_recursive_(gubg::naft::Node &node, const model::Env &env, const model::Book &book)
    {
        MSS_BEGIN(bool);
        auto book_n = node.node("book");
        book_n.attr("uri", book.uri_hr());
        book_n.attr("display_name", book.display_name());

        for (const auto &fn: book.script_filenames())
        {
            book_n.node("script").attr("path", gubg::filesystem::combine(env.project_dir(), fn).string());
        }

        for (const auto &p: book.recipe_per_name())
        {
            MSS(write_recipe_(book_n, env, p.second, true));
        }

        for (const auto &p: book.book_per_name())
        {
            MSS(write_book_recursive_(book_n, env, p.second));
        }

        MSS_END();
    }

    bool write_details_(const model::Env &env, const model::RecipeDAG &dag)
    {
        MSS_BEGIN(bool);

        gubg::naft::Document doc(os_);

        auto details_n = doc.node("details");
        auto write_recipe = [&](const auto &recipe) { return write_recipe_(details_n, env, recipe, false); };
        MSS(dag.each_vertex<gubg::network::Direction::Backward>(write_recipe));

        MSS_END();
    }
    bool write_structure_(const model::Env &env, const model::Library &lib)
    {
        MSS_BEGIN(bool);
        gubg::naft::Document doc(os_);

        auto structure_n = doc.node("structure");

        write_book_recursive_(structure_n, env, lib.root());

        MSS_END();
    }
    std::ostream &os_;
};

class NaftProcessor : public Processor
{
public:
    NaftProcessor(const Logger & logger)
        : Processor(logger),
          w(std::cout)
    {
    }

    virtual bool visit(cook::Model & model, const std::vector<std::string> & recipes) override
    {
        MSS_BEGIN(bool);

        MSS(recipes.empty(), LOG(logger(), Error, "Unexpected arguments given"));

        model::RecipeDAG dag;
        // We currently do not use the dag when writing the structure, but we still need to execute this
        // to make sure all recipes are propely merged according to the dependency structure.
        MSS(model.library.get(dag, ""), LOG(logger(), Error, "Could not extract the DAG"));
        args.env = &model.env;
        args.library= &model.library;

        MSS_END();
    }
    virtual bool process() override
    {
        return w.write_structure(*args.env, *args.library);
    }

private:
    NaftWriter w;
    struct
    {
        const model::Env * env;
        const model::Library * library;
    } args;

};

class NaftFactory : public Factory
{
public:
    std::shared_ptr<Processor> construct(const Logger & logger) const override
    {
        return std::make_shared<NaftProcessor>(logger);
    }
};

} } 

#endif
