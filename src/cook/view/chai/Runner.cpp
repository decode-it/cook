#include "cook/view/chai/Runner.hpp"
#include "cook/view/chai/Engine.hpp"
#include "cook/view/chai/RunnerInfo.hpp"
#include "cook/view/chai/Book.hpp"
#include "cook/view/chai/File.hpp"
#include "cook/view/chai/Recipe.hpp"
#include "cook/view/chai/Util.hpp"
#include "cook/view/Book.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace view { namespace chai {

struct Runner::D
{
    D(Presenter * presenter, Logger &logger)
        : presenter_(presenter),
          logger_(logger)
    {
    }
    Presenter * presenter_;
    Logger &logger_;
    RunnerInfo runner_info_{presenter_, logger_};
    Book root_book_{runner_info_};
    Engine chai_engine_;

    std::filesystem::path project_path_;

    bool execute_ok_ = true;
};

Runner::Runner(Presenter *presenter, Logger &logger)
    : d_(new D(presenter, logger))
{
    setup_chai_functions_();
}

Runner::~Runner()
{
}

bool Runner::process(const std::list<std::string> & input_files)
{
    MSS_BEGIN(bool);

    // set the project directory
    {
        using C = presenter::Command;
        D & d = *d_;
        d.project_path_ = std::filesystem::current_path();
        MSS(d.presenter_->set( {C::env, C::dir, C::project}, as_any(d.project_path_.string()) ));
    }

    if (input_files.empty())
        MSS(execute_(""));
    else
        for (const auto & file : input_files)
            MSS(execute_(file));

    MSS_END();
}

bool Runner::execute_(const std::string &file_or_dir)
{
    MSS_BEGIN(bool);

    using C = presenter::Command;

    D & d = *d_;

    if (!d.execute_ok_)
    {
        d.logger_.log(Info) << d.runner_info_.indent() << "Skipping " << file_or_dir << ", execution went wrong" << std::endl;
        MSS(false);
    }

    const auto script_fn = expand_(file_or_dir);
    d.logger_.log(Info) << d.runner_info_.indent() << ">> Script " << script_fn << std::endl;
    d.runner_info_.script_stack.push_back(script_fn);

    d.presenter_->set({C::script, C::filename}, as_any(script_fn.string()));

    if (d.runner_info_.script_stack.size() == 1)
        //Needed to get the script filename for the root node, as this is never created
        d.presenter_->set({C::model, C::book, C::create}, as_any<std::string>("/"));

    std::ostream *os = nullptr;
    try
    {
        d.chai_engine_.eval_file(script_fn);
    }
    catch (const chaiscript::exception::file_not_found_error &exc)
    {
        os = &d.logger_.log(Error); *os << "Could not find " << script_fn << ": " << exc.what() << std::endl;
    }
    catch (const chaiscript::exception::eval_error &exc)
    {
        os = &d.logger_.log(Error); *os << "Could not run " << script_fn << ": " << exc.pretty_print() << std::endl;
    }
    if (!!os)
    {
        d.execute_ok_ = false;
        for (auto ix = d.runner_info_.script_stack.size(); ix-- > 0;)
            *os << ix << "\t" << d.runner_info_.script_stack[ix] << std::endl;
    }

    d.runner_info_.script_stack.pop_back();

    {
        const auto & args = as_any((d.runner_info_.script_stack.empty() ? std::string{} : d.runner_info_.script_stack.back().string()));
        d.presenter_->set({C::script, C::filename}, args);
    }

    d.logger_.log(Info) << d.runner_info_.indent() << "<< Script " << script_fn << (d.execute_ok_ ? " (OK)" : " (KO)") << std::endl;

    MSS(d.execute_ok_);

    MSS_END();
}

void Runner::setup_chai_functions_()
{
    D & d = *d_;
    auto &chai = d.chai_engine_.raw();

    chai.add(chaiscript::fun(&Runner::chai_include, this), "include");

    chai.add(file_module());
    chai.add(book_module());    
    chai.add(recipe_module());

    chai.add(util_module());

    chai.add_global(chaiscript::var(d.root_book_), "root");
}

std::filesystem::path Runner::expand_(const std::string &file_or_dir)
{
    D & d = *d_;
    std::filesystem::path script_fn;

    const std::filesystem::path fod_path(file_or_dir);

    if (d.runner_info_.script_stack.empty() || fod_path.is_absolute())
        script_fn = fod_path;
    else
        script_fn = d.runner_info_.script_stack.back().parent_path() / fod_path;

    if (script_fn.empty())
        script_fn = std::filesystem::path("recipes.chai");
    else if (std::filesystem::is_directory(script_fn))
        script_fn /= std::filesystem::path("recipes.chai");

    return script_fn;
}

} } }
