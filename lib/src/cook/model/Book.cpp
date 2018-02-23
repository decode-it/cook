#include "cook/model/Book.hpp"
#include "cook/model/Recipe.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace model {

Book::Book()
    : Element(make_root_uri())
{
}

Book::Book(const Uri & uri)
    : Element(uri)
{
}

bool Book::is_root() const
{
    return uri().path().empty();
}

gubg::Range<Book::BookIterator> Book::books() const
{
    return gubg::make_range(
                boost::make_transform_iterator<ExtractPointer>(subbooks_.begin()),
                boost::make_transform_iterator<ExtractPointer>(subbooks_.end())
                );
}
gubg::Range<Book::RecipeIterator> Book::recipes() const
{
    return gubg::make_range(
                boost::make_transform_iterator<ExtractPointer>(recipes_.begin()),
                boost::make_transform_iterator<ExtractPointer>(recipes_.end())
                );
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
        Uri child_uri = uri();
        child_uri.add_path_part(part);
        auto ptr = std::make_shared<Book>(child_uri);

        ptr->set_parent(this);

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

