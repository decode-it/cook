#ifndef HEADER_cook_model_Uri_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Uri_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "gubg/Range.hpp"
#include <string>
#include <list>
#include <utility>
#include <optional>

namespace cook { namespace model {

class Uri;

class Part
{
public:
    Part(const Part &) = default;
    Part(Part &&) = default;
    Part & operator=(const Part &) = default;
    Part & operator=(Part &&) = default;

    static std::optional<Part> make_part(const std::string & part);

    operator const std::string &() const { return value_; }

    bool operator==(const Part & rhs) const { return value_ == rhs.value_; }
    bool operator<(const Part & rhs) const { return value_ < rhs.value_; }

private:
    friend class Uri;
    Part(const std::string & value);

    std::string value_;
};

class Uri
{
    using PathContainer = std::list<Part>;

public:
    using iterator = PathContainer::const_iterator;

    static const char separator;

    Uri() : absolute_(false) {}
    Uri(const std::string & str);

    Uri operator/(const Uri & rhs) const;
    Uri operator/(const Part & rhs) const;
    Uri & operator/=(const Uri & rhs);
    Uri & operator/=(const Part & rhs);
    bool operator==(const Uri & rhs) const;
    bool operator!=(const Uri & rhs) const;
    bool operator<(const Uri & rhs) const;

    void clear();

    gubg::Range<iterator> path() const;
    bool pop_back(Part & part);
    bool pop_back();

    bool absolute() const;
    void set_absolute(bool is_absolute);

    std::string string(const char sep = separator) const;
    std::string string(bool initial_separator, const char sep = separator) const;

    Uri parent() const;
    Uri as_absolute() const;
    Uri as_relative() const;

private:
    PathContainer path_;
    bool absolute_;
};

std::ostream & operator<<(std::ostream & str, const Part & part);
std::ostream & operator<<(std::ostream & str, const Uri & uri);

Uri make_root_uri();

} }

#endif
