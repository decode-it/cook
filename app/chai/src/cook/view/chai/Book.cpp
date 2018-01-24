#include "cook/view/chai/Book.hpp"
#include "cook/view/Book.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace view { namespace chai {

ModulePtr book_module()
{
    ModulePtr m(new chaiscript::Module());

    //user_type and constructor are added to make the books copyable in chaiscript.
    m->add(chaiscript::user_type<Book>(), "Book");
    m->add(chaiscript::constructor<Book(const Book &)>(), "Book");
    m->add(chaiscript::fun(&Book::display_name), "display_name");
    m->add(chaiscript::fun(&Book::print), "print");
    m->add(chaiscript::fun(&Book::book), "book");
    m->add(chaiscript::fun(&Book::recipe_3), "recipe");
    m->add(chaiscript::fun(&Book::recipe_2), "recipe");

    return m;
}

} } }
