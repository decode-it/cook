#include "cook/chai/Details.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace chai {
    
    bool BookWrapper::add_dir(const std::string & dir)
    {
        MSS_BEGIN(bool);
        
        ScriptStack & stack = info_.open_scripts;
        
        // add a new script info to the stack
        std::filesystem::path script_fn;
        {
            const std::filesystem::path sdir(dir);
            if (stack.empty() || sdir.is_absolute())
                script_fn = sdir;
            else
                script_fn = stack.top().parent_path() / sdir;
            
            script_fn /= std::filesystem::path("recipes.chai");
            stack.push(script_fn);
            Book & book = book_;
        }
        
        {
            VerbosityWriter v(1, info_);
            v.str() << "Processing script file " << script_fn.string() << " started" << std::endl;
            // evaluate the script
            MSS(info_.engine.eval_file(script_fn));
            
        }
               
        // and pop back from the stack
        stack.pop();
        
        MSS_END();
    }
    
    bool BookWrapper::add_book(const std::string & tag, std::function<void (BookWrapper &)> callback)
    {
        MSS_BEGIN(bool);
        
        BookStack & stack = info_.open_books;
        
        std::pair<Tag, bool> p = Tag::construct(tag);
        MSS(p.second, std::cerr << "'" << tag << "' is not a valid tag" << std::endl);
        MSS(!p.first.empty(), std::cerr << "'" << tag << "' is not a valid tag" << std::endl);
        
        MSS(book_.find_element(p.first) == 0, std::cerr << "A book with name '" << tag << "' already exists in book '" << book_.string() << "'" << std::endl);
        
        Book * child = nullptr;
        
        {
            VerbosityWriter v(1, info_);
            v.str() << "Adding new book " << p.first << " into " << book_.uri() << std::endl;
        
            MSS(book_.add_subbook(child, p.first, info_.last_script()));
            MSS(!!child);
        }
        
        
        BookWrapper ldr(info_, *child);
        
        {
            VerbosityWriter v(2, info_);
            v.str() << "Starting callback for Book: " << child->uri() << std::endl;
        
            // and call the callback
            stack.push(child);
            callback(ldr);
            stack.pop();
        }
        
        MSS_END();
    }
    
    bool GlobalInfo::add_dir(const std::string & dir)
    {
        return BookWrapper(*this, last_book()).add_dir(dir);
    }
    
    bool GlobalInfo::add_book(const std::string & tag, std::function<void (BookWrapper &)> callback)
    {
        return BookWrapper(*this, last_book()).add_book(tag, callback);
    }
    
   
    bool GlobalInfo::create_new_recipe(const std::string & tag, std::function<void (RecipeWrapper &)> callback)
    {
        return BookWrapper(*this, last_book()).create_new_recipe(tag, callback);
    }
    
    void GlobalInfo::set_display_name(const std::string & display_name)
    {
        return BookWrapper(*this, last_book()).set_display_name(display_name);
    }
    
    void GlobalInfo::print()
    {
        return BookWrapper(*this, last_book()).print();
    }
    
    
    bool BookWrapper::create_new_recipe(const std::string & tag, std::function<void (RecipeWrapper &)> callback)
    {
        MSS_BEGIN(bool);
        
        std::pair<Tag, bool> p = Tag::construct(tag);
        MSS(p.second, std::cerr << "'" << tag << "' is not a valid tag" << std::endl);
        MSS(!p.first.empty(), std::cerr << "'" << tag << "' is not a valid tag" << std::endl);
               
        // do we have this recipe?
        MSS(book_.find_element(p.first) == 0, std::cerr << "A recipe with name '" << tag << "' already exists in book '" << book_.string() << "'" << std::endl);
        
        Recipe * recipe = nullptr;
        {
            VerbosityWriter v(2, info_);
            v.str() << "Adding new recipe " << tag << " into book " << book_.uri() << std::endl;
            MSS(book_.add_recipe(recipe, p.first, info_.last_script()));
            MSS(!!recipe);
        }
        
        RecipeWrapper wrapper(info_, *recipe);
        
        {
            VerbosityWriter v(2, info_);
            v.str() << "Starting callback for Recipe: " << recipe->uri() << std::endl;
        
            callback(wrapper);
        }
        
        MSS_END();
    }

    
} } 