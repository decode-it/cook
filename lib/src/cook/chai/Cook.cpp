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
            return working_directory(false);
        }
        std::string Cook::working_directory(bool make_absolute) const
        {
            if (make_absolute)
                return gubg::filesystem::combine(std::filesystem::current_path(), context_->current_working_directory()).string();
            else
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
            const auto & p = context_->dirs().output();

            if (make_absolute) 
                return gubg::filesystem::normalize(p).string();
            else
                return gubg::filesystem::get_relative_to(working_directory(), p).string();
        }

        std::string Cook::temporary_directory(bool make_absolute) const
        {
            const auto & p = context_->dirs().temporary();

            if (make_absolute) 
                return gubg::filesystem::normalize(p).string();
            else
                return gubg::filesystem::get_relative_to(working_directory(), p).string();
        }

} }
