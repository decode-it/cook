#include "cook/chai/Cook.hpp"
#include "cook/chai/Context.hpp"

namespace cook { namespace chai { 

        Cook::Cook(model::Book * root_book, Context * context) : 
            Book(root_book, context),
            context_(context),
            toolchain_(&context->toolchain(), context)   
        {
        }

        Book Cook::operator[](const model::Uri & uri)
        {
            return root().book(uri);
        }

        Book Cook::root() const
        {
            return static_cast<const Book &>(*this);
        }
        
        std::string Cook::script_local_directory() const
        {
            //TODO: later, we can provide a relative script_local_directory, relative to the path of the root script that was loaded
            return context_->current_working_directory().string();
        }

        std::string Cook::project_name() const
        {
            return context_->project_name();
        }

        void Cook::set_project_name(const std::string & name)
        {
            return context_->set_project_name(name);
        }

        Toolchain Cook::toolchain() const
        {
            return toolchain_;
        }
        
        std::string Cook::output_directory() const
        {
            return output_directory(false);
        }

        std::string Cook::temporary_directory() const
        {
            return temporary_directory(false);
        }

        std::string Cook::output_directory(bool make_absolute) const
        {
            if (make_absolute) 
                return gubg::filesystem::normalize(context_->dirs().output(true)).string();
            else
                return gubg::filesystem::get_relative_to(std::filesystem::current_path(), context_->dirs().output()).string();
        }

        std::string Cook::temporary_directory(bool make_absolute) const
        {
            if (make_absolute) 
                return gubg::filesystem::normalize(context_->dirs().temporary(true)).string();
            else
                return gubg::filesystem::get_relative_to(std::filesystem::current_path(), context_->dirs().temporary()).string();
        }

} }
