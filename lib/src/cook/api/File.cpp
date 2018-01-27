#include "cook/api/File.h"
#include "cook/api/macro.hpp"
#include "cook/Flags.hpp"
#include "cook/File.hpp"

unsigned int cook_api_Path_num_components(cook_Handle_t path)
{
    RUNTIME_FATAL(path);
    std::filesystem::path * fn = static_cast<std::filesystem::path *>(path);

    return std::distance(fn->begin(), fn->end());
}

const char * cook_api_Path_component(cook_Handle_t path, unsigned int position)
{
    RUNTIME_FATAL(path);
    std::filesystem::path * fn = static_cast<std::filesystem::path *>(path);

    if (position >= std::distance(fn->begin(), fn->end()))
            return nullptr;

    auto it = fn->begin();
    std::advance(it, position);

    return it->c_str();
}

cook_bool_t cook_api_Path_has_filename(cook_Handle_t path)
{
    RUNTIME_FATAL(path);
    std::filesystem::path * fn = static_cast<std::filesystem::path *>(path);

    return fn->has_filename();
}

cook_Handle_t cook_api_File_get_directory_path(cook_Handle_t file)
{
    RUNTIME_FATAL(file);
    cook::File * f = static_cast<cook::File*>(file);
    return static_cast<cook_Handle_t>(&f->dir);
}

cook_Handle_t cook_api_File_get_relative_path(cook_Handle_t file)
{
    RUNTIME_FATAL(file);
    cook::File * f = static_cast<cook::File*>(file);
    return static_cast<cook_Handle_t>(&f->rel);
}

cook_Flags_t cook_api_File_get_flags(cook_Handle_t file)
{
    RUNTIME_FATAL(file);
    cook::File * f = static_cast<cook::File*>(file);
    return f->flags;
}
cook_FileType_t cook_api_File_get_type(cook_Handle_t file)
{
    RUNTIME_FATAL(file);
    cook::File * f = static_cast<cook::File*>(file);
    return f->type;
}

void cook_api_File_set_type(cook_Handle_t file, cook_FileType_t type)
{
    RUNTIME_FATAL(file);
    cook::File * f = static_cast<cook::File*>(file);
    f->type = type;
}

void cook_api_File_set_flags(cook_Handle_t file, cook_Flags_t flags)
{
    RUNTIME_FATAL(file);
    cook::File * f = static_cast<cook::File*>(file);
    f->flags = flags;
}
