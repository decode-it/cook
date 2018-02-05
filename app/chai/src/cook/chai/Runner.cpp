#include "cook/chai/Runner.hpp"
#include "cook/chai/Engine.hpp"
#include "cook/chai/RunnerInfo.hpp"
#include "cook/chai/Book.hpp"
#include "cook/chai/File.hpp"
#include "cook/chai/Flags.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/Util.hpp"
#include "cook/view/Book.hpp"
#include "cook/model/Uri.hpp"
#include "cook/API.h"
#include "gubg/mss.hpp"

namespace cook { namespace view { namespace chai {

struct Runner::D
{
    D(cook_Handle_t api, Logger &logger)
        : api_(api),
          logger_(logger),
          runner_info_(logger),
          root_book_(runner_info_, cook_API_root_book(api_))
    {
    }

    cook_Handle_t api_;
    Logger &logger_;
    RunnerInfo runner_info_;
    Book root_book_;
    Engine chai_engine_;

    std::filesystem::path project_path_;

    bool execute_ok_ = true;
};

Runner::Runner(cook_Handle_t api, Logger &logger)
    : d_(new D(api, logger))
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
        D & d = *d_;
        d.project_path_ = std::filesystem::current_path();
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

    D & d = *d_;

    if (!d.execute_ok_)
    {
        d.logger_.log(cook_NotificationType_Info) << d.runner_info_.indent() << "Skipping " << file_or_dir << ", execution went wrong" << std::endl;
        MSS(false);
    }

    const auto script_fn = expand_(file_or_dir);
    d.logger_.log(cook_NotificationType_Info) << d.runner_info_.indent() << ">> Script " << script_fn << std::endl;
    d.runner_info_.script_stack.push_back(script_fn);

    std::ostream *os = nullptr;
    try
    {
        d.chai_engine_.eval_file(script_fn);
    }
    catch (const chaiscript::exception::file_not_found_error &exc)
    {
        os = &d.logger_.log(cook_NotificationType_Error); *os << "Could not find " << script_fn << ": " << exc.what() << std::endl;
    }
    catch (const chaiscript::exception::eval_error &exc)
    {
        os = &d.logger_.log(cook_NotificationType_Error); *os << "Could not run " << script_fn << ": " << exc.pretty_print() << std::endl;
    }
    if (!!os)
    {
        d.execute_ok_ = false;
        for (auto ix = d.runner_info_.script_stack.size(); ix-- > 0;)
            *os << ix << "\t" << d.runner_info_.script_stack[ix] << std::endl;
    }

    d.runner_info_.script_stack.pop_back();
    d.logger_.log(cook_NotificationType_Info) << d.runner_info_.indent() << "<< Script " << script_fn << (d.execute_ok_ ? " (OK)" : " (KO)") << std::endl;

    MSS(d.execute_ok_);

    MSS_END();
}

void Runner::setup_chai_functions_()
{
    D & d = *d_;
    auto &chai = d.chai_engine_.raw();

    chai.add(chaiscript::fun(&Runner::chai_include, this), "include");

    chai.add(flags_module());
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
