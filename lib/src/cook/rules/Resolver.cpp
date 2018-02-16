#include "cook/rules/Resolver.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

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

bool Resolver::operator()(model::Recipe & recipe, LanguageTypePair & key, property::File & file) const
{
    MSS_BEGIN(bool);

    auto accepts = [&](const Interface & interface) { return interface.accepts_file(key, file); };
    const Interface & interface = rule_set_->find(accepts);

    MSS(interface.resolve_file(key, file));
    MSS(interface.add_file(recipe, key, file));

    MSS_END();
}

bool Resolver::operator()(model::Recipe & recipe, const model::GlobInfo & globber) const
{
    MSS_BEGIN(bool);

    // get the directory
    std::filesystem::path dir = globber.dir;
    if (dir.is_relative())
        dir = recipe.working_directory() / dir;

    bool error_occured = false;

    // find all dirs matching the pattern
    auto file_callback = [&](const std::filesystem::path & fn)
    {
        MSS_BEGIN(bool);

        // extract the dir and the fn part
        std::filesystem::path f_dir, f_rel;
        extract_dir(gubg::make_range(fn), gubg::make_range(dir), f_dir, f_rel);

        // resolve the file if possible
        property::File file(f_dir, f_rel);
        LanguageTypePair key(globber.language, globber.type);

        auto accepts = [&](const Interface & interface) { return interface.accepts_file(key, file); };
        const Interface & interface = rule_set_->find(accepts);

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





} }
