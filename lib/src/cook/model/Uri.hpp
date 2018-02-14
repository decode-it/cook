#ifndef HEADER_cook_model_Uri_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Uri_hpp_ALREADY_INCLUDED

#include "gubg/Range.hpp"
#include <string>
#include <list>
#include <utility>

namespace cook { namespace model {

class Uri
{
    using PathContainer = std::list<std::string>;

public:
    using iterator = PathContainer::const_iterator;

    static const char separator;

    Uri() : absolute_(false) {}

    static std::pair<Uri, bool> recipe_uri(const std::string & uri_str);
    static std::pair<Uri, bool> book_uri(const std::string & uri_str);

    void clear();

    bool add_path_part(const std::string & path);
    gubg::Range<iterator> path() const;
    bool pop_back(std::string & part);
    bool pop_back();

    bool set_name(const std::string & name);
    bool has_name() const;
    void clear_name();

    bool absolute() const;
    void set_absolute(bool is_absolute);

    std::string string(const char sep = separator) const;


private:
    bool add_path_part_(const std::string & part);

    std::list<std::string> path_;
    std::string name_;
    bool absolute_;
};


} }

#endif
