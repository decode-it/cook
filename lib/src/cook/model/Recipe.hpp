#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Snapshot.hpp"
#include "cook/model/Uri.hpp"
#include "cook/model/Globber.hpp"

namespace cook { namespace model {

class Book;

class Recipe
{
public:
    Recipe(const Uri & uri);

    const Snapshot & pre() const    { return pre_; }
    const Snapshot & post() const   { return post_; }

    void add_globber(const Globber & globbing) { globbings_.push_back(globbing); }
    bool add_property(const property::PropertiesKey & key, const property::File & file)             { return pre_.add_property(key, file); }
    bool add_property(const property::PropertiesKey & key, const property::KeyValue & key_value)    { return pre_.add_property(key, key_value); }

private:
    Recipe(const Recipe &) = delete;
    Recipe(Recipe &&) = delete;
    Recipe & operator=(const Recipe &) = delete;
    Recipe & operator=(Recipe &&) = delete;

    Snapshot pre_;
    Snapshot post_;
    Uri uri_;
    std::list<Globber> globbings_;
};

} }

#endif
