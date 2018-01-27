#ifndef HEADER_cook_api_Element_h_ALREADY_INCLUDED
#define HEADER_cook_api_Element_h_ALREADY_INCLUDED

#include "cook/Handle.h"
#include "cook/bool.h"

typedef struct
{
    cook_Handle_t api;
    cook_Handle_t element;

} cook_api_Element_t;

cook_bool_t cook_api_Element_valid(cook_api_Element_t element);


#endif
