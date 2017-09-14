#ifndef HEADER_cook_structure_Alias_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_Alias_hpp_ALREADY_INCLUDED

#include <string>
#include <ostream>
#include <sstream>
#include <utility>

namespace cook { namespace structure { 

    class Alias
    {
        public:
            Alias(const std::string &str)
            {
                const auto ix = str.find('#');
                if (ix == std::string::npos)
                    ns_ = str;
                else
                {
                    ns_ = str.substr(0, ix);
                    tag_ = str.substr(ix+1);
                }
            }
            Alias(const std::string &ns, const std::string &tag): ns_(ns), tag_(tag) {}

            bool operator<(const Alias &rhs) const {return std::make_pair(ns_, tag_) < std::make_pair(rhs.ns_, rhs.tag_);}
            bool operator==(const Alias &rhs) const {return std::make_pair(ns_, tag_) == std::make_pair(rhs.ns_, rhs.tag_);}
            bool operator!=(const Alias &rhs) const {return !operator==(rhs);}

            const std::string &ns() const {return ns_;}
            const std::string &tag() const {return tag_;}

            void stream(std::ostream &os) const
            {
                os << ns_ << '#' << tag_;
            }

            std::string str() const
            {
                std::ostringstream oss;
                stream(oss);
                return oss.str();
            }

        private:
            std::string ns_;
            std::string tag_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Alias &alias)
    {
        alias.stream(os);
        return os;
    }

} } 

#endif
