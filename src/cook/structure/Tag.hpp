#ifndef HEADER_cook_structure_Tag_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_Tag_hpp_ALREADY_INCLUDED

#include <string>

namespace cook { namespace structure {
    
    const static char SEPARATOR = '#';
    
    struct Tag
    {
        static bool is_valid(const std::string & tag);
        static std::pair<Tag, bool> construct(const std::string & tag);
        
        bool operator==(const Tag & rhs) const;
        bool operator!=(const Tag & rhs) const;
        bool operator<(const Tag & rhs) const;
        
        bool empty() const;
        std::size_t hash() const;
        
        const std::string & string() const;
        friend std::ostream & operator<<(std::ostream & str, const Tag & tag);
        
    private:
        std::string tag_;
    };
    
} } 

namespace std {
    
    template <> struct hash<cook::structure::Tag>
    {
        typedef cook::structure::Tag argument_type;
        typedef std::size_t result_type;
        
        result_type operator()(const argument_type & s) const { return s.hash();  }
    };
}

#endif
