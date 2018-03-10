#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Uri.hpp"
#include "cook/model/Element.hpp"
#include "cook/model/GlobInfo.hpp"
#include "cook/ingredient/Properties.hpp"
#include "cook/ingredient/File.hpp"
#include "cook/ingredient/KeyValue.hpp"
#include "cook/util/ElementAt.hpp"
#include "cook/log/Scope.hpp"
#include "gubg/Range.hpp"
#include "boost/iterator/transform_iterator.hpp"

namespace cook { namespace model {

    namespace tag {

        struct File_t {};
        struct KeyValue_t {};

    }

class Book;

class Recipe : public Element
{
public:
    using Dependency = Uri;
    using Dependencies = std::map<Uri, Recipe *>;

    using DependencyPairIterator = Dependencies::const_iterator;
    using DependencyIterator = boost::transform_iterator<util::ElementAt<1>, DependencyPairIterator>;

    using Files = ingredient::Ingredients<ingredient::File>;
    using KeyValues = ingredient::Ingredients<ingredient::KeyValue>;
    using Globbings = std::list<GlobInfo>;

    Recipe(Book * book, const Part & part);
    //Copying is needed in MESSAGE()
    Recipe(const Recipe &) = default;

    const Files & files() const          { return files_; }
    const KeyValues & key_values() const { return key_values_; }
    Files & files()                      { return files_; }
    KeyValues & key_values()             { return key_values_; }

    const Files & ingredients(tag::File_t) const            { return files_; }
    const KeyValues & ingredients(tag::KeyValue_t) const    { return key_values_; }
    Files & ingredients(tag::File_t)                        { return files_; }
    KeyValues & ingredients(tag::KeyValue_t)                { return key_values_; }

    const std::filesystem::path & working_directory() const;
    void set_working_directory(const std::filesystem::path & wd);

    void add_globber(const GlobInfo & globbing) { globbings_.push_back(globbing); }
    bool allows_early_globbing() const { return allows_early_globbing_; }
    const Globbings &globbings() const { return globbings_; }

    gubg::Range<DependencyPairIterator> dependency_pairs() const;
    gubg::Range<DependencyIterator> dependencies() const;

    bool add_dependency(const Dependency & dependency);
    bool resolve_dependency(const Uri & uri, Recipe * recipe);

    void set_type(const Type & type);
    Type type() const;

    void stream(log::Scope &, int level = 1) const;

private:
    Recipe(Recipe &&) = delete;
    Recipe & operator=(const Recipe &) = delete;
    Recipe & operator=(Recipe &&) = delete;

    Type type_;
    Globbings globbings_;
    std::filesystem::path wd_;
    Files files_;
    KeyValues key_values_;
    Dependencies dependencies_;
    bool allows_early_globbing_;
};

} }

#endif
