#include "cook/structure/Tag.hpp"
#include "gubg/mss.hpp"
#include <sstream>
#include <cassert>

namespace cook { namespace structure {
    
    bool Tag::is_valid(const std::string & tag)
    {
        return tag.find(SEPARATOR) == std::string::npos;
    }
    
    std::pair<Tag, bool> Tag::construct(const std::string & tag)
    {
        std::pair<Tag, bool> res(Tag(), false);
        if (tag.find(SEPARATOR) != std::string::npos)
            return res;
        
        res.first.tag_ = tag;
        res.second = true;
        return res;
    }
    
    bool Tag::operator==(const Tag & rhs) const
    {
        return tag_ == rhs.tag_;
    }
    
    bool Tag::operator!=(const Tag & rhs) const
    {
        return !operator==(rhs);
    }
    
    bool Tag::operator<(const Tag & rhs) const
    {
        return tag_ < rhs.tag_; 
    }
    
    const std::string & Tag::string() const 
    { 
        return tag_; 
        
    }
    
    bool Tag::empty() const 
    { 
        return tag_.empty(); 
        
    }
    
    std::ostream & operator<<(std::ostream & str, const Tag & tag)
    {
        str << tag.tag_;
        return str;
    }
    
    std::size_t Tag::hash() const 
    { 
        return std::hash<std::string>()(tag_); 
        
    } 
} }
