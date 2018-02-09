#ifndef HEADER_cook_model_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_File_hpp_ALREADY_INCLUDED

#include <gubg/std/filesystem.hpp>

namespace cook { namespace model {

class File
{
private:
    std::filesystem::path full_;
    std::filesystem::path dir_;
    std::filesystem::path rel_;
    
};

} }

#endif
