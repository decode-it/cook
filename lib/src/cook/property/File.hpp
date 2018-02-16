#ifndef HEADER_cook_property_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_property_File_hpp_ALREADY_INCLUDED

#include "cook/property/Interface.hpp"
#include "gubg/std/filesystem.hpp"

namespace cook { namespace property {

class File : public Interface<std::filesystem::path>
{
public:
    File(const std::filesystem::path & dir, const std::filesystem::path & rel)
        : Interface<std::filesystem::path>(dir/rel),
          dir_(dir),
          rel_(rel)
    {
    }

    bool operator==(const File & rhs) const
    {
        return equal_(rhs)
                && dir() == rhs.dir()
                && rel() == rhs.rel();
    }

    bool merge(const File & rhs)
    {
        MSS_BEGIN(bool);

        MSS(merge_(*this, rhs));

        dir_ = rhs.dir();
        rel_ = rhs.rel();

        MSS_END();
    }

    const std::filesystem::path & dir() const { return dir_; }
    const std::filesystem::path & rel() const { return rel_; }

private:
    std::filesystem::path dir_;
    std::filesystem::path rel_;
};

} }

#endif
