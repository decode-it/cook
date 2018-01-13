#ifndef HEADER_cook_view_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_File_hpp_ALREADY_INCLUDED

#include <string>

namespace cook { namespace view {

enum FileType
{
    Unknown,
    Header,
    Source,
    Force_Include
};

inline std::string to_string(FileType ft)
{
    switch(ft)
    {
        case Header:        return "header";
        case Source:        return "source";
        case Force_Include: return "force_include";
        default:
            return "";
    }
}

struct File
{
public:
    File(const std::string & dir, const std::string & rel)
        : dir_(dir), rel_(rel), type(Unknown) {}

    const std::string & dir() const { return dir_; }
    const std::string & rel() const { return rel_; }

    FileType type;

private:
    std::string dir_;
    std::string rel_;
};



} }

#endif
