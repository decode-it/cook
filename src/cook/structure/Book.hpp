#ifndef HEADER_cook_structure_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_Book_hpp_ALREADY_INCLUDED

#include "cook/structure/Element.hpp"
#include "cook/structure/Tag.hpp"
#include "cook/structure/Recipe.hpp"
#include "cook/util/Range.hpp"
#include "gubg/std/filesystem.hpp"
#include <functional>
#include <list>

namespace cook { namespace structure {
    
    class Book : public Element
    {   
    public:
        explicit Book(const std::filesystem::path & script_fn, const Tag & tag = Tag(), Book * parent = nullptr) :
            Element(Type::Book, script_fn, tag, parent)
        {
            add_script_file(script_fn);
        }
        
        bool get_subbook(Book *& book, const Tag & tag, const std::filesystem::path & fn);
        bool add_recipe(Recipe *& recipe, const Tag & tag, const std::filesystem::path & fn);
        void add_script_file(const std::filesystem::path & script_file);
        const std::set<std::filesystem::path> & script_files() const { return script_files_; }
        
        void print() const;        
        std::string string() const;
        
    private:
        Book(const Book &) = delete;
        Book & operator=(const Book &) = delete;
        Book(Book &&) = delete;
        Book & operator=(Book &&) = delete;

        std::set<std::filesystem::path> script_files_;
    };
} }

#endif
