#include "cook/api/Recipe.h"
#include "cook/api/Element.hpp"
#include "cook/api/macro.hpp"
#include "cook/model/Recipe.hpp"

using namespace cook::api;

namespace  {

#define HAS_FLAG(value, flag) ((value & flag) == flag)

cook_Overwrite_t tr_overwrite(cook_Flags_t flags, cook_Overwrite_t default_value)
{
    if (false) {}
    else if (HAS_FLAG(flags, cook_Flags_Overwrite_Never))   return cook_Overwrite_Never;
    else if (HAS_FLAG(flags, cook_Flags_Overwrite_IfSame))  return cook_Overwrite_IfSame;
    else if (HAS_FLAG(flags, cook_Flags_Overwrite_Always))  return cook_Overwrite_Always;
    else
        return default_value;
}

cook_Propagation_t tr_propagation(cook_Flags_t flags, cook_Propagation_t default_value)
{
    if (false) {}
    else if (HAS_FLAG(flags, cook_Flags_Propagation_Public)) return cook_Propagation_Public;
    else if (HAS_FLAG(flags, cook_Flags_Propagation_Private)) return cook_Propagation_Private;
    else
        return default_value;
}

}

void cook_api_Recipe_add(cook_api_Element_t element, const char * dir, const char * pattern, cook_Flags_t flags, cook_api_FileFilterPredicate_t filter, void * filter_context)
{
    get_recipe(element).add(dir, pattern, flags, [&filter, filter_context](cook::File & file)
    {
        return !filter || filter(filter_context, static_cast<cook_Handle_t>(&file));
    });
}

void cook_api_Recipe_add_dependency(cook_api_Element_t element, const char * recipe_name)
{
    get_recipe(element).set_dependency(recipe_name);
}

void cook_api_Recipe_set_display_name(cook_api_Element_t element, const char * name)
{
    get_recipe(element).set_display_name(name);
}

const char * cook_api_Recipe_get_display_name(cook_api_Element_t element)
{
    return get_recipe(element).display_name().c_str();
}

void cook_api_Recipe_add_define(cook_api_Element_t element, const char * name, const char * value, cook_Flags_t flags)
{
    auto & recipe = get_recipe(element);
    auto overwrite = tr_overwrite(flags, cook_Overwrite_IfSame);

    if (value)
        recipe.add_define(name, value, overwrite);
    else
        recipe.add_define(name, value, overwrite);
}

void cook_api_Recipe_add_library(cook_api_Element_t element, const char * name, cook_Flags_t flags)
{
    auto propagation = tr_propagation(flags, cook_Propagation_Private);
    get_recipe(element).add_library(name, propagation);
}

void cook_api_Recipe_add_library_path(cook_api_Element_t element, const char * path, cook_Flags_t flags)
{
    auto propagation = tr_propagation(flags, cook_Propagation_Private);
    get_recipe(element).add_library_path(path, propagation);
}

void cook_api_Recipe_add_include_path(cook_api_Element_t element, const char * path, cook_Flags_t flags)
{
    auto propagation = tr_propagation(flags, cook_Propagation_Private);
    get_recipe(element).add_include_path(path, propagation);
}
