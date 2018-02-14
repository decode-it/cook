#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Snapshot.hpp"
#include "cook/model/Globber.hpp"

namespace cook { namespace model {

class Recipe
{
public:
    const Snapshot & pre() const    { return pre_; }
    const Snapshot & post() const   { return post_; }

    void add_globber(const Globber & globbing) { globbings_.push_back(globbing); }
    bool add_property(const property::PropertiesKey & key, const property::File & file)             { return pre_.add_property(key, file); }
    bool add_property(const property::PropertiesKey & key, const property::KeyValue & key_value)    { return pre_.add_property(key, key_value); }

private:
    Snapshot pre_;
    Snapshot post_;

    std::list<Globber> globbings_;
};


} }

class Recipe
{
public:
    Recipe();
};

#endif // RECIPE_HPP
