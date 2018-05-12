#ifndef HEADER_cook_chai_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Book_hpp_ALREADY_INCLUDED

#include "cook/chai/UserData.hpp"
#include "cook/model/Book.hpp"
#include "cook/Logger.hpp"
#include "cook/model/Recipe.hpp"
#include <functional>
#include <optional>

namespace cook { namespace chai {

class Context;
class Recipe;

class Book
{
public:
    using RecipeFunctor = std::function<void (Recipe)>;
    using BookFunctor = std::function<void (Book)>;

    Book(model::Book * book, const Context * context);

    Book book(const model::Uri & uri);
    void book(const model::Uri & uri, const BookFunctor &functor);
    bool has_recipe(const model::Uri & uri) const;

    Recipe recipe(const model::Uri & uri, TargetType type);
    Recipe recipe(const model::Uri & uri);
    void recipe(const model::Uri & uri, TargetType type, const RecipeFunctor & functor);
    void recipe(const model::Uri & uri, const RecipeFunctor & functor);

    // deprecated functionality
    Recipe recipe(const model::Uri & uri, const std::string & type);
    void recipe(const model::Uri & uri, const std::string & type, const RecipeFunctor & functor);
    

    const model::Uri & uri() const;

    UserData data() const { return data_; }

private:
    Recipe recipe_(const model::Uri & uri, const std::optional<TargetType> & type = std::optional<TargetType>());;
    Result convert_(const std::string & str, TargetType & type) const;
    
    model::Book * book_;
    const Context * context_;
    UserData data_;
};

} }

#endif
