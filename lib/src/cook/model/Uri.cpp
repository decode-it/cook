#include "cook/model/Uri.hpp"
#include "gubg/Strange.hpp"
#include "gubg/mss.hpp"
#include <cassert>

namespace cook { namespace model {

const char Uri::separator = '/';

std::pair<Uri, bool> Uri::recipe_uri(const std::string & uri_str)
{
    std::pair<Uri, bool> res(Uri(), true);

    gubg::Strange strange(uri_str);
    res.first.absolute_ = strange.pop_if(separator);

    std::string part;
    while (strange.pop_until(part, separator))
        if (!res.first.add_path_part_(part))
            return std::make_pair(Uri(), false);

    if (!res.first.set_name(strange.str()))
        return std::make_pair(Uri(), false);

    return res;
}

std::pair<Uri, bool> Uri::book_uri(const std::string & uri_str)
{
    std::pair<Uri, bool> res(Uri(), true);

    gubg::Strange strange(uri_str);
    res.first.absolute_ = strange.pop_if(separator);

    std::string part;
    while (strange.pop_until(part, separator))
        if (!res.first.add_path_part_(part))
            return std::make_pair(Uri(), false);

    if (!strange.empty())
        if (!res.first.add_path_part_(strange.str()))
            return std::make_pair(Uri(), false);

    return res;
}

bool Uri::add_path_part(const std::string & path)
{
    MSS_BEGIN(bool);

    gubg::Strange strange(path);
    MSS(!strange.empty());

    bool is_absolute_part = strange.pop_if(separator);
    if (is_absolute_part)
    {
        MSS(path.empty());
        MSS(!strange.empty());
    }

    // pop the final separator (if necessary)
    strange.pop_back_if(separator);

    // check for a valid part
    const std::string & part = strange.str();
    MSS(part.find(separator) == std::string::npos);

    // add it
    MSS(add_path_part_(part));
    absolute_ |= is_absolute_part;

    MSS_END();
}

bool Uri::add_path_part_(const std::string & part)
{
    MSS_BEGIN(bool);
    assert(part.find(separator) == std::string::npos);
    MSS(!part.empty());

    path_.push_back(part);

    MSS_END();
}


bool Uri::set_name(const std::string & name)
{
    MSS_BEGIN(bool);
    MSS(!name.empty());

    name_ = name;

    MSS_END();
}

void Uri::clear_name()
{
    name_.clear();
}

void Uri::clear()
{
    path_.clear();
    name_.clear();
    absolute_ = false;
}

bool Uri::pop_back(std::string & part)
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
    std::string result;
    if (absolute_)
        result += sep;

    for(const auto & part: path_)
    {
        result += part;
        result += sep;
    }

    if (!name_.empty())
        result += name_;

    return result;
}

bool Uri::has_name() const
{
    return !name_.empty();
}

void Uri::set_absolute(bool is_absolute)
{
    absolute_ = is_absolute;
}

} }
