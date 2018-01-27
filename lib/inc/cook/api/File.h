#ifndef HEADER_cook_api_File_h_ALREADY_INCLUDED
#define HEADER_cook_api_File_h_ALREADY_INCLUDED

#include "cook/bool.h"
#include "cook/Handle.h"
#include "cook/Flags.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef enum
{
    cook_FileType_Unknown,
    cook_FileType_Header,
    cook_FileType_Source,
    cook_FileType_UserDefined = 0x100,

} cook_FileType_t;

unsigned int cook_api_Path_num_components(cook_Handle_t path);
const char * cook_api_Path_component(cook_Handle_t path, unsigned int position);
cook_bool_t cook_api_Path_has_filename(cook_Handle_t path);

cook_Handle_t cook_api_File_get_directory_path(cook_Handle_t file);
cook_Handle_t cook_api_File_get_relative_path(cook_Handle_t file);

cook_Flags_t cook_api_File_get_flags(cook_Handle_t file);
cook_FileType_t cook_api_File_get_type(cook_Handle_t file);
void cook_api_File_set_type(cook_Handle_t file, cook_FileType_t type);
void cook_api_File_set_flags(cook_Handle_t file, cook_Flags_t flags);

#ifdef __cplusplus
}
#endif


#endif
