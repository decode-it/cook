#include "cook/util/File.hpp"

namespace cook { namespace util {

Result open_file(const std::filesystem::path & path, std::ofstream & ofs)
{
    MSS_BEGIN(Result);
    auto s = log::scope("open file", [&](auto & n) { n.attr("path", path); });

    std::filesystem::path parent = path.parent_path();
    if (!parent.empty() && !std::filesystem::is_directory(parent))
        MSG_MSS(std::filesystem::create_directories(parent), Error, "Unable to create directory '" << parent.string() << "'");

    ofs.open(path.string());
    MSG_MSS(ofs.good(), Error, "Unable to create file '" << path.string() << "'");

    MSS_END();
}

std::filesystem::path make_recipe_adj_path(const model::Recipe & old_recipe, const model::Recipe & new_recipe)
{
    return gubg::filesystem::get_relative_to(new_recipe.working_directory(), old_recipe.working_directory());
}

std::filesystem::path make_recipe_adj_path(const model::Recipe & recipe)
{
    return gubg::filesystem::get_relative_to(recipe.working_directory(), ".");
}

std::filesystem::path make_local_to_recipe(const std::filesystem::path & adj_path, const std::filesystem::path & path)
{
    return gubg::filesystem::combine(adj_path, path);
}

ingredient::File make_local_to_recipe(const std::filesystem::path & adj_path, const ingredient::File & file)
{
    if (file.key().is_absolute())
        return file;

    ingredient::File f(make_local_to_recipe(adj_path, file.dir()), file.rel());
    f.set_overwrite(file.overwrite());
    f.set_owner(file.owner());
    f.set_propagation(file.propagation());

    return f;
}


std::filesystem::path make_global_from_recipe(const model::Recipe & recipe, const std::filesystem::path & path)
{
    return gubg::filesystem::combine(recipe.working_directory(), path);
}


} }

