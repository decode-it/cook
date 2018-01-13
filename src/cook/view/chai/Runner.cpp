#include "cook/view/chai/Runner.hpp"


namespace cook { namespace view { namespace chai {

bool Runner::execute(const std::string &file_or_dir)
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
    if (runner_info_.script_stack.size() == 1)
        //Needed to get the script filename for the root node, as this is never created
        presenter_.set("model.book.create", "/");

    std::ostream *os = nullptr;
    try { chai_engine_.eval_file(script_fn); }
    catch (const chaiscript::exception::file_not_found_error &exc)
    {
        os = &logger_.log(Error); *os << "Could not find " << script_fn << ": " << exc.what() << std::endl;
    }
    catch (const chaiscript::exception::eval_error &exc)
    {
        os = &logger_.log(Error); *os << "Could not run " << script_fn << ": " << exc.pretty_print() << std::endl;
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

void Runner::setup_chai_functions_()
{
    auto &chai = chai_engine_.raw();

    chai.add(chaiscript::fun(&Runner::chai_include, this), "include");

    //user_type and constructor are added to make the books copyable in chaiscript.
    chai.add(chaiscript::user_type<Book>(), "Book");
    chai.add(chaiscript::constructor<Book(const Book &)>(), "Book");
    chai.add_global(chaiscript::var(root_book_), "root");
    chai.add(chaiscript::fun(&Book::chai_display_name), "display_name");
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
    chai.add(chaiscript::fun(&Recipe::chai_library_path), "library_path");
}

std::filesystem::path Runner::expand_(const std::string &file_or_dir)
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

} } }
