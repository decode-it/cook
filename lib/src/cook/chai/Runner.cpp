#include "cook/chai/Runner.hpp"
#include "cook/chai/Book.hpp"
#include "cook/chai/Recipe.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/mss.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace chai {

struct Runner::D
{
    using Parser = chaiscript::parser::ChaiScript_Parser<chaiscript::eval::Noop_Tracer, chaiscript::optimizer::Optimizer_Default>;
    using Engine = chaiscript::ChaiScript_Basic;

    D(model::Book * book, Runner * runner)
        : engine(chaiscript::Std_Lib::library(), std::make_unique<Parser>()),
          root_book(book, runner)
    {
    }

    Engine engine;
    std::stack<std::filesystem::path> scripts;
    Book root_book;

    std::filesystem::path top_level_path() const
    {
        if (scripts.empty())
            return std::filesystem::path();
        else
            return scripts.top().parent_path();
    }
};

Runner::Runner()
{
}

Runner::~Runner() = default;

void Runner::report_error(const std::string & message)
{
    throw chaiscript::exception::eval_error(message);
}

void Runner::reset_engine_()
{
    d = std::make_unique<D>(&root_, this);

    {
        auto & chai = d->engine;

        chai.add(chaiscript::fun(&Runner::include_, this), "include");
        chai.add(book_module());
        chai.add(recipe_module());
        chai.add_global(chaiscript::var(d->root_book), "root");
    }
}


bool Runner::load(const std::list<std::string> & recipes)
{
    MSS_BEGIN(bool);

    reset_engine_();

    try
    {
        // create and initialize the engine
        for(const auto & recipe: recipes)
            include_(recipe);
    }
    catch(std::runtime_error & error)
    {
        std::cout << "caught error: " << error.what() << std::endl;
        MSS(false);
    }

    MSS_END();
}

std::filesystem::path Runner::generate_file_path_(const std::string & file) const
{
    // make the path to the file
    std::filesystem::path script_fn(file);
    if (script_fn.is_relative())
        script_fn = d->top_level_path() / script_fn;

    if (script_fn.empty())
        script_fn = "recipes.chai";
    if (std::filesystem::is_directory(script_fn))
        script_fn /= "recipes.chai";

    return script_fn;
}

void Runner::include_(const std::string & file)
{
    const std::filesystem::path script_fn = generate_file_path_(file);

    // push the script
    d->scripts.push(script_fn);
    d->engine.eval_file(script_fn.string());
    d->scripts.pop();
}

} }
