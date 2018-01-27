#ifndef HEADER_cook_api_GeneratorManager_h_ALREADY_INCLUDED
#define HEADER_cook_api_GeneratorManager_h_ALREADY_INCLUDED

#include "cook/api/GenerationType.h"
#include "cook/api/Element.h"
#include "cook/bool.h"

#ifdef __cplusplus
extern "C" {
#endif

cook_bool_t cook_api_GeneratorManager_generate(cook_api_Element_t manager, cook_api_GenerationType_t type, int argc, const char * argv[]);

#ifdef __cplusplus
}
#endif

#endif
