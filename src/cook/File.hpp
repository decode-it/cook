#ifndef HEADER_cook_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_File_hpp_ALREADY_INCLUDED

#include <string>
#include "gubg/std/filesystem.hpp"

namespace cook {

enum FileType
{
    Unknown,
    Header,
    Source,
    Force_Include
};

inline std::ostream & operator<<(std::ostream & str, FileType ft)
{
    switch(ft)
    {
#define L_CASE(NAME, VAL) case NAME: str << VAL; break;
        L_CASE(Header, "header");
        L_CASE(Source, "source");
        L_CASE(Force_Include, "force_include");
#undef L_CASE
        default:
            str << "unknown";
    }

    return str;
}

struct File
{
public:
    std::string dir_part() const    { return dir.string(); }
    std::string rel_part() const    { return rel.string(); }
    std::string filename() const    { return rel.filename(); }
    std::string extension() const   { return rel.extension(); }

    FileType type = Unknown;
    std::filesystem::path dir;
    std::filesystem::path rel;

private:

};

}

#endif
