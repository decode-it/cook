#include "cook/process/souschef/Resolver.hpp"
#include "cook/log/Scope.hpp"

namespace cook { namespace process { namespace souschef { 

std::string Resolver::description() const
{
    return "Resolver";
}

Result Resolver::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & /*context*/) const
{
    MSS_BEGIN(Result);
    auto scope = log::Scope::top().scope("process");
    for (const auto &globber: recipe.globbings())
    {
        MSG_MSS(process_one(recipe, globber), Error, "Could not resolve " << globber << " for " << recipe.uri());
    }
    MSS_END();
}


bool Resolver::process_(model::Recipe & recipe, LanguageTypePair & key, ingredient::File & file) const
{
    MSS_BEGIN(bool);

    auto accepts = [&](const rules::Interface & interface) { return interface.accepts_file(key, file); };
    const rules::Interface & interface = rule_set_->find(accepts);

    MSS(interface.resolve_file(key, file));
    MSS(interface.add_file(recipe, key, file));

    MSS_END();
}

namespace  {

template <typename It>
void extract_dir(
        gubg::Range<It> fn_rng,
        gubg::Range<It> dir_rng,
        std::filesystem::path & actual_dir,
        std::filesystem::path & actual_rel)
{
    // process both filesystems until a difference is found
    while(!dir_rng.empty() && !fn_rng.empty() && dir_rng.front() == fn_rng.front())
    {
        // add to this directory
        actual_dir /= (*dir_rng.begin());

        dir_rng.pop_front();
        fn_rng.pop_front();
    }

    // match the end
    for(const auto & part : fn_rng)
        actual_rel /= (part);
}

}

bool Resolver::process_one(model::Recipe & recipe, const model::GlobInfo & globber) const
{
    MSS_BEGIN(bool);

    auto scope = log::Scope::top().scope("process_one");
    globber.stream(scope);

    // get the directory
    std::filesystem::path dir = globber.dir;
    {

        if (dir.is_relative())
            dir = recipe.working_directory() / dir;

        scope.scope("directory", [&](auto & n) {
            n.attr("dir", dir);
        });

    }

    bool error_occured = false;

    // find all dirs matching the pattern
    auto file_callback = [&](const std::filesystem::path & fn)
    {
        MSS_BEGIN(bool);
        L(C(fn));

        // extract the dir and the fn part
        std::filesystem::path f_dir, f_rel;
        extract_dir(gubg::make_range(fn), gubg::make_range(dir), f_dir, f_rel);

        // some logging
        scope.scope("file", [&](auto & n) {
            n.attr("full_name", fn).attr("dir", f_dir).attr("rel", f_rel);
        });

        // resolve the file if possible
        ingredient::File file(f_dir, f_rel);
        LanguageTypePair key(globber.language, globber.type);

        auto accepts = [&](const rules::Interface & interface) { return interface.accepts_file(key, file); };
        const rules::Interface & interface = rule_set_->find(accepts);

        MSS(interface.resolve_file(key, file), error_occured = true);

        // overwrite the specifications
        if (globber.propagation)    file.set_propagation(*globber.propagation);
        if (globber.overwrite)      file.set_overwrite(*globber.overwrite);

        if (!globber.filter_and_adaptor || globber.filter_and_adaptor(key, file))
            MSS(interface.add_file(recipe, key, file), error_occured = true);

        return true;
    };

    MSS(gubg::file::each_glob(globber.pattern, file_callback, dir));
    MSS(!error_occured);

    MSS_END();

}

} } }
