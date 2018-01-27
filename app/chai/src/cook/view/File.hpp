#ifndef HEADER_cook_view_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_File_hpp_ALREADY_INCLUDED

#include "cook/api/File.h"
#include <string>

namespace cook { namespace view {

struct Path
{
    Path(cook_Handle_t handle = nullptr)
        : handle(handle)
    {
    }

    unsigned int num_components() const
    {
        return cook_api_Path_num_components(handle);
    }
    std::string component(unsigned int position) const
    {
        const char * ptr = cook_api_Path_component(handle, position);
        return ptr == nullptr ? std::string() : std::string(ptr);
    }
    bool has_filename() const
    {
        return cook_api_Path_has_filename(handle);
    }

    operator cook_Handle_t() const { return handle; }

    cook_Handle_t handle;
};

struct File
{
    File(cook_Handle_t handle = nullptr)
        : handle(handle)
    {
    }

    Path directory_path() const { return Path(cook_api_File_get_directory_path(handle)); }
    Path relative_path() const { return Path(cook_api_File_get_relative_path(handle)); }

    cook_Flags_t flags() { return cook_api_File_get_flags(handle); }
    cook_FileType_t type() { return cook_api_File_get_type(handle); }

    void set_flags(cook_Flags_t flags)  { return cook_api_File_set_flags(handle, flags); }
    void set_type(cook_FileType_t type)     { return cook_api_File_set_type(handle, type); }

    cook_Handle_t handle;
};


} }

#endif
