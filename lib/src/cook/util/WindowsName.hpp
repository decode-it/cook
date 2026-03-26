#ifndef HEADER_cook_util_WindowsName_hpp_ALREADY_INCLUDED
#define HEADER_cook_util_WindowsName_hpp_ALREADY_INCLUDED

#include "cook/model/Uri.hpp"
#include "gubg/hash/MD5.hpp"

#include <cctype>
#include <filesystem>
#include <string>

namespace cook { namespace util {

inline std::string stable_hash_hex(const std::string & str, std::size_t width = 16)
{
    gubg::hash::md5::Stream stream;
    stream << str;

    std::string hash = stream.hash_hex();
    if (hash.size() > width)
        hash.resize(width);
    return hash;
}

inline std::string sanitize_fragment(const std::string & str, std::size_t width = 16)
{
    std::string res;
    bool last_was_sep = false;

    for (unsigned char ch: str)
    {
        if (std::isalnum(ch))
        {
            res += std::tolower(ch);
            last_was_sep = false;
        }
        else if (!res.empty() && !last_was_sep)
        {
            res += '_';
            last_was_sep = true;
        }
    }

    if (!res.empty() && res.back() == '_')
        res.pop_back();

    if (res.size() > width)
        res.resize(width);

    if (!res.empty() && res.back() == '_')
        res.pop_back();

    if (res.empty())
        res = "x";

    return res;
}

inline std::string shortened_rule_name(const std::string & uri, unsigned int count)
{
    const std::string readable = sanitize_fragment(std::filesystem::path(uri).filename().string());
    return "cmd_" + readable + "_" + stable_hash_hex(uri) + "_" + std::to_string(count);
}

inline std::filesystem::path shortened_object_relpath(const model::Uri & recipe_uri, const std::filesystem::path & source_dir, const std::filesystem::path & source_rel, const std::filesystem::path & intermediary_rel)
{
    std::string extension = intermediary_rel.extension().string();
    if (extension.empty())
        extension = ".obj";

    const std::string filename = sanitize_fragment(source_rel.stem().string()) + "_" + stable_hash_hex(intermediary_rel.string()) + extension;

    return std::filesystem::path("obj") / stable_hash_hex(recipe_uri.string(false)) / stable_hash_hex(source_dir.string()) / filename;
}

} }

#endif
