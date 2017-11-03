#include "cook/chai/Details.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace chai {


    
    void BookWrapper::include(const std::string & file_or_dir)
    {
        ScriptStack & stack = info_.open_scripts;
        
        // add a new script info to the stack
        std::filesystem::path script_fn;
        {
            const std::filesystem::path fod_path(file_or_dir);

            if (stack.empty() || fod_path.is_absolute())
                script_fn = fod_path;
            else
                script_fn = stack.top().parent_path() / fod_path;

            if (script_fn.empty())
                script_fn = std::filesystem::path("recipes.chai");
            else if (std::filesystem::is_directory(script_fn))
                script_fn /= std::filesystem::path("recipes.chai");

            stack.push(script_fn);

            // add to the recipes in the book
            assert(!info_.open_books.empty());
            info_.open_books.top()->add_script_file(script_fn);
        }
        
        {
            VerbosityWriter v(1, info_);
            v.os() << "Processing script file " << script_fn.string() << " started" << std::endl;
            // evaluate the script
            info_.engine.eval_file(script_fn);
        }
               
        // and pop back from the stack
        stack.pop();
    }
    
    void BookWrapper::add_book(const std::string & tag, std::function<void (BookWrapper &)> callback)
    {
        BookStack & stack = info_.open_books;
        
        std::pair<Tag, bool> p = Tag::construct(tag);
        if(!p.second || p.first.empty())
            throw chaiscript::exception::eval_error("Not a valid tag");
        
//        MSS(book_.find_element(p.first) == 0, std::cerr << "A book with name '" << tag << "' already exists in book '" << book_.string() << "'" << std::endl);
        
        Book * child = nullptr;
        
        {
            VerbosityWriter v(1, info_);
            v.os() << "Adding new book " << p.first << " into " << book_.uri() << std::endl;
        
            if (!book_.add_subbook(child, p.first, info_.last_script()) || child == nullptr)
                throw chaiscript::exception::eval_error("Internal logic error while adding book");
        }
        
        
        BookWrapper ldr(info_, *child);
        
        {
            VerbosityWriter v(2, info_);
            v.os() << "Starting callback for Book: " << child->uri() << std::endl;
        
            // and call the callback
            stack.push(child);
            callback(ldr);
            stack.pop();
        }
    }
    
    void GlobalInfo::include(const std::string & file_or_dir)
    {
        BookWrapper(*this, last_book()).include(file_or_dir);
    }
    
    void GlobalInfo::add_book(const std::string & tag, std::function<void (BookWrapper &)> callback)
    {
        BookWrapper(*this, last_book()).add_book(tag, callback);
    }
    
   
    void GlobalInfo::create_new_recipe(const std::string & tag, std::function<void (RecipeWrapper &)> callback)
    {
        BookWrapper(*this, last_book()).create_new_recipe(tag, callback);
    }
    
    void GlobalInfo::set_display_name(const std::string & display_name)
    {
        BookWrapper(*this, last_book()).set_display_name(display_name);
    }
    
    void GlobalInfo::print()
    {
        BookWrapper(*this, last_book()).print();
    }
    
    
    void BookWrapper::create_new_recipe(const std::string & tag, std::function<void (RecipeWrapper &)> callback)
    {
        std::pair<Tag, bool> p = Tag::construct(tag);

        if(!p.second || p.first.empty())
            throw chaiscript::exception::eval_error("Not a valid tag");
               
        // do we have this recipe?
        if (book_.find_element(p.first) != 0)
            throw chaiscript::exception::eval_error("A recipe with the same tag already exists in this book");

        Recipe * recipe = nullptr;
        {
            VerbosityWriter v(2, info_);
            v.os() << "Adding new recipe " << tag << " into book " << book_.uri() << std::endl;

            if (!book_.add_recipe(recipe, p.first, info_.last_script()) || recipe == nullptr)
                throw chaiscript::exception::eval_error("Internal logic error while adding recipe");
        }
        
        RecipeWrapper wrapper(info_, *recipe);
        
        {
            VerbosityWriter v(2, info_);
            v.os() << "Starting callback for Recipe: " << recipe->uri() << std::endl;
        
            callback(wrapper);
        }
    }

    
} } 
