#include "cook/structure/Uri.hpp"
#include "gubg/mss.hpp"
#include <sstream>
#include <cassert>

namespace cook { namespace structure {

    Uri::Uri(const std::string & complete_path)
    {
        std::size_t pos = 0;
        
        while( pos < complete_path.size())
        {
            auto v = complete_path.find(SEPARATOR, pos);
            
            if(v != pos)
            {
                const std::string & cur = complete_path.substr(pos, v-pos);
                
                std::pair<Tag, bool> p = Tag::construct(cur);
                assert(p.second);
                data_.push_back(p.first);
            }
            
            pos = v == std::string::npos ? v : v+1;
        }

    }
    
    Uri::Uri(const util::Range<iterator> & lst)
    {
        data_.assign(RANGE(lst));
    }
    
    Uri operator+(const Uri & lhs, const Uri & rhs)
    {
        Uri res = lhs;
        res += rhs;
        return res;
    }
    
    Uri operator+(const Uri & lhs, const Tag & rhs)
    {
        Uri res = lhs;
        res += rhs;
        return res;
    }
       
    Uri & Uri::operator+=(const Uri & rhs)
    {
        if (&rhs != this)
        {
            std::vector<Tag> d = rhs.data_;
            data_.insert(data_.end(), RANGE(d));
        }
        else
            data_.insert(data_.end(), RANGE(rhs.tags()));
        
        return *this;
    }
    
    Uri & Uri::operator+=(const Tag & rhs)
    {
        data_.push_back(rhs);        
        return *this;
    }
    
    
    bool Uri::operator==(const Uri & rhs) const
    {
        return std::equal(RANGE(tags()), RANGE(rhs.tags()));
    }
    
    bool Uri::operator!=(const Uri & rhs) const
    {
        return !operator==(rhs);
    }
    
    bool Uri::operator<(const Uri & rhs) const
    {
        return data_ < rhs.data_;
    }
    
    const Tag & Uri::back() const
    {
        return data_.back();
    }
    
    std::string Uri::string() const
    {
        std::ostringstream str;
        str << *this;
        return str.str();
    }
    
    std::ostream & operator<<(std::ostream & str, const Uri & ns)
    {
        for(auto it = ns.data_.begin(); it != ns.data_.end(); str << *it++)
            if(it != ns.data_.begin())
                str << SEPARATOR;
        
        return str;
    }
    
    namespace {
        
        template <class T>
        inline void hash_combine(std::size_t& seed, const T& v)
        {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        
    }
    
    std::size_t Uri::hash() const
    {
        std::size_t seed = 0;
        for(const auto & tag : tags())
            hash_combine(seed, tag);
        return seed;
    }
    
} }
