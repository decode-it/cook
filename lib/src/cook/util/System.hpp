#ifndef HEADER_cook_util_System_hpp_ALREADY_INCLUDED
#define HEADER_cook_util_System_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"
#include "cook/Result.hpp"
#include "cook/Log.hpp"
#include <regex>
#include <cassert>

namespace cook { namespace util {

template <typename Functor>
Result recurse_all_files(const std::filesystem::path & directory, const std::string & pattern, Functor && functor)
{
    MSS_BEGIN(Result);
    auto ss = log::scope("recurse", [&](auto & n) { n.attr("dir", directory).attr("pattern", pattern); });

    if (std::filesystem::is_directory(directory))
    {
        auto chdir = gubg::filesystem::change_directory(directory);
        std::regex regex(pattern);

        for(const std::filesystem::path & p : std::filesystem::recursive_directory_iterator("."))
        {
            const auto & str = p.string();
            assert(str.size() >= 2 && str[0] == '.');

            std::string fn_str = std::string(str.begin() + 2, str.end());
            std::filesystem::path fn(fn_str);

            if (std::regex_match(fn_str, regex))
                MSS(functor(fn_str));
        }
    }

    MSS_END();
}


} }

#endif
