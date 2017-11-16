#ifndef HEADER_cook_view_chai_Runner_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_chai_Runner_hpp_ALREADY_INCLUDED

#include "cook/view/chai/Engine.hpp"
#include "cook/view/Logger.hpp"
#include "cook/presenter/Interface.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/mss.hpp"
#include <vector>
#include <functional>

namespace cook { namespace view { namespace chai { 

    struct RunnerInfo
    {
        using ScriptStack = std::vector<std::filesystem::path>;
        ScriptStack script_stack;
        presenter::Reference presenter;
        Logger &logger;

        RunnerInfo(presenter::Reference presenter, Logger &logger): presenter(presenter), logger(logger) {}

        std::string indent() const {return std::string(script_stack.size()*2, ' ');}
        std::filesystem::path current_script() const { return script_stack.back(); }
        std::filesystem::path working_directory() const { return std::filesystem::current_path() / current_script().parent_path(); }
    };

    class Recipe
    {
    public:
        Recipe(RunnerInfo &info): info_(info) {}
        Recipe(RunnerInfo &info, model::Uri uri): info_(info), uri_(uri) {}

        void chai_print() const
        {
            std::cout << "Recipe " << uri_.str('/','/') << std::endl;
        }
        void chai_add_3(const std::string &dir, const std::string &pattern, const std::string &option)
        {
            info_.logger.log(Info) << info_.indent() << ">> Add files from " << dir << " // " << pattern << " as " << option << std::endl;
            const Strings args = {uri_.str(), dir, pattern, option};
            if (!info_.presenter.set("model.recipe.add", args))
            {
                std::ostringstream oss; oss << "Could not add files";
                throw chaiscript::exception::eval_error(oss.str());
            }
            info_.logger.log(Info) << info_.indent() << "<< Add files from " << dir << " // " << pattern << " as " << option << std::endl;
        }
        void chai_add_2(const std::string &dir, const std::string &pattern) { chai_add_3(dir, pattern, ""); }
        void chai_add_1(const std::string &pattern) { chai_add_3("", pattern, ""); }
        void chai_depends_on(const std::string &rn)
        {
            info_.logger.log(Info) << info_.indent() << ">> Adding dependency on " << rn << std::endl;
            const Strings args = {uri_.str(), rn};
            info_.presenter.set("model.recipe.depends_on", args);
            info_.logger.log(Info) << info_.indent() << "<< Adding dependency on " << rn << std::endl;
        }
        void chai_display_name(const std::string &dn)
        {
            info_.logger.log(Info) << info_.indent() << ">> Setting display name to " << dn << std::endl;
            const Strings args = {uri_.str(), dn};
            info_.presenter.set("model.recipe.display_name", args);
            info_.logger.log(Info) << info_.indent() << "<< Setting display name to " << dn << std::endl;
        }
        void chai_library(const std::string &name)
        {
            info_.logger.log(Info) << info_.indent() << ">> Adding library " << name << std::endl;
            const Strings args = {uri_.str(), name};
            info_.presenter.set("model.recipe.library", args);
            info_.logger.log(Info) << info_.indent() << "<< Adding library " << name << std::endl;
        }

    private:
        RunnerInfo &info_;
        model::Uri uri_;
    };

    class Book
    {
    public:
        Book(RunnerInfo &info): info_(info) {}
        Book(RunnerInfo &info, model::Uri uri): info_(info), uri_(uri) {}

        void chai_print() const
        {
            std::cout << "Book " << uri_.str('/','/') << std::endl;
        }
        void chai_book(const std::string &name, std::function<void(Book &)> callback)
        {
            info_.logger.log(Info) << info_.indent() << ">> Book " << name << std::endl;
            model::Uri uri = uri_;
            uri.add_path_part(name);
            info_.presenter.set("model.book.create", uri.str());
            Book book{info_, uri};
            callback(book);
            info_.logger.log(Info) << info_.indent() << "<< Book " << name << std::endl;
        }
        void chai_recipe_3(const std::string &name, const std::string &type, std::function<void(Recipe &)> callback)
        {
            info_.logger.log(Info) << info_.indent() << ">> Recipe " << name << " for type \"" << type << "\"" << std::endl;
            model::Uri uri = uri_;
            uri.set_name(name);
            const Strings args = {uri.str(), type, info_.working_directory().string()};
            if (!info_.presenter.set("model.recipe.create", args))
            {
                std::ostringstream oss; oss << "Recipe \"" << uri << "\" already exists";
                throw chaiscript::exception::eval_error(oss.str());
            }
            Recipe recipe{info_, uri};
            callback(recipe);
            info_.logger.log(Info) << info_.indent() << "<< Recipe " << name << " for type \"" << type << "\"" << std::endl;
        }
        void chai_recipe_2(const std::string &name, std::function<void(Recipe &)> callback) { chai_recipe_3(name, "", callback); }

    private:
        RunnerInfo &info_;
        model::Uri uri_;
    };

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
                logger_.log(Info) << runner_info_.indent() << "Skipping " << file_or_dir << ", execution went wrong" << std::endl;
                return execute_ok_;
            }

            const auto script_fn = expand_(file_or_dir);
            logger_.log(Info) << runner_info_.indent() << ">> Script " << script_fn << std::endl;
            runner_info_.script_stack.push_back(script_fn);
            presenter_.set("script.filename", script_fn.string());

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
                for (auto ix = runner_info_.script_stack.size(); ix-- > 0;)
                    *os << ix << "\t" << runner_info_.script_stack[ix] << std::endl;
            }

            runner_info_.script_stack.pop_back();
            presenter_.set("script.filename", (runner_info_.script_stack.empty() ? std::string{} : runner_info_.script_stack.back().string()));
            logger_.log(Info) << runner_info_.indent() << "<< Script " << script_fn << (execute_ok_ ? " (OK)" : " (KO)") << std::endl;

            return execute_ok_;
        }

        //Functions called from chaiscript
        void chai_include(const std::string &file_or_dir) {execute(file_or_dir);}

    private:
        void setup_chai_functions_()
        {
            auto &chai = chai_engine_.raw();

            chai.add(chaiscript::fun(&Runner::chai_include, this), "include");

            //user_type and constructor are added to make the books copyable in chaiscript.
            chai.add(chaiscript::user_type<Book>(), "Book");
            chai.add(chaiscript::constructor<Book(const Book &)>(), "Book");
            chai.add_global(chaiscript::var(root_book_), "root");
            chai.add(chaiscript::fun(&Book::chai_print), "print");
            chai.add(chaiscript::fun(&Book::chai_book), "book");
            chai.add(chaiscript::fun(&Book::chai_recipe_3), "recipe");
            chai.add(chaiscript::fun(&Book::chai_recipe_2), "recipe");

            //user_type and constructor are added to make the recipes copyable in chaiscript.
            chai.add(chaiscript::user_type<Recipe>(), "Recipe");
            chai.add(chaiscript::constructor<Recipe(const Recipe &)>(), "Recipe");
            chai.add(chaiscript::fun(&Recipe::chai_print), "print");
            chai.add(chaiscript::fun(&Recipe::chai_add_3), "add");
            chai.add(chaiscript::fun(&Recipe::chai_add_2), "add");
            chai.add(chaiscript::fun(&Recipe::chai_add_1), "add");
            chai.add(chaiscript::fun(&Recipe::chai_depends_on), "depends_on");
            chai.add(chaiscript::fun(&Recipe::chai_display_name), "display_name");
            chai.add(chaiscript::fun(&Recipe::chai_library), "library");
        }

        std::filesystem::path expand_(const std::string &file_or_dir)
        {
            std::filesystem::path script_fn;

            const std::filesystem::path fod_path(file_or_dir);

            if (runner_info_.script_stack.empty() || fod_path.is_absolute())
                script_fn = fod_path;
            else
                script_fn = runner_info_.script_stack.back().parent_path() / fod_path;

            if (script_fn.empty())
                script_fn = std::filesystem::path("recipes.chai");
            else if (std::filesystem::is_directory(script_fn))
                script_fn /= std::filesystem::path("recipes.chai");

            return script_fn;
        }

        presenter::Reference presenter_;
        Logger &logger_;
        RunnerInfo runner_info_{presenter_, logger_};
        Book root_book_{runner_info_};

        cook::view::chai::Engine chai_engine_;

        bool execute_ok_ = true;
    };

} } } 

#endif
