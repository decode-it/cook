#include "cook/chai/module/Book.hpp"
#include "cook/chai/Book.hpp"
#include "cook/chai/Recipe.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module {

    gubg::chai::ModulePtr book()
    {
        auto ptr = gubg::chai::make_module();

        
        ptr->add(chaiscript::user_type<Book>(), "Book");
        ptr->add(chaiscript::constructor<Book(const Book &)>(), "Book");

        ptr->add(chaiscript::fun(static_cast<Book (Book::*)(const model::Uri &)>(&Book::book)), "book");
        ptr->add(chaiscript::fun(static_cast<void (Book::*)(const model::Uri &, const Book::BookFunctor & fctr)>(&Book::book)), "book");
        ptr->add(chaiscript::fun(&Book::has_recipe), "has_recipe");


        ptr->add(chaiscript::fun(static_cast<Recipe (Book::*)(const model::Uri &)>(&Book::recipe)), "recipe");
        ptr->add(chaiscript::fun(static_cast<Recipe (Book::*)(const model::Uri &, TargetType)>(&Book::recipe)), "recipe");
        ptr->add(chaiscript::fun(static_cast<void (Book::*)(const model::Uri &, const Book::RecipeFunctor &)>(&Book::recipe)), "recipe");
        ptr->add(chaiscript::fun(static_cast<void (Book::*)(const model::Uri &, TargetType, const Book::RecipeFunctor &)>(&Book::recipe)), "recipe");

        // deprecated functions 
        ptr->add(chaiscript::fun(static_cast<Recipe (Book::*)(const model::Uri &, const std::string &)>(&Book::recipe)), "recipe");
        ptr->add(chaiscript::fun(static_cast<void (Book::*)(const model::Uri &, const std::string & , const Book::RecipeFunctor &)>(&Book::recipe)), "recipe");
       
        ptr->add(chaiscript::fun(&Book::data), "data");
        ptr->add(chaiscript::fun(&Book::uri), "uri");

        return ptr;
    }

} } }
