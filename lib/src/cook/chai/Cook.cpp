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
        
        std::string Cook::working_directory() const
        {
            return context_->current_working_directory().string();
        }
        std::string Cook::working_directory(bool make_absolute) const
        {
            if (make_absolute)
                return gubg::filesystem::combine(std::filesystem::current_path(), context_->current_working_directory()).string();
            else
                return working_directory();
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

} }
