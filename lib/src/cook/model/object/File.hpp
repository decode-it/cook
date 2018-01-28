#ifndef HEADER_cook_model_object_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_object_File_hpp_ALREADY_INCLUDED

#include "cook/model/object/Object.hpp"
#include "cook/api/File.h"
#include "gubg/std/filesystem.hpp"

namespace cook { namespace model { namespace object {

class File : public Object
{
public:
    File(Type type, const std::filesystem::path & dir, const std::filesystem::path & rel);

    bool operator==(const File &rhs) const;

    cook_FileType_t file_type = cook_FileType_Unknown;
    cook_Flags_t flags = {};

    std::filesystem::path dir;
    std::filesystem::path rel;
    std::filesystem::path path;
    std::string language;
};

} } }

#endif
