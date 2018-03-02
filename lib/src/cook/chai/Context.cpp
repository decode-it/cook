#include "cook/chai/Context.hpp"
#include "cook/chai/Book.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/Logger.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/mss.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace chai {

struct Context::D
{
    using Parser = chaiscript::parser::ChaiScript_Parser<chaiscript::eval::Noop_Tracer, chaiscript::optimizer::Optimizer_Default>;
    using Engine = chaiscript::ChaiScript_Basic;

    D(model::Book * book)
        : engine(chaiscript::Std_Lib::library(), std::make_unique<Parser>()),
          root_book(book, &logger)
    {
    }

    Logger logger;
    Engine engine;
    std::stack<std::filesystem::path> scripts;
    Book root_book;

    void initialize_engine_(Context * kitchen)
    {
        auto & chai = engine;

        chai.add(chaiscript::fun(&Context::include_, kitchen), "include");
        chai.add(book_module());
        chai.add(recipe_module());
        chai.add(user_data_module());
        chai.add_global(chaiscript::var(root_book), "root");
    }

    std::filesystem::path top_level_path() const
    {
        if (scripts.empty())
            return std::filesystem::path();
        else
            return scripts.top().parent_path();
    }
};

const cook::Logger & Context::logger() const
{
    return d->logger;
}

Context::Context()
    : d(std::make_unique<D>(root_book()))
{
    d->initialize_engine_(this);
}


Context::~Context() = default;

Result Context::set_variable(const std::string & name, const std::string & value)
{
    MSS_BEGIN(Result);

    MSG_MSS(data_.set_variable(name, value), Warning, "Could not set variable '" << name << "' to '" << value << "': Variable already exists");

    MSS_END();
}

bool Context::load(const std::string & recipe)
{
    MSS_BEGIN(bool);

    try
    {
        // create and initialize the engine
        include_(recipe);
    }
    catch(chaiscript::exception::eval_error & error)
    {
        std::cout << error.pretty_print();
        MSS(false);
    }
    catch(std::runtime_error & error)
    {
        std::cout << "caught error: " << error.what() << std::endl;
        MSS(false);
    }

    MSS_END();
}

std::filesystem::path Context::generate_file_path_(const std::string & file) const
{
    // make the path to the file
    std::filesystem::path script_fn(file);

    if (script_fn.is_relative())
        script_fn = d->top_level_path() / script_fn;

    if (script_fn.empty())
        script_fn = "recipes.chai";
    else if (std::filesystem::is_directory(script_fn))
        script_fn /= "recipes.chai";

    return script_fn;
}

void Context::include_(const std::string & file)
{
    const std::filesystem::path script_fn = generate_file_path_(file);

    // push the script
    d->scripts.push(script_fn);
    d->engine.eval_file(script_fn.string());
    d->scripts.pop();
}

} }
