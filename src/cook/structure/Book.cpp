#include "cook/structure/Book.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace structure {
          
    bool Book::add_subbook(Book *& book, const Tag & tag, const std::filesystem::path & script_fn)
    {
        MSS_BEGIN(bool);
        MSS(!tag.empty());
        
        MSS(subbooks_.find(tag) == subbooks_.end());
        
        Uri child = uri() + tag;
        
        MSS(script_fn.has_filename());
        
        auto it = subbooks_.emplace(std::piecewise_construct, std::forward_as_tuple(tag), std::forward_as_tuple(script_fn, child)).first;
        book = &it->second;
        
        MSS_END();
    }

    bool Book::add_recipe(Recipe *& recipe, const Tag & tag)
    {
        return add_recipe(recipe, tag, script_filename());
    }
    
    bool Book::add_recipe(Recipe *& recipe, const Tag & tag, const std::filesystem::path & script_fn)
    {
        MSS_BEGIN(bool);
        MSS(!tag.empty());
        
        MSS(recipes_.find(tag) == recipes_.end());
        
        auto it = recipes_.emplace(std::piecewise_construct, std::forward_as_tuple(tag), std::forward_as_tuple(tag, script_fn, this)).first;
        
        recipe = &it->second;

        MSS_END();
    }
    
        
    bool Book::has_recipe(const Tag & tag) const
    {
        return recipes_.find(tag) != recipes_.end();
    }
    
    bool Book::has_subbook(const Tag & tag) const
    {
        return recipes_.find(tag) != recipes_.end();
    }
    
    
    std::string Book::string() const
    {
        return uri().string();
    }
    
    void Book::print() const
    {
        std::cout << "Book: " << uri().string() << std::endl;
        if (!recipes_.empty())
        {
            std::cout << "  - recipes (" << recipes_.size() << ")" << std::endl;
            for(const auto & p : recipes_)
                std::cout << "       " << p.second.string() << std::endl;
        }
        
        if (!subbooks_.empty())
        {
            std::cout << "  - subbooks (" << subbooks_.size() << ")" << std::endl;
            for(const auto & p : subbooks_)
                std::cout << "       " << p.second.string() << std::endl;
        }
    }
    
} }
