#ifndef HEADER_cook_model_BuildTarget_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_BuildTarget_hpp_ALREADY_INCLUDED

#include "cook/TargetType.hpp"
#include "gubg/std/filesystem.hpp"
#include <string>
#include <optional>

namespace cook { namespace model {

struct BuildTarget
{
    explicit BuildTarget(const std::string & name = std::string(), const std::filesystem::path & filename = std::filesystem::path(), TargetType type = TargetType::Undefined)
        : name(name),
          type(TargetType::Undefined)
    {
    }

    std::string name;
    std::optional<std::filesystem::path> filename;
    TargetType type;
};


} }

#endif
