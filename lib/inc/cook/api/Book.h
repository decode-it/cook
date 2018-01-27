#ifndef HEADER_cook_api_Book_h_ALREADY_INCLUDED
#define HEADER_cook_api_Book_h_ALREADY_INCLUDED

#include "cook/api/Element.h"

#ifdef __cplusplus
extern "C" {
#endif

void cook_api_Book_set_display_name(cook_api_Element_t book, const char * name);
const char * cook_api_Book_get_display_name(cook_api_Element_t book);
cook_api_Element_t cook_api_Book_goc_subbook(cook_api_Element_t handle, const char * uri_str);
cook_api_Element_t cook_api_Book_create_recipe(cook_api_Element_t book, const char * name, const char * type, const char * working_directory);
cook_api_Element_t cook_api_Book_get_recipe(cook_api_Element_t book, const char * name);

#ifdef __cplusplus
}
#endif

#endif
