#ifndef HEADER_cook_view_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_File_hpp_ALREADY_INCLUDED

#include <string>
#include "gubg/std/filesystem.hpp"

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
    File(const std::filesystem::path & dir, const std::filesystem::path & rel)
        : dir_(dir), rel_(rel), type(Unknown) {}

    std::string dir() const { return dir_.string(); }
    std::string relative() const { return rel_.string(); }
    std::string filename() const { return rel_.filename(); }
    std::string extension() const { return rel_.extension(); }

    FileType type;

private:
    std::filesystem::path dir_;
    std::filesystem::path rel_;
};



} }

#endif
