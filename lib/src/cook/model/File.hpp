#ifndef HEADER_cook_model_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_File_hpp_ALREADY_INCLUDED

#include "cook/model/Property.hpp"
#include "gubg/std/filesystem.hpp"

namespace cook { namespace model {

class File : public Property<std::filesystem::path>
{
public:
    File(const std::filesystem::path & dir, const std::filesystem & rel, Recipe * owner)
        : Property<std::filesystem::path>(dir/rel, owner),
          dir_(dir),
          rel_(rel)
    {
    }

    bool operator==(const File & rhs) const
    {
        return equal_(rhs)
                && dir() == rhs.dir()
                && rel() == rhs.re();
    }

    const std::filesystem & dir() const { return dir_; }
    const std::filesystem & rel() const { return rel_; }

private:
    std::filesystem::path dir_;
    std::filesystem::path rel_;
};

} }

#endif
