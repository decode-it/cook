#ifndef HEADER_cook_util_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_util_File_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "gubg/std/filesystem.hpp"
#include <fstream>

namespace cook { namespace util {

inline Result open_file(const std::filesystem::path & path, std::ofstream & ofs)
{
    MSS_BEGIN(Result);

    std::filesystem::path parent = path.parent_path();
    if (!parent.empty())
        MSG_MSS(std::filesystem::create_directories(parent), Error, "Unable to create directory '" << parent.string() << "'");

    ofs.open(path.string());
    MSG_MSS(ofs.good(), Error, "Unable to create file '" << path.string() << "'");

    MSS_END();
}

} }

#endif
