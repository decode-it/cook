#include "cook/Presenter.hpp"
#include "cook/presenter/NinjaWriter.hpp"
#include "cook/presenter/NaftWriter.hpp"
#include "cook/File.hpp"
#include "gubg/Strange.hpp"
#include "gubg/stream.hpp"

namespace cook {

using C = presenter::Command;

namespace {

template <typename It>
std::string hr(const gubg::Range<It> & keys)
{
    return gubg::stream([&](auto & os) {
        for(auto it = keys.begin(); it != keys.end(); ++it)
            os << (it != keys.begin() ? "/" : "") << hr(*it);
    });
}

template <typename It> bool pop_if(gubg::Range<It> &) { return true; }
template <typename It, typename ... cmds> bool pop_if(gubg::Range<It> & key, C c1, cmds ... commands)
{
    if (key.empty() || key.front() != c1)
        return false;

    auto rng = key;
    rng.pop_front();

    if (!pop_if(rng, commands...))
        return false;

    key = rng;
    return true;
}

}

template<typename T> struct argument_type;
template<typename T, typename U> struct argument_type<T(U)> { typedef U type; };
#define FOO(t,name) argument_type<void(t)>::type name

bool Presenter::set(const Keys & c_key, const Values &values)
{
    MSS_BEGIN(bool);

    view_.log(Info) << "Received key: " << hr(gubg::make_range(c_key)) << std::endl;

    auto key = gubg::make_range(c_key);

#define UNKNOWN_KEY \
    { \
    if (key.begin() == c_key.begin()) \
    MSS(false, view_.log(Error) << "Unknown key '" << hr(key) << "'"  << std::endl); \
    else \
    MSS(false, view_.log(Error) << "Unknown key '" << hr(key) << "' in '" << hr(gubg::make_range(c_key.begin(), key.begin())) << "'" << std::endl); }

#define SAFE_CAST_VALUE(IDX, TYPE, NAME) \
    MSS(IDX < values.size(), view_.log(Error) << "key '" << hr(key) << "' needs a value for " << #NAME << " at position " << IDX << " with type " << #TYPE << std::endl); \
    const auto * NAME ## _ptr = std::any_cast<argument_type<void(TYPE)>::type>(&values[IDX]); \
    MSS(!!NAME ## _ptr, view_.log(Error) << "key '" << hr(key) << "' needs a value for " << #NAME << " at position " << IDX << " with type " << #TYPE << std::endl); \
    const auto & NAME = *NAME ## _ptr

    if (false) {}
    else if (pop_if(key, C::script, C::filename ))      { SAFE_CAST_VALUE(0, std::string, fn); script_fn_ = fn; }
    else if (pop_if(key, C::help, C::message ))         { SAFE_CAST_VALUE(0, std::string, msg); model_.help_message = msg; }
    else if (pop_if(key, C::env, C::dir, C::project ))    { SAFE_CAST_VALUE(0, std::string, path); model_.env.set_project_dir(path); }
    else if (pop_if(key, C::env, C::dir, C::temp ))    { SAFE_CAST_VALUE(0, std::string, path); model_.env.set_temp_dir(path); }
    else if (pop_if(key, C::env, C::dir, C::output ))   { SAFE_CAST_VALUE(0, std::string, path); model_.env.set_output_dir(path); }
    else if (pop_if(key, C::toolchain ))
    {
        SAFE_CAST_VALUE(0, std::string, value);
        if (false) {}
        else if (pop_if(key, C::name))     { model_.toolchain.set_name(value); }
        else if (pop_if(key, C::config))   { model_.toolchain.set_config(value); }
        else if (pop_if(key, C::arch))     { model_.toolchain.set_arch(value); }
        else UNKNOWN_KEY
    }
    else if (pop_if(key, C::model))
    {
        if (false) {}
        else if (pop_if(key, C::book))
        {
            SAFE_CAST_VALUE(0, std::string, uri);
            auto book = model_.library.goc_book(uri);
            MSS(!!book, view_.log(Error) << "Book " << uri << " does not exists or could not be created" << std::endl);

            if (false) {}
            else if (pop_if(key, C::create))
            {
                book->set_script_filename(script_fn_);
            }
            else if (pop_if(key, C::display_name))
            {
                SAFE_CAST_VALUE(1, std::string, dn);
                book->set_display_name(dn);
            }
            else UNKNOWN_KEY
        }
        else if (pop_if(key, C::recipe))
        {
            if (false) {}
            else if (pop_if(key, C::create))
            {
                SAFE_CAST_VALUE(0, std::string, uri);
                SAFE_CAST_VALUE(1, std::string, type);
                SAFE_CAST_VALUE(2, std::string, wd);

                auto recipe = model_.library.create_recipe(uri);
                MSS(!!recipe, view_.log(Error) << "Recipe " << uri << " already exists" << std::endl);
                recipe->set_script_filename(script_fn_);
                recipe->set_type(type);
                recipe->set_working_directory(wd);
            }
            else
            {
                SAFE_CAST_VALUE(0, std::string, uri);
                auto recipe = model_.library.get_recipe(uri);
                MSS(!!recipe, view_.log(Error) << "Recipe " << uri << " does not exists" << std::endl);

                if (false) {}
                else if (pop_if(key, C::add))
                {
                    SAFE_CAST_VALUE(1, std::string, dir);
                    SAFE_CAST_VALUE(2, std::string, rel);
                    SAFE_CAST_VALUE(3, FileType, ftype);
                    SAFE_CAST_VALUE(4, FileFilterPredicate, filter);
                    recipe->add(dir, rel, ftype, filter);
                }
                else if (pop_if(key, C::define))
                {
                    SAFE_CAST_VALUE(1, Overwrite, overwrite);
                    SAFE_CAST_VALUE(2, std::string, name);

                    if (values.size() > 3)
                    {
                        SAFE_CAST_VALUE(3, std::string, value);
                        MSS(recipe->add_define(name, value, overwrite), view_.log(Error) << "Unable to add the define" << name << " to recipe " << uri << std::endl);
                    }
                    else
                    {
                        MSS(recipe->add_define(name,  overwrite), view_.log(Error) << "Unable to add the define" << name << " to recipe " << uri << std::endl);
                    }
                }
                else if (pop_if(key, C::depends_on))
                {
                    SAFE_CAST_VALUE(1, std::string, rn);
                    recipe->set_dependency(rn);
                }
                else if (pop_if(key, C::display_name))
                {
                    SAFE_CAST_VALUE(1, std::string, name);
                    recipe->set_display_name(name);
                }
                else if (pop_if(key, C::library_path))
                {
                    SAFE_CAST_VALUE(1, std::string, dir);
                    recipe->add_library_path(dir);
                }
                else if (pop_if(key, C::library))
                {
                    SAFE_CAST_VALUE(1, std::string, name);
                    recipe->add_library(name);
                }
                else if (pop_if(key, C::include_path))
                {
                    SAFE_CAST_VALUE(1, std::string, name);
                    SAFE_CAST_VALUE(2, Propagation, propagation);
                    recipe->add_include_path(name, propagation);
                }
                else UNKNOWN_KEY
            }

        }
        else UNKNOWN_KEY
    }
    else if(pop_if(key, C::action))
    {
        if (false) {}
        else if (pop_if(key, C::generate))
        {
            if(false) {}
            else if (pop_if(key, C::ninja))
            {
                SAFE_CAST_VALUE(0, std::string, rn);
                model::RecipeDAG dag;
                MSS(model_.library.get(dag, rn), view_.log(Error) << "Could not extract the DAG for " << rn << std::endl);
                {
                    // create the output path
                    std::filesystem::path output = model_.env.output_dir();
                    if (!std::filesystem::exists(output))
                        MSS(std::filesystem::create_directories(output), view_.log(Error) << "Unable to create directory'" << output.string() << "'" << std::endl);

                    output /= "build.ninja";
                    std::ofstream fo(output.string());
                    MSS(fo.good(), view_.log(Error) << "Unable to open file '" << output.string() << "'" << std::endl);

                    presenter::NinjaWriter nw(fo);
                    MSS(nw.write(model_.env, model_.toolchain, dag));
                }
            }
            else if (pop_if(key, C::details))
            {
                SAFE_CAST_VALUE(0, std::string, rn);
                model::RecipeDAG dag;
                MSS(model_.library.get(dag, rn), view_.log(Error) << "Could not extract the DAG for " << rn << std::endl);
                {
                    presenter::NaftWriter nw(std::cout);
                    MSS(nw.write_details(model_.env, dag));
                }
            }
            else if (pop_if(key, C::structure))
            {
                model::RecipeDAG dag;
                //We currently do not use the dag when writing the structure, but we still need to execute this
                //to make sure all recipes are propely merged according to the dependency structure.
                MSS(model_.library.get(dag, ""), view_.log(Error) << "Could not extract the DAG" << std::endl);
                presenter::NaftWriter nw(std::cout);
                MSS(nw.write_structure(model_.env, model_.library));
            }
            else UNKNOWN_KEY
        }
        else if (pop_if(key, C::print))
        {
            if (false) {}
            else if (pop_if(key, C::help))  view_.log(Always) << model_.help_message << std::endl;
            else if (pop_if(key, C::books)) model_.library.stream(view_.log(Message));
            else UNKNOWN_KEY
        }
        else UNKNOWN_KEY
    }
    else if (pop_if(key, C::runner, C::done))
    {
        MSS(model_.library.resolve(), view_.log(Error) << "Failed to resolve the dependencies in the library" << std::endl);
    }
    else UNKNOWN_KEY

    MSS_END();
}

}
