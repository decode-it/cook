#include "cook/model/Book.hpp"
#include "cook/model/Recipe.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace model {

Book::Book()
    : uri_(make_root_uri()),
      parent_(nullptr)
{
}

bool Book::is_root() const
{
    return uri().path().empty();
}

Book * Book::parent() const
{
    return parent_;
}

Book * Book::find_book(const Part & part) const
{
    auto it = subbooks_.find(part);
    return (it == subbooks_.end() ? nullptr : it->second.get());
}

Recipe * Book::find_recipe(const Part & part) const
{
    auto it = recipes_.find(part);
    return (it == recipes_.end() ? nullptr : it->second.get());
}

Book & Book::goc_book(const Part & part)
{
    auto it = subbooks_.find(part);
    if (it == subbooks_.end())
    {
        Uri uri = uri_;
        uri.add_path_part(part);

        auto ptr = std::make_shared<Book>();
        ptr->uri_ = uri;
        ptr->parent_= this;

        it = subbooks_.insert( std::make_pair(part, ptr) ).first;
    }

    return *(it->second);
}

Recipe & Book::goc_recipe(const Part & part)
{
    auto it = recipes_.find(part);
    if (it == recipes_.end())
    {
        auto ptr = std::make_shared<Recipe>(this, part);
        it = recipes_.insert( (std::make_pair(part, ptr)) ).first;
    }

    return *(it->second);
}

const Uri & Book::uri() const
{
    return uri_;
}

bool find_book(Book *& result, Book * book, const Uri & uri)
{
    MSS_BEGIN(bool);
    result = nullptr;

    MSS(!!book);
    MSS(!uri.has_name());
    MSS(book->is_root() || !uri.absolute());

    Book * current = book;
    for(const auto & p : uri.path())
    {
        Book * child = current->find_book(p);
        if (!child)
            MSS_RETURN_OK();

        current = child;
    }
    result = current;

    MSS_END();
}

bool goc_book(Book *& result, Book * book, const Uri & uri)
{
    MSS_BEGIN(bool);
    result = nullptr;

    MSS(!!book);
    MSS(!uri.has_name());
    MSS(book->is_root() || !uri.absolute());

    Book * current = book;
    for(const auto & p : uri.path())
        current = &current->goc_book(p);

    result = current;

    MSS_END();
}


bool find_recipe(Recipe *& result, Book * book, const Uri & uri)
{
    MSS_BEGIN(bool);
    result = nullptr;

    MSS(!!book);
    MSS(uri.has_name());
    MSS(book->is_root() || !uri.absolute());

    Book * parent_book = nullptr;
    MSS(find_book(parent_book, book, uri.parent()));

    if (parent_book)
        result = parent_book->find_recipe(*uri.name());

    MSS_END();
}

bool goc_recipe(Recipe *& result, Book * book, const Uri & uri)
{
    MSS_BEGIN(bool);
    result = nullptr;

    MSS(!!book);
    MSS(uri.has_name());
    MSS(book->is_root() || !uri.absolute());

    Book * parent_book = nullptr;
    MSS(goc_book(parent_book, book, uri.parent()));
    MSS(!!parent_book);

    result = &parent_book->goc_recipe(*uri.name());

    MSS_END();
}

} }

