#include "cook/api/Element.hpp"
#include "cook/api/macro.hpp"
#include "cook/API.hpp"

namespace cook { namespace api {

template <typename T> T & get_element_(cook_api_Element_t element)
{
    RUNTIME_FATAL(!!element.api && !!element.element);
    return *static_cast<T *>(element.element);
}

template <typename T>
cook_api_Element_t make_element_(API * api, T * element)
{
    LOGIC_FATAL(!!api && !!element);
    return { static_cast<cook_Handle_t>(api), static_cast<cook_Handle_t>(element) };
}


model::Book & get_book(cook_api_Element_t element)
{
    return get_element_<model::Book>(element);
}

model::Recipe & get_recipe(cook_api_Element_t element)
{
    return get_element_<model::Recipe>(element);
}

generator::Manager & get_manager(cook_api_Element_t element)
{
    return get_element_<generator::Manager>(element);
}

API & get_api(cook_api_Element_t element)
{
    RUNTIME_FATAL(!!element.api && !!element.element);
    return *static_cast<API *>(element.api);
}

cook_api_Element_t make_element(API * api, model::Book * book)
{
    return make_element_(api, book);
}

cook_api_Element_t make_element(API * api, model::Recipe * recipe)
{
    return make_element_(api, recipe);
}

cook_api_Element_t make_element(API * api, generator::Manager * manager)
{
    return make_element_(api, manager);
}

} }

cook_bool_t cook_api_Element_valid(cook_api_Element_t element)
{
    return !!element.api && !!element.element;
}
