#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Snapshot.hpp"
#include "cook/model/Uri.hpp"
#include "cook/model/GlobInfo.hpp"
#include <set>

namespace cook { namespace model {

class Book;

class Recipe : public Snapshot
{
public:
    using Dependency = Uri;
    using Dependencies = std::map<Uri, Recipe *>;

    Recipe(Book * book, const Part & part);

    const Snapshot & pre() const    { return *this; }
    const Snapshot & post() const   { return post_; }
    Book * book() const             { return book_; }

    void add_globber(const GlobInfo & globbing) { globbings_.push_back(globbing); }
    void resolve_globbings();

    const Dependencies & dependencies() const;
    bool add_dependency(const Dependency & dependency);
    bool resolve_dependency(const Uri & uri, Recipe * recipe);

    void set_type(const Type & type);
    Type type() const;

private:
    Recipe(const Recipe &) = delete;
    Recipe(Recipe &&) = delete;
    Recipe & operator=(const Recipe &) = delete;
    Recipe & operator=(Recipe &&) = delete;

    Snapshot post_;
    std::list<GlobInfo> globbings_;
    Dependencies dependencies_;
    Type type_;
    Book * book_;
};

} }

#endif
