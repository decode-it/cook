#include "cook/process/souschef/RecipeNamer.hpp"

namespace cook { namespace process { namespace souschef {

void RecipeNamer::update_build_target_name(model::Recipe & recipe) const
{
    if (recipe.build_target().name.empty())
        recipe.build_target().name = recipe.uri().as_relative().string('.');
}

std::filesystem::path RecipeNamer::dynamic_library_filename(const std::string & build_target_name) const
{
#if BOOST_OS_WINDOWS
    return gubg::stream([&](auto & os) { os << build_target_name << ".dll"; });
#else
    return gubg::stream([&](auto & os) { os << "lib" << build_target_name << ".so"; });
#endif
}

std::filesystem::path RecipeNamer::executable_filename(const std::string &build_target_name) const
{
#if BOOST_OS_WINDOWS
    return gubg::stream([&](auto & os) { os << build_target_name << ".exe"; });
#else
    return gubg::stream([&](auto & os) { os << build_target_name; });
#endif
}

std::filesystem::path RecipeNamer::static_library_filename(const std::string &build_target_name) const
{
#if BOOST_OS_WINDOWS
    return gubg::stream([&](auto & os) { os << build_target_name << ".lib"; });
#else
    return gubg::stream([&](auto & os) { os << "lib" << build_target_name << ".a"; });
#endif
}


Result RecipeNamer::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & /*context*/) const
{
    MSS_BEGIN(Result);

    update_build_target_name(recipe);

    switch(recipe.type())
    {
        case model::Recipe::Type::Archive:
            recipe.build_target().filename = static_library_filename(recipe.build_target().name);
            break;

        case model::Recipe::Type::Executable:
            recipe.build_target().filename = executable_filename(recipe.build_target().name);
            break;

        case model::Recipe::Type::SharedLibrary:
            recipe.build_target().filename = dynamic_library_filename(recipe.build_target().name);
            break;

        default:
            MSS(false);
    }

    MSS_END();
}

} } }

