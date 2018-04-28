#include "cook/model/Uri.hpp"
#include "gubg/Strange.hpp"
#include "gubg/mss.hpp"
#include <cassert>

namespace cook { namespace model {

const char Uri::separator = '/';

std::optional<Part> Part::make_part(const std::string & part)
{
    if (part.find(Uri::separator) != std::string::npos || part.empty())
        return {};

    return Part(part);
}

Part::Part(const std::string & value)
    : value_(value)
{
}

Uri::Uri(const std::string & str)
{
    gubg::Strange strange(str);
    absolute_ = strange.pop_if(separator);

    std::string part;
    while(strange.pop_until(part, separator))
    {
        if (!part.empty())
            path_.push_back(Part(part));
    }

    if (!strange.empty())
        path_.push_back(Part(strange.str()));
}

Uri Uri::operator/(const Uri & rhs) const
{
    Uri result = *this;
    return result /= rhs;
}

Uri Uri::operator/(const Part & rhs) const
{
    Uri result = *this;
    return result /= rhs;
}

Uri & Uri::operator/=(const Uri & rhs)
{
    path_.insert(path_.end(), rhs.path_.begin(), rhs.path_.end());
    return *this;
}
Uri & Uri::operator/=(const Part & rhs)
{
    path_.push_back(rhs);
    return *this;
}

void Uri::clear()
{
    path_.clear();
    absolute_ = false;
}

bool Uri::pop_back(Part & part)
{
    MSS_BEGIN(bool);
    MSS(!path_.empty());

    part = path_.back();
    path_.pop_back();

    MSS_Q(path_.empty());
    MSS_END();
}

bool Uri::pop_back()
{
    MSS_BEGIN(bool);
    MSS(!path_.empty());

    path_.pop_back();

    MSS_Q(path_.empty());
    MSS_END();
}

gubg::Range<Uri::iterator> Uri::path() const
{
    return gubg::make_range(path_);
}

bool Uri::absolute() const
{
    return absolute_;
}

std::string Uri::string(const char sep) const
{
    return string(absolute(), sep);

}

std::string Uri::string(bool initial_separator, const char sep) const
{
    std::string result;
    if (initial_separator)
        result += sep;

    for(auto it = path_.begin(); it != path_.end(); ++it)
    {
        if (it != path_.begin())
            result += sep;

        result += *it;
    }

    return result;
}



void Uri::set_absolute(bool is_absolute)
{
    absolute_ = is_absolute;
}

Uri Uri::as_absolute() const
{
    Uri uri = *this;
    uri.set_absolute(true);
    return uri;
}
Uri Uri::as_relative() const
{
    Uri uri = *this;
    uri.set_absolute(false);
    return uri;
}

Uri make_root_uri()
{
    Uri uri;
    uri.set_absolute(true);

    return uri;
}

Uri Uri::parent() const
{
    Uri result = *this;

    if (false) {}
    else if (!result.path().empty())    { result.pop_back();  }

    return result;
}

std::ostream & operator<<(std::ostream & str, const Uri & uri)
{
    return str << uri.string();
}

std::ostream & operator<<(std::ostream & str, const Part & part)
{
    return str << static_cast<const std::string &>(part);
}

bool Uri::operator==(const Uri & rhs) const
{
    return string() == rhs.string();
}

bool Uri::operator<(const Uri & rhs) const
{
    return string() < rhs.string();
}



} }
