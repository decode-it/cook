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

std::pair<Uri, bool> Uri::recipe_uri(const std::string & uri_str)
{
    std::pair<Uri, bool> res(Uri(), true);
    res.second = recipe_uri(uri_str, res.first);

    return res;
}

Result Uri::recipe_uri(const std::string & uri_str, Uri & uri)
{
    MSS_BEGIN(Result);

    uri.clear();

    gubg::Strange strange(uri_str);
    uri.absolute_ = strange.pop_if(separator);

    std::string part;
    while (strange.pop_until(part, separator))
        MSS(uri.add_path_part_(part));

    MSS(uri.set_name(strange.str()));

    MSS_END();
}

Result Uri::book_uri(const std::string & uri_str, Uri & uri)
{
    MSS_BEGIN(Result);

    gubg::Strange strange(uri_str);
    uri.absolute_ = strange.pop_if(separator);

    std::string part;
    while (strange.pop_until(part, separator))
        MSS(uri.add_path_part_(part));

    if (!strange.empty())
        MSS(uri.add_path_part_(strange.str()));

    MSS_END();
}

std::pair<Uri, bool> Uri::book_uri(const std::string & uri_str)
{
    std::pair<Uri, bool> res(Uri(), true);
    res.second = book_uri(uri_str, res.first);


    return res;
}


void Uri::add_path_part(const Part & part)
{
    path_.push_back(part);
}

bool Uri::add_path_part_(const std::string & part)
{
    MSS_BEGIN(bool);

    std::optional<Part> p = Part::make_part(part);
    MSS(!!p);
    path_.push_back(*p);

    MSS_END();
}

bool Uri::set_name(const std::string & name)
{
    MSS_BEGIN(bool);

    name_ = Part::make_part(name);
    MSS(!!name_);

    MSS_END();
}

void Uri::set_name(const Part & name)
{
    name_ = name;
}

void Uri::clear_name()
{
    name_.reset();
}

void Uri::clear()
{
    path_.clear();
    name_.reset();
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
    std::string result;
    if (absolute_)
        result += sep;

    for(const auto & part: path_)
    {
        result += part.string();
        result += sep;
    }

    if (name_.has_value())
        result += name_->string();

    return result;
}

bool Uri::has_name() const
{
    return name_.has_value();
}

const Part & Uri::name() const
{
    return *name_;
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
    else if (result.has_name())         { result.clear_name(); }
    else if (!result.path().empty())    { result.pop_back();  }

    return result;
}

std::ostream & operator<<(std::ostream & str, const Uri & uri)
{
    return str << uri.string();
}

std::ostream & operator<<(std::ostream & str, const Part & part)
{
    return str << part.string();
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
