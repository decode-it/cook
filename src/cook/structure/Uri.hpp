#ifndef HEADER_cook_structure_Uri_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_Uri_hpp_ALREADY_INCLUDED

#include "cook/structure/Tag.hpp"
#include "cook/util/Range.hpp"
#include <string>
#include <vector>


namespace cook { namespace structure {
       
    class Uri
    {
        using storage_type = std::vector<Tag>;
        
    public:
        using iterator = storage_type::const_iterator;
        using reverse_iterator = storage_type::const_reverse_iterator;
                      
        Uri() = default;
        explicit Uri(const std::string & complete_path);
        Uri(const util::Range<iterator> & lst);
        
        friend Uri operator+(const Uri & lhs, const Uri & rhs);
        friend Uri operator+(const Uri & lhs, const Tag & rhs);
        Uri & operator+=(const Uri & rhs);
        Uri & operator+=(const Tag& rhs);
        
        bool operator==(const Uri & rhs) const;
        bool operator!=(const Uri & rhs) const;
        bool operator<(const Uri & rhs) const;
        
        const Tag & back() const;
        
        util::Range<iterator> tags() const              { return util::make_range(data_); }
        util::Range<reverse_iterator> rtags() const     { return util::make_range(data_.rbegin(), data_.rend()); }
        
        std::string string() const;
        std::size_t hash() const;
        friend std::ostream & operator<<(std::ostream & str, const Uri & ns);
        
    private:
        storage_type data_;
    };
} } 

namespace std {
       
    template <> struct hash<cook::structure::Uri>
    {
        typedef cook::structure::Uri argument_type;
        typedef std::size_t result_type;
        
        result_type operator()(const argument_type & s) const { return s.hash();  }
    };
}

#endif
