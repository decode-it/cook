#include "cook/model/Book.hpp"
#include "cook/model/Recipe.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace model {

Book::Book()
    : uri_(make_root_uri())
{
}

Book::Book(const Uri & uri)
    : uri_(uri)
{
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

        it = subbooks_.insert( std::make_pair(part, ptr) ).first;
    }

    return *(it->second);
}

Recipe & Book::goc_recipe(const Part & part)
{
    auto it = recipes_.find(part);
    if (it == recipes_.end())
    {
        Uri uri = uri_;
        uri.set_name(part.string());

        auto ptr = std::make_shared<Recipe>(uri);
        it = recipes_.insert( (std::make_pair(part, ptr)) ).first;
    }

    return *(it->second);
}


} }

