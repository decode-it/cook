#ifndef HEADER_cook_presenter_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_presenter_Interface_hpp_ALREADY_INCLUDED

#include "cook/Types.hpp"
#include <string>

namespace cook { namespace presenter { 

    class Interface
    {
    public:
        virtual ~Interface() {}

        virtual bool set(const std::string &key, const std::string &value) = 0;
        virtual bool set(const std::string &key, const Strings &value) = 0;
        virtual bool set(const std::string &key) = 0;

        virtual bool get(const std::string &key, std::string & result) const = 0;
        virtual bool get(const std::string &key, const Strings & args, std::string & result) const = 0;
    };

} } 

#endif
