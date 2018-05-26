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
    return gubg::iterator::transform<ExtractPointer>(gubg::make_range(subbooks_));
}
gubg::Range<Book::RecipeIterator> Book::recipes() const
{
    return gubg::iterator::transform<ExtractPointer>(gubg::make_range(recipes_));
}

Book * Book::find_book_(const Part & part) const
{
    auto it = subbooks_.find(part);
    return (it == subbooks_.end() ? nullptr : it->second.get());
}

Recipe * Book::find_recipe_(const Part & part) const
{
    auto it = recipes_.find(part);
    return (it == recipes_.end() ? nullptr : it->second.get());
}

Book & Book::goc_book_(const Part & part)
{
    auto it = subbooks_.find(part);
    if (it == subbooks_.end())
    {
        Uri child_uri = uri() / part;
        auto ptr = std::make_shared<Book>(child_uri);

        ptr->set_parent(this);

        it = subbooks_.insert( std::make_pair(part, ptr) ).first;
    }

    return *(it->second);
}

Recipe & Book::goc_recipe_(const Part & part)
{
    auto it = recipes_.find(part);
    if (it == recipes_.end())
    {
        auto ptr = std::make_shared<Recipe>(this, part);
        it = recipes_.insert( (std::make_pair(part, ptr)) ).first;
    }

    return *(it->second);
}

Result Book::find_relative(Recipe *& result, const Uri & uri, Book * ancestor)
{
    MSS_BEGIN(Result);

    result = nullptr;

    MSS(!!ancestor);
    MSG_MSS(!uri.path().empty(), Error, "The supplied uri is empty");
    MSG_MSS(!uri.absolute(), Error, "The uri '" << uri << " is not relative");

    Book * parent = nullptr;
    MSS(find_relative(parent, uri.parent(), ancestor));

    if (parent)
        result = parent->find_recipe_(uri.path().back());

    MSS_END();
}

Result Book::find_relative(Book *& result, const Uri & uri, Book * ancestor)
{
    MSS_BEGIN(Result);

    result = nullptr;

    MSS(!!ancestor);
    MSG_MSS(!uri.absolute(), Error, "The uri '" << uri << " is not relative");

    Book * current = ancestor;
    for(const auto & p : uri.path())
    {
        Book * child = current->find_book_(p);
        if (!child)
            MSS_RETURN_OK();

        current = child;
    }

    result = current;

    MSS_END();
}

Result Book::goc_relative(Recipe *& result, const Uri & uri, Book * ancestor)
{
    MSS_BEGIN(Result);

    result = nullptr;

    MSS(!!ancestor);
    MSG_MSS(!uri.path().empty(), Error, "The supplied uri is empty");
    MSG_MSS(!uri.absolute(), Error, "The uri '" << uri << " is not relative");

    Book * parent = nullptr;
    MSS(goc_relative(parent, uri.parent(), ancestor));
    MSS(!!parent);

    result = &parent->goc_recipe_(uri.path().back());
    MSS(!!result);

    MSS_END();
}

Result Book::goc_relative(Book *& result, const Uri & uri, Book * ancestor)
{
    MSS_BEGIN(Result);

    result = nullptr;

    MSS(!!ancestor);
    MSG_MSS(!uri.absolute(), Error, "The uri '" << uri << " is not relative");

    result = ancestor;
    for(const auto & p : uri.path())
        result = &result->goc_book_(p);

    MSS_END();
}

} }

