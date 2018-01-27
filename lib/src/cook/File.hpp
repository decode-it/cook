#ifndef HEADER_cook_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_File_hpp_ALREADY_INCLUDED

#include "cook/api/File.h"
#include "cook/Flags.hpp"
#include "gubg/std/filesystem.hpp"

namespace cook {

struct File
{
    cook_FileType_t type = cook_FileType_Unknown;
    cook_Flags_t flags = {};
    std::filesystem::path dir;
    std::filesystem::path rel;
};

}

#endif
