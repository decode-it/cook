#include "cook/model/Recipe.hpp"

namespace cook { namespace model {

Recipe::Recipe(const Uri & uri)
    : Snapshot(uri),
      post_(uri),
      type_(Type::Undefined)
{
}

const Recipe::Dependencies & Recipe::dependencies() const
{
    return dependencies_;
}

bool Recipe::add_dependency(const std::string & dependency)
{
    MSS_BEGIN(bool);
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

