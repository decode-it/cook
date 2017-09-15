#ifndef HEADER_cook_structure_Element_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_Element_hpp_ALREADY_INCLUDED

#include "cook/structure/Tag.hpp"
#include "cook/structure/Uri.hpp"
#include "cook/util/Range.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/mss.hpp"
#include <map>

namespace cook { namespace structure { 
    
    enum class Type
    {
        Book,
        Recipe
    };
    
    inline std::string to_s(Type type)
    {
        switch(type)
        {
            case Type::Book:    return "book";
            case Type::Recipe:  return "recipe";
            default:
                return "";
        }
    }
    
    inline std::ostream & operator<<(std::ostream & str, Type type)
    {
        str << to_s(type);        
        return str;
    }
    
    
    
    class Element 
    {
    public:
        using ElementMap = std::map<Tag, Element *>;
        using iterator = ElementMap::const_iterator;
        
        Element(Type type, const std::filesystem::path & script_fn, const Tag & tag = Tag(), Element * parent = nullptr) :
            uri_( (parent == 0 ? Uri() : parent->uri()) + tag),
            script_fn_(script_fn),
            parent_(parent),
            type_(type)
        { 
        }
        
        virtual ~Element() = 0;
        
        const Uri & uri() const                                     { return uri_; }
        const std::filesystem::path & script_filename() const       { return script_fn_; }
        const std::string & display_name() const                    { return display_name_; }
        void set_display_name(const std::string & display_name)     { display_name_ = display_name; }
        Element * parent() const                                    { return parent_; }
        Type type() const                                           { return type_; }
        
        bool add_element(const Tag & tag, Element * element)
        {
            MSS_BEGIN(bool);
            MSS(map_.find(tag) == map_.end());
            MSS(accepts_element_(tag, element));
            
            map_[tag] = element;
            
            MSS_END();
        }
        
        Element * find_element(const Tag & tag) const
        {
            auto it = map_.find(tag);
            if (it == map_.end())
                return nullptr;
            return it->second;
        }
        
        util::Range<iterator> elements() const
        {
            return util::make_range(map_);
        }
        
        
    protected:
        virtual bool accepts_element_(const Tag & tag, Element * element) const { return true; }
        
    private:
        const std::filesystem::path script_fn_;
        const Uri uri_;
        std::string display_name_;
        ElementMap map_;
        Element * const parent_;
        const Type type_;
    };
    
    inline Element::~Element()
    {
    }
    
} }

#endif
