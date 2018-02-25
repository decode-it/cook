#ifndef HEADER_cook_ingredient_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_ingredient_File_hpp_ALREADY_INCLUDED

#include "cook/ingredient/Base.hpp"
#include "gubg/std/filesystem.hpp"
#include <ostream>

namespace cook { namespace ingredient {

class File : public Base<std::filesystem::path>
{
public:
    File(const std::filesystem::path & dir, const std::filesystem::path & rel)
        : Base<std::filesystem::path>(dir/rel),
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

    Result merge(const File & rhs)
    {
        MSS_BEGIN(Result);

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

inline std::ostream &operator<<(std::ostream &os, const File &file)
{
    os << "File" << file.dir() << " | " << file.rel();
    return os;
}

} }

#endif
