#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Snapshot.hpp"
#include "cook/model/Uri.hpp"
#include "cook/model/Element.hpp"
#include "cook/model/GlobInfo.hpp"
#include "cook/util/ElementAt.hpp"
#include "gubg/Range.hpp"
#include "boost/iterator/transform_iterator.hpp"

namespace cook { namespace model {

class Book;

class Recipe : public Element
{
public:
    using Dependency = Uri;
    using Dependencies = std::map<Uri, Recipe *>;

    using DependencyPairIterator = Dependencies::const_iterator;
    using DependencyIterator = boost::transform_iterator<util::ElementAt<1>, DependencyPairIterator>;

    Recipe(Book * book, const Part & part);

    const Snapshot & pre() const    { return pre_; }
    const Snapshot & post() const   { return post_; }
    Snapshot & pre()                { return pre_; }
    Snapshot & post()               { return post_; }

    void add_globber(const GlobInfo & globbing) { globbings_.push_back(globbing); }

    gubg::Range<DependencyPairIterator> dependency_pairs() const;
    gubg::Range<DependencyIterator> dependencies() const;

    bool add_dependency(const Dependency & dependency);
    bool resolve_dependency(const Uri & uri, Recipe * recipe);

    void set_type(const Type & type);
    Type type() const;

private:
    Recipe(const Recipe &) = delete;
    Recipe(Recipe &&) = delete;
    Recipe & operator=(const Recipe &) = delete;
    Recipe & operator=(Recipe &&) = delete;

    Snapshot pre_;
    Snapshot post_;
    std::list<GlobInfo> globbings_;
    Dependencies dependencies_;
    Type type_;
};

} }

#endif
