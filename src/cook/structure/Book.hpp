#ifndef HEADER_cook_structure_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_Book_hpp_ALREADY_INCLUDED

#include "cook/structure/Tag.hpp"
#include "cook/structure/Recipe.hpp"
#include "cook/util/Range.hpp"
#include "gubg/std/filesystem.hpp"
#include <functional>
#include <list>

namespace cook { namespace structure {
    
    class Book
    {   
        using RecipeMap = std::map<Tag, Recipe>;
        using BookMap = std::map<Tag, Book>;
        
    public:
        using BookIterator = BookMap::iterator;
        using ConstBookIterator = BookMap::const_iterator;
        using RecipeIterator = RecipeMap::iterator;
        using ConstRecipeIterator = RecipeMap::const_iterator;

        explicit Book(const std::filesystem::path & script_fn, const Uri & uri = Uri()) :
            script_fn_(script_fn),  
            uri_(uri),
            display_name_(uri.string())
        {
        }
        
        bool add_subbook(Book *& book, const Tag & tag, const std::filesystem::path & fn);
        bool add_recipe(Recipe *& recipe, const Tag & tag);
        bool add_recipe(Recipe *& recipe, const Tag & tag, const std::filesystem::path & fn);
        void print() const;
        
        bool has_recipe(const Tag & tag) const;
        bool has_subbook(const Tag & tag) const;
        std::string string() const;
        
        void set_display_name(const std::string & display_name) { display_name_ = display_name; }
        const std::string & display_name() const                { return display_name_; }
        
        util::Range<BookIterator> subbooks()                    { return util::make_range(subbooks_); }
        util::Range<ConstBookIterator> subbooks() const         { return util::make_range(subbooks_); }
        util::Range<RecipeIterator> recipes()                   { return util::make_range(recipes_); }
        util::Range<ConstRecipeIterator> recipes() const        { return util::make_range(recipes_); }
                
        const Uri & uri() const { return uri_; }
        const std::filesystem::path & script_filename() const { return script_fn_; }
        
    private:
        Book(const Book &) = delete;
        Book & operator=(const Book &) = delete;
        Book(Book &&) = delete;
        Book & operator=(Book &&) = delete;
        
        const Uri uri_;
        const std::filesystem::path script_fn_;
        std::string display_name_;
        
        RecipeMap recipes_;
        BookMap subbooks_;
    };
} }

#endif
