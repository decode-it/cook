#ifndef HEADER_cook_model_object_Path_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_object_Path_hpp_ALREADY_INCLUDED

#include "cook/model/object/Object.hpp"
#include "gubg/std/filesystem.hpp"

namespace cook { namespace model { namespace object {

class Path : public Object
{
public:
    Path(Type type, const std::filesystem::path & path);

    bool operator==(const Path &rhs) const;

    std::filesystem::path path;
};

} } }

#endif
