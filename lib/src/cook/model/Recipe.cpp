#include "cook/model/Recipe.hpp"
#include "cook/model/Book.hpp"

namespace cook { namespace model {

    Recipe::Recipe(Book * book, const Part & part)
    : Element(book->uri() / part),
    allows_early_globbing_(false),
    build_target_(uri().as_relative().string('.'))
    {
        set_parent(book);
    }

    gubg::Range<Recipe::DependencyPairIterator> Recipe::dependency_pairs() const
    {
        return gubg::make_range(dependencies_);
    }

    gubg::Range<Recipe::DependencyIterator> Recipe::dependencies() const
    {
        using Functor = DependencyExtractor;

        //    DependencyIterator begin()

        return gubg::make_range(
                                boost::make_transform_iterator<Functor>(dependencies_.begin()),
                                boost::make_transform_iterator<Functor>(dependencies_.end())
                               );
    }

    bool Recipe::resolve_dependency(const Uri & uri, Recipe * recipe)
    {
        MSS_BEGIN(bool);

        auto it = dependencies_.find(uri);
        MSS(it != dependencies_.end());
        MSS(it->second.recipe == nullptr);
        it->second.recipe = recipe;

        MSS_END();

    }

    Result Recipe::add_dependency(const Dependency & dependency, const DependencyFileFilter & file_filter, const DependencyKeyValueFilter & key_value_filter)
    {
        MSS_BEGIN(Result);

        {
            DependencyValue dep_value;
            dep_value.recipe = nullptr;
            dep_value.file_filter = file_filter;
            dep_value.key_value_filter = key_value_filter;

            MSG_MSS(dependencies_.emplace(dependency, std::move(dep_value)).second, Error, uri() << " is already dependent on " << dependency);
        }

        MSS_END();
    }

    const std::filesystem::path & Recipe::working_directory() const
    {
        return wd_;
    }

    void Recipe::set_working_directory(const std::filesystem::path & wd)
    {
        wd_ = wd;
    }

    const std::set<Language> & Recipe::languages() const
    {
        return languages_;
    }

    void Recipe::add_language(Language language)
    {
        languages_.insert(language);
    }

    void Recipe::set_language(Language language)
    {
        languages_.clear();
        languages_.insert(language);
    }

    void Recipe::stream(log::Importance importance) const
    {
        const auto imp = log::importance(importance);

        auto ss = log::scope("recipe", imp, [&](auto &n){
                             n.attr("name", name()).attr("uri", uri()).attr("type", build_target().type);
                             });

        if (log::do_log(imp-1))
        {
            {
                auto ss = log::scope("files", imp);
                for (const auto &p: files_)
                {
                    auto ss = log::scope("entry", imp);
                    p.first.stream(imp);
                    for (const auto &e: p.second)
                    {
                        auto ss = log::scope("file", imp, [&](auto &n){
                                             n.attr("full_name", e.key()).attr("dir", e.dir()).attr("rel", e.rel()).attr("propagation", e.propagation());
                                             });
                    }
                }
            }
            {
                auto ss = log::scope("key_values", imp);
                for (const auto &p: key_values_)
                {
                    auto ss = log::scope("entry", imp);
                    p.first.stream(imp);
                    for (const auto &e: p.second)
                    {
                        auto ss = log::scope("key_value", imp, [&](auto &n){
                                             n.attr("key", e.key());
                                             if (e.has_value())
                                             n.attr("value", e.value());
                                             });
                    }
                }
            }
        }
    }
    void Recipe::set_callback(Hook hook, const Callback & callback)
    {
        std::size_t idx = static_cast<std::size_t>(hook);
        callbacks_.at(idx) = callback;
    }
    
    const Callback & Recipe::callback(Hook hook) const
    {
        std::size_t idx = static_cast<std::size_t>(hook);
        return callbacks_.at(idx);
    }

} }

