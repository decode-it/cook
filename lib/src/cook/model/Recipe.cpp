#include "cook/model/Recipe.hpp"
#include "cook/model/Book.hpp"

namespace cook { namespace model {

namespace  {

Uri append_part(const Uri & uri, const Part & part)
{
    Uri new_uri = uri;
    new_uri.set_name(part);
    return new_uri;
}

}

Recipe::Recipe(Book * book, const Part & part)
    : Element(append_part(book->uri(), part)),
      type_(Type::Archive),
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
    using Functor = util::ElementAt<1>;

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
    MSS(it->second == nullptr);
    it->second = recipe;

    MSS_END();

}

bool Recipe::add_dependency(const Dependency &dependency)
{
    MSS_BEGIN(bool);
    MSS(dependency.has_name());

    MSS(dependencies_.emplace(dependency, nullptr).second);
    MSS_END();
}

void Recipe::set_type(const Type &type)
{
    type_ = type;
}

Recipe::Type Recipe::type() const
{
    return type_;
}

const std::filesystem::path & Recipe::working_directory() const
{
    return wd_;
}

void Recipe::set_working_directory(const std::filesystem::path & wd)
{
    wd_ = wd;
}

void Recipe::stream(log::Importance importance) const
{
    const auto imp = log::importance(importance);

    auto ss = log::scope("recipe", imp, [&](auto &n){
            n.attr("name", name()).attr("uri", uri()).attr("type", type());
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
                            n.attr("full_name", e.key()).attr("dir", e.dir()).attr("rel", e.rel());
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

} }

