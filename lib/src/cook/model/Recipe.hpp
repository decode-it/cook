#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Snapshot.hpp"
#include "cook/model/Uri.hpp"
#include "cook/model/Globber.hpp"

namespace cook { namespace model {

class Book;

class Recipe : public Snapshot
{
public:
    Recipe(const Uri & uri);

    const Snapshot & pre() const    { return *this; }
    const Snapshot & post() const   { return post_; }

    void add_globber(const Globber & globbing) { globbings_.push_back(globbing); }
    void resolve_globbings();

private:
    Recipe(const Recipe &) = delete;
    Recipe(Recipe &&) = delete;
    Recipe & operator=(const Recipe &) = delete;
    Recipe & operator=(Recipe &&) = delete;

    Snapshot post_;

    std::list<Globber> globbings_;
};

} }

#endif
