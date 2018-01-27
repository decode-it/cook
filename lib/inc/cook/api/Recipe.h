#ifndef HEADER_cook_api_Recipe_h_ALREADY_INCLUDED
#define HEADER_cook_api_Recipe_h_ALREADY_INCLUDED

#include "cook/api/Element.h"
#include "cook/bool.h"
#include "cook/Flags.h"

typedef cook_bool_t (*cook_api_FileFilterPredicate_t)(void * context, cook_Handle_t file);

void cook_api_Recipe_add(cook_api_Element_t recipe, const char * dir, const char * pattern, cook_Flags_t flags, cook_api_FileFilterPredicate_t filter, void *filter_context);
void cook_api_Recipe_add_dependency(cook_api_Element_t recipe, const char * recipe_name);
void cook_api_Recipe_set_display_name(cook_api_Element_t recipe, const char * name);
const char * cook_api_Recipe_get_display_name(cook_api_Element_t recipe);
void cook_api_Recipe_add_define(cook_api_Element_t recipe, const char * name, const char * value, cook_Flags_t flags);
void cook_api_Recipe_add_library(cook_api_Element_t recipe, const char * name, cook_Flags_t flags);
void cook_api_Recipe_add_library_path(cook_api_Element_t recipe, const char * path, cook_Flags_t flags);
void cook_api_Recipe_add_include_path(cook_api_Element_t recipe, const char * path, cook_Flags_t flags);

#endif
