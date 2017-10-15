#include "cook/structure/Book.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace structure {
          
    bool Book::add_subbook(Book *& book, const Tag & tag, const std::filesystem::path & script_fn)
    {
        MSS_BEGIN(bool);
        MSS(!tag.empty());
        MSS(script_fn.has_filename());
        
        book = new Book(script_fn, tag, this);
        MSS(add_element(tag, book), delete book);
        
        MSS_END();
    }
    
    bool Book::add_recipe(Recipe *& recipe, const Tag & tag, const std::filesystem::path & script_fn)
    {
        MSS_BEGIN(bool);
        MSS(!tag.empty());
        MSS(script_fn.has_filename());
        
        recipe = new Recipe(script_fn, tag, this);
        MSS(add_element(tag, recipe), delete recipe);

        MSS_END();
    }

    void Book::add_script_file(const std::filesystem::path & script_file)
    {
        script_files_.insert(script_file);
    }
    
    std::string Book::string() const
    {
        return uri().string();
    }
    
    void Book::print() const
    {
//        std::cout << "Book: " << uri().string() << std::endl;
//        if (!recipes_.empty())
//        {
//            std::cout << "  - recipes (" << recipes_.size() << ")" << std::endl;
//            for(const auto & p : recipes_)
//                std::cout << "       " << p.second.string() << std::endl;
//        }
        
//        if (!subbooks_.empty())
//        {
//            std::cout << "  - subbooks (" << subbooks_.size() << ")" << std::endl;
//            for(const auto & p : subbooks_)
//                std::cout << "       " << p.second.string() << std::endl;
//        }
    }
    
} }
