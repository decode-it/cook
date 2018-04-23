#include "cook/process/souschef/Resolver.hpp"
#include "cook/log/Scope.hpp"
#include "cook/util/System.hpp"
#include "gubg/string_algo/substitute.hpp"

namespace cook { namespace process { namespace souschef { 

std::string Resolver::description() const
{
    return "Resolver";
}

Result Resolver::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & /*context*/) const
{
    MSS_BEGIN(Result);
    auto ss = log::scope("process");
    for (const auto &globber: recipe.globbings())
    {
        MSG_MSS(process_one(recipe, globber), Error, "Could not resolve " << globber << " for " << recipe.uri());
    }
    MSS_END();
}


Result Resolver::process_(model::Recipe & recipe, LanguageTypePair & key, ingredient::File & file) const
{
    MSS_BEGIN(Result);

    auto accepts = [&](const rules::Interface & interface) { return interface.accepts_file(key, file); };
    const rules::Interface & interface = rule_set_->find(accepts);

    MSS(interface.resolve_file(key, file));
    MSS(interface.add_file(recipe, key, file));

    MSS_END();
}

Result Resolver::process_one(model::Recipe & recipe, const model::GlobInfo & globber) const
{
    MSS_BEGIN(Result);

    auto ss = log::scope("process_one");
    //    globber.stream();

    // get the directory
    std::filesystem::path dir = globber.dir;
    {
        if (dir.is_relative())
            dir = recipe.working_directory() / dir;

        auto ss = log::scope("directory", [&](auto & n) {
            n.attr("dir", dir);
        });
    }

    const bool should_add = globber.mode == model::GlobInfo::Add;

    // find all dirs matching the pattern
    auto cb = [&](const std::filesystem::path & fn)
    {
        MSS_BEGIN(Result);

        // resolve the file if possible
        ingredient::File file(globber.dir, fn);
        LanguageTypePair key(globber.language, globber.type);

        auto accepts = [&](const rules::Interface & interface) { return interface.accepts_file(key, file); };
        const rules::Interface & interface = rule_set_->find(accepts);

        MSS(interface.resolve_file(key, file));

        // overwrite the specifications
        if (globber.propagation)    file.set_propagation(*globber.propagation);
        if (globber.overwrite)      file.set_overwrite(*globber.overwrite);

        if (!globber.filter_and_adaptor || globber.filter_and_adaptor(key, file))
        {
            if (should_add)
                MSS(interface.add_file(recipe, key, file));
            else
                interface.remove_file(recipe, key, file);
        }

        MSS_END();
    };

    std::string regex = glob_to_regex_(globber.pattern);
    MSS(util::recurse_all_files(dir, regex, cb));

    MSS_END();
}



std::string Resolver::glob_to_regex_(const std::string & pattern) const
{
    std::string pattern_re = pattern;

    if (std::filesystem::path::preferred_separator == '\\')
        gubg::string_algo::substitute(pattern_re, pattern_re, std::string("/"), std::string("\\\\"));
    gubg::string_algo::substitute(pattern_re, pattern_re, std::string("."), std::string("\\."));
    //We use \0 to represent * temporarily
    gubg::string_algo::substitute(pattern_re, pattern_re, std::string("**"), std::string(".\0", 2));
    gubg::string_algo::substitute(pattern_re, pattern_re, std::string("*"), std::string("[^/\\\\]\0", 7));

    //Replace \0 with *
    gubg::string_algo::substitute(pattern_re, pattern_re, std::string("\0", 1), std::string("*"));

    return pattern_re;
}

} } }
