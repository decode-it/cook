#include "cook/model/object/Path.hpp"

namespace cook { namespace model { namespace object {


Path::Path(Type type, const std::filesystem::path & path)
    : Object(type, path.string()),
      path(path)
{
}

bool Path::operator==(const Path &rhs) const
{
    return equal_(rhs) && path == rhs.path;
}

} } }

