#include "cook/api/Book.h"
#include "cook/api/Element.hpp"
#include "cook/model/Book.hpp"
#include "cook/API.hpp"

using namespace cook::api;
using cook::model::Uri;

void cook_api_Book_set_display_name(cook_api_Element_t book, const char * name)
{
    get_book(book).set_display_name(name);
}

const char * cook_api_Book_get_display_name(cook_api_Element_t book)
{
    return get_book(book).display_name().c_str();
}

cook_api_Element_t cook_api_Book_goc_subbook(cook_api_Element_t handle, const char * uri_str)
{
    auto & book = get_book(handle);
    auto & api = get_api(handle);

    LOG(api, Info, "getting recipe " << uri_str << " under " << book.uri() );

    cook::model::Uri uri = book.uri();
    uri.append(cook::model::Uri::book_uri(uri_str));

    auto * child_book = api.library.goc_book(&book, uri);
    if (!child_book)
    {
        LOG(api, Error, "unable to get or create book '" << uri << "'");
        return error_element();
    }

    return make_element(&api, child_book);
}

cook_api_Element_t cook_api_Book_create_recipe(cook_api_Element_t handle, const char * uri_str, const char * type, const char * working_directory)
{
    auto & book = get_book(handle);
    auto & api = get_api(handle);

    LOG(api, Info, "creating recipe " << uri_str << " under " << book.uri() );

    cook::model::Uri uri = cook::model::Uri::recipe_uri(uri_str);
    auto * child_book = api.library.goc_book(&book, uri);
    if (!child_book)
    {
        LOG(api, Error, "unable to get or create book '" << uri.parent() << "'");
        return error_element();
    }

    auto * recipe = child_book->create_recipe(uri.name());
    if (!recipe)
    {
        LOG(api, Error, "unable to create recipe '" << uri << "'");
        return error_element();
    }

    recipe->set_type(type);
    recipe->set_working_directory(api.resolve_local(working_directory));
    return make_element(&api, recipe);
}

cook_api_Element_t cook_api_Book_get_recipe(cook_api_Element_t handle, const char * uri_str)
{
    auto & book = get_book(handle);
    auto & api = get_api(handle);

    LOG(api, Info, "getting recipe " << uri_str << " under " << book.uri() );

    cook::model::Uri uri = book.uri();
    uri.append(cook::model::Uri::recipe_uri(uri_str));

    auto * child_book = api.library.goc_book(&book, uri);
    if (!child_book)
    {
        LOG(api, Error, "unable to get or create book '" << uri.parent() << "'");
        return error_element();
    }

    auto * recipe = child_book->get_recipe(uri.name());
    if (!recipe)
    {
        LOG(api, Error, "unable to get recipe '" << uri << "'");
        return error_element();
    }

    return make_element(&api, recipe);
}
