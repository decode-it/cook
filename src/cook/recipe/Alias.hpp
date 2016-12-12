#ifndef HEADER_cook_recipe_Alias_hpp_ALREADY_INCLUDED
#define HEADER_cook_recipe_Alias_hpp_ALREADY_INCLUDED

#include <string>
#include <ostream>

namespace cook { namespace recipe { 

    class Alias
    {
        public:
            Alias(const std::string &ns, const std::string &tag): ns_(ns), tag_(tag) {}

            const std::string &ns() const {return ns_;}
            const std::string &tag() const {return tag_;}

            void stream(std::ostream &os) const
            {
                os << ns_ << '#' << tag_;
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
