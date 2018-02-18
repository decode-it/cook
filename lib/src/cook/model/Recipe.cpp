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
    : Snapshot(append_part(book->uri(), part)),
      post_(uri()),
      type_(Type::Undefined),
      book_(book)
{
}

const Recipe::Dependencies & Recipe::dependencies() const
{
    return dependencies_;
}

bool Recipe::add_dependency(const Dependency &dependency)
{
    MSS_BEGIN(bool);
    MSS(dependency.has_name());
    MSS(dependencies_.insert(dependency).second);
    MSS_END();
}

void Recipe::set_type(const Type & type)
{
    type_ = type;
}

Type Recipe::type() const
{
    return type_;
}

} }

