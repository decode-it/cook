#include "cook/model/object/File.hpp"

namespace cook { namespace model { namespace object {

File::File(Type type, const std::filesystem::path & dir, const std::filesystem::path & rel)
    : Object(type, (dir/rel).string()),
      dir(dir),
      rel(rel),
      path(gubg::filesystem::combine(dir, rel))
{
}

bool File::operator==(const File &rhs) const
{
    if (!equal_(rhs))
        return false;

    return file_type == rhs.file_type
            && flags == rhs.flags
            && dir == rhs.dir
            && rel == rhs.rel
            && language == rhs.language;
}


} } }


