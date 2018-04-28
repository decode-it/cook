#include "cook/model/Element.hpp"
#include "cook/model/Book.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace model {

Element::Element(const Uri & uri)
    : uri_(uri),
      parent_(nullptr)
{
}

void Element::set_name(const std::string & name)
{
    name_ = name;
}

std::string Element::name() const
{
    if (false) {}
    else if (!name_.empty())
        return name_;
    else if (!uri().path().empty())
        return uri().path().back().string();
    else return "";
}

const Uri & Element::uri() const
{
    return uri_;
}

Book * Element::parent() const
{
    return parent_;
}

std::any & Element::user_data()
{
    return user_data_;
}
const std::any & Element::user_data() const
{
    return user_data_;
}

bool Element::set_parent(Book * parent)
{
    MSS_BEGIN(bool);

    if (parent == nullptr)
        MSS(uri() == Uri());
    else
        MSS(uri().parent() == parent->uri());

    parent_ = parent;

    MSS_END();
}


} }
