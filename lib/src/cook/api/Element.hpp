#ifndef HEADER_cook_api_Element_hpp_ALREADY_INCLUDED
#define HEADER_cook_api_Element_hpp_ALREADY_INCLUDED

#include "cook/api/Element.h"
#include <cassert>

namespace cook {

namespace model {

struct Book;
struct Recipe;

}

namespace generator { struct Manager; }

struct API;

namespace api {

model::Book & get_book(cook_api_Element_t handle);
model::Recipe & get_recipe(cook_api_Element_t handle);
generator::Manager & get_manager(cook_api_Element_t handle);
API & get_api(cook_api_Element_t element);

cook_api_Element_t make_element(API * api, model::Book * book);
cook_api_Element_t make_element(API * api, model::Recipe * recipe);
cook_api_Element_t make_element(API * api, generator::Manager * manager);
inline cook_api_Element_t make_element(API & api, model::Book & book)
{
    return make_element(&api, &book);
}

inline cook_api_Element_t make_element(API & api, model::Recipe & recipe)
{
    return make_element(&api, &recipe);
}

inline cook_api_Element_t make_element(API & api, generator::Manager & manager)
{
    return make_element(&api, &manager);
}

inline cook_api_Element_t error_element() { return {nullptr, nullptr }; }

} }



#endif
