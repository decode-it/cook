#ifndef HEADER_cook_model_BuildTarget_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_BuildTarget_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"
#include <string>
#include <optional>

namespace cook { namespace model {

struct BuildTarget
{
    explicit BuildTarget(const std::string & name = std::string())
        : name(name)
    {
    }

    BuildTarget(const std::string & name, const std::filesystem::path & filename)
        : name(name),
          filename(filename)
    {}

    std::string name;
    std::optional<std::filesystem::path> filename;
};


} }

#endif
