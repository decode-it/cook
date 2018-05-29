#include "cook/chai/Book.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/Context.hpp"
#include "cook/chai/mss.hpp"
#include "cook/Log.hpp"

namespace cook { namespace chai {

    Book::Book(model::Book * book, const Context *context)
    : book_(book),
    context_(context),
    data_(from_any(book->user_data()))
    {
    }

    Book Book::book(const model::Uri &uri)
    {
        CHAI_MSS_BEGIN();

        auto ss = log::scope("goc book", [&](auto & n) {n.attr("parent uri", book_->uri()).attr("uri", uri); });

        model::Book * subbook = nullptr;
        CHAI_MSS(model::Book::goc_relative(subbook, uri, book_));

        return Book(subbook, context_);
    }

    void Book::book(const model::Uri & uri, const BookFunctor &functor)
    {
        functor(book(uri));
    }

    bool Book::has_recipe(const model::Uri & uri) const
    {
        CHAI_MSS_BEGIN();

        CHAI_MSS_MSG(!uri.path().empty(), Error, "The recipe uri can not be empty");

        model::Recipe * recipe = nullptr;
        CHAI_MSS(model::Book::find_relative(recipe, uri, book_));

        return !!recipe;
    }


    Recipe Book::recipe_(const model::Uri & uri, const std::optional<TargetType> & type)
    {
        CHAI_MSS_BEGIN();

        CHAI_MSS_MSG(!uri.path().empty(), Error, "The recipe uri can not be empty");

        // check whether this book already exists
        model::Recipe * recipe = nullptr;
        bool already_existing = false;

        {
            CHAI_MSS(model::Book::find_relative(recipe, uri.as_relative(), book_));
            if (recipe)
                already_existing = true;
            else
                CHAI_MSS(model::Book::goc_relative(recipe, uri.as_relative(), book_));
        }

        Recipe r(recipe, context_);

        // set the Type
        if (type)
            r.set_type(*type);

        // set the working directory
        if (!already_existing)
            r.set_working_directory("./");

        return r;
    }


    Recipe Book::recipe(const model::Uri & uri, TargetType type)
    {
        return recipe_(uri, type);
    }

    Recipe Book::recipe(const model::Uri & uri)
    {
        return recipe_(uri);
    }

    void Book::recipe(const model::Uri & uri, TargetType type, const RecipeFunctor & functor)
    {
        functor(recipe_(uri, type));
    }

    void Book::recipe(const model::Uri & uri, const RecipeFunctor & functor)
    {
        functor(recipe_(uri));
    }

    Recipe Book::recipe(const model::Uri & uri, const std::string & type)
    {
        CHAI_MSS_BEGIN();
        TargetType tp;
        CHAI_MSS(convert_(type, tp)); 

        return recipe_(uri, tp);
    }

    void Book::recipe(const model::Uri & uri, const std::string & type, const RecipeFunctor & functor)
    {
        functor(recipe(uri, type));
    }

    const model::Uri & Book::uri() const 
    { 
        return book_->uri(); 
    }

    Result Book::convert_(const std::string & str, TargetType & type) const
    {
        MSS_BEGIN(Result);

        if (false) {}

        else if (str == "executable")                   { type = TargetType::Executable; }
        else if (str == "TargetType.Executable")        { type = TargetType::Executable; }
        else if (str == "Executable")                   { type = TargetType::Executable; }

        else if (str == "library")                      { type = TargetType::Archive; }
        else if (str == "archive")                      { type = TargetType::Archive; }
        else if (str == "TargetType.Archive")           { type = TargetType::Archive; }
        else if (str == "Archive")                      { type = TargetType::Archive; }


        else if (str == "shared_library")               { type = TargetType::SharedLibrary; }
        else if (str == "TargetType.SharedLibrary")     { type = TargetType::SharedLibrary; }
        else if (str == "SharedLibrary")                { type = TargetType::SharedLibrary; }

        else if (str == "script")                       { type = TargetType::Script; }
        else if (str == "Script")                       { type = TargetType::Script; }
        else if (str == "TargetType.Script")            { type = TargetType::Script; }

        else
            MSG_MSS(false, Error, "Unknown target type: " << str);

        MSS_END();
    }

} }
