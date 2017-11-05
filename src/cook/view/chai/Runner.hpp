#ifndef HEADER_cook_view_chai_Runner_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_chai_Runner_hpp_ALREADY_INCLUDED

#include "cook/view/chai/Engine.hpp"
#include "cook/view/Logger.hpp"
#include "cook/presenter/Interface.hpp"
#include "gubg/mss.hpp"
#include <vector>
#include <functional>

namespace cook { namespace view { namespace chai { 

    class Runner
    {
    public:
        Runner(presenter::Reference presenter, Logger &logger): presenter_(presenter), logger_(logger)
        {
            setup_chai_functions_();
        }
        bool execute(const std::string &file_or_dir)
        {
            if (!execute_ok_)
            {
                logger_.log(Info) << indent_() << "Skipping " << file_or_dir << ", execution went wrong" << std::endl;
                return execute_ok_;
            }

            const auto script_fn = expand_(file_or_dir);
            logger_.log(Info) << indent_() << ">> Script " << script_fn << std::endl;
            script_stack_.push_back(script_fn);

            std::ostream *os = nullptr;
            try { chai_engine_.eval_file(script_fn); }
            catch (const chaiscript::exception::file_not_found_error &exc)
            {
                os = &logger_.log(Error); *os << "Could not find " << script_fn << ": " << exc.what() << std::endl;
            }
            catch (const chaiscript::exception::eval_error &exc)
            {
                os = &logger_.log(Error); *os << "Could not run " << script_fn << ": " << exc.what() << std::endl;
            }
            if (!!os)
            {
                execute_ok_ = false;
                for (auto ix = script_stack_.size(); ix-- > 0;)
                    *os << ix << "\t" << script_stack_[ix] << std::endl;
            }

            script_stack_.pop_back();
            logger_.log(Info) << indent_() << "<< Script " << script_fn << (execute_ok_ ? " (OK)" : " (KO)") << std::endl;

            return execute_ok_;
        }

        //Functions called from chaiscript
        void chai_include(const std::string &file_or_dir) {execute(file_or_dir);}
        void chai_book(const std::string &name, std::function<void()> callback)
        {
            logger_.log(Info) << indent_() << ">> Book " << name << std::endl;
            presenter_.set("model.book.push", name);
            callback();
            presenter_.set("model.book.pop", name);
            logger_.log(Info) << indent_() << "<< Book " << name << std::endl;
        }
        void chai_recipe_3(const std::string &name, const std::string &type, std::function<void()> callback)
        {
            logger_.log(Info) << indent_() << ">> Recipe " << name << " for type \"" << type << "\"" << std::endl;
            if (false) {}
            else if (type == "")
            {
            }
            else if (type == "executable")
            {
            }
            else
            {
                std::ostringstream oss; oss << "Unknown recipe type \"" << type << "\"";
                throw chaiscript::exception::eval_error(oss.str());
            }
            if (!presenter_.set("model.recipe.create", name))
            {
                std::ostringstream oss; oss << "Recipe \"" << name << "\" already exists";
                throw chaiscript::exception::eval_error(oss.str());
            }
            callback();
            presenter_.set("model.recipe.close", name);
            logger_.log(Info) << indent_() << "<< Recipe " << name << " for type \"" << type << "\"" << std::endl;
        }
        void chai_recipe_2(const std::string &name, std::function<void()> callback) { chai_recipe_3(name, "", callback); }
        void chai_add_2(const std::string &dir, const std::string &pattern)
        {
            logger_.log(Info) << indent_() << ">> Add files from " << dir << " // " << pattern << std::endl;
            Strings args = {dir, pattern};
            if (!presenter_.set("model.recipe.add", args))
            {
                std::ostringstream oss; oss << "No current recipe when adding files";
                throw chaiscript::exception::eval_error(oss.str());
            }
            logger_.log(Info) << indent_() << "<< Add files from " << dir << " // " << pattern << std::endl;
        }

    private:
        void setup_chai_functions_()
        {
            auto &chai = chai_engine_.raw();
            chai.add(chaiscript::fun(&Runner::chai_include, this), "include");
            chai.add(chaiscript::fun(&Runner::chai_book, this), "book");
            chai.add(chaiscript::fun(&Runner::chai_recipe_3, this), "recipe");
            chai.add(chaiscript::fun(&Runner::chai_recipe_2, this), "recipe");
            chai.add(chaiscript::fun(&Runner::chai_add_2, this), "add");
        }
        std::filesystem::path expand_(const std::string &file_or_dir)
        {
            std::filesystem::path script_fn;

            const std::filesystem::path fod_path(file_or_dir);

            if (script_stack_.empty() || fod_path.is_absolute())
                script_fn = fod_path;
            else
                script_fn = script_stack_.back().parent_path() / fod_path;

            if (script_fn.empty())
                script_fn = std::filesystem::path("recipes.chai");
            else if (std::filesystem::is_directory(script_fn))
                script_fn /= std::filesystem::path("recipes.chai");

            return script_fn;
        }
        std::string indent_() const {return std::string(script_stack_.size()*2, ' ');}

        presenter::Reference presenter_;
        Logger &logger_;

        cook::view::chai::Engine chai_engine_;

        using ScriptStack = std::vector<std::filesystem::path>;
        ScriptStack script_stack_;
        bool execute_ok_ = true;
    };

} } } 

#endif
