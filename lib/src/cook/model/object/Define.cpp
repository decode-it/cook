#include "cook/model/object/Define.hpp"

namespace cook { namespace model { namespace object {

Define::Define(const std::string & name)
    : Object(Type::Define, name),
      name(name)
{

}
Define::Define(const std::string & name, const std::string & value)
    : Object(Type::Define, name),
      name(name),
      value(value)
{
}


bool Define::operator==(const Define &rhs) const
{
    return equal_(rhs) && value == rhs.value;
}

bool Define::has_value() const
{
    return value.empty();
}

} } }

