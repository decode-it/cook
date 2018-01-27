#ifndef HEADER_cook_API_h_ALREADY_INCLUDED
#define HEADER_cook_API_h_ALREADY_INCLUDED

#include "cook/Handle.h"
#include "cook/Notification.h"
#include "cook/api/Element.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    const char * project_directory;
    cook_NotificationCallback_t callback;
    void * callback_context;

} cook_API_Parameters_t;

cook_Handle_t cook_API_create(const cook_API_Parameters_t * params);
void cook_API_destroy(cook_Handle_t api);

cook_api_Element_t cook_API_root_book(cook_Handle_t api);
cook_bool_t cook_API_resolve(cook_Handle_t);

cook_api_Element_t cook_API_generator_manager(cook_Handle_t api);

void cook_API_set_toolchain_name(cook_Handle_t api, const char * name);
void cook_API_set_toolchain_config(cook_Handle_t api, const char * config);
void cook_API_set_toolchain_arch(cook_Handle_t api, const char * arch);
void cook_API_set_cache_dir(cook_Handle_t api, const char * dir);
void cook_API_set_output_dir(cook_Handle_t api, const char * dir);





#ifdef __cplusplus
}
#endif

#endif

