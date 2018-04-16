#include "cook/chai/Context.hpp"
#include "cook/chai/Book.hpp"
#include "cook/chai/Recipe.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/mss.hpp"
#include "chaiscript/chaiscript.hpp"
#include <stack>

namespace cook { namespace chai {

namespace {

struct Error : public chaiscript::exception::eval_error
{
    explicit Error(const Result & result)
        : chaiscript::exception::eval_error(""),
        result(result)
    {
    }

    Result result;
};

struct Logger : public cook::Logger
{
    void log(const Result & result) const override
    {
        if (result.test_flag(Message::Error) || result.test_flag(Message::InternalError))
            throw Error(result);
    }
};

}

struct Cook
{
    Cook(model::Book * book, Context * context, Logger * logger)
        : root(book, context, logger)
    {

    }

    Book operator[](const std::string & uri)
    {
        return root.subbook(uri);
    }

    Book root;
};

struct Context::D
{
    using Parser = chaiscript::parser::ChaiScript_Parser<chaiscript::eval::Noop_Tracer, chaiscript::optimizer::Optimizer_Default>;
    using Engine = chaiscript::ChaiScript_Basic;

    D(model::Book * book, Context * context)
        : engine(chaiscript::Std_Lib::library(), std::make_unique<Parser>()),
          cook(book, context, &logger)
    {
    }

    Logger logger;
    Engine engine;
    std::stack<std::filesystem::path> scripts;
    Cook cook;

    void initialize_engine_(Context * kitchen)
    {
        auto & chai = engine;

        chai.add(chaiscript::fun(&Context::include_, kitchen), "include");
        initialize_book();
        initialize_recipe();
        chai.add(user_data_module());
        chai.add_global(chaiscript::var(cook.root), "root");
        chai.add_global(chaiscript::var(cook), "cook");
        chai.add(chaiscript::fun(&Cook::operator []), "[]");
    }

    void initialize_book()
    {
        engine.add(chaiscript::user_type<Book>(), "Book");
        engine.add(chaiscript::fun(&Book::book), "book");
        engine.add(chaiscript::fun(&Book::recipe_2), "recipe");
        engine.add(chaiscript::fun(&Book::recipe_3), "recipe");
        engine.add(chaiscript::fun(&Book::data), "data");

    }

    void initialize_recipe()
    {
        engine.add(chaiscript::user_type<Recipe>(), "Recipe");
        engine.add(chaiscript::fun(&Recipe::add), "add");
        engine.add(chaiscript::fun(&Recipe::depends_on), "depends_on");
        engine.add(chaiscript::fun(&Recipe::set_type), "set_type");
        engine.add(chaiscript::fun(&Recipe::library), "library");
        engine.add(chaiscript::fun(&Recipe::library_path), "library_path");
        engine.add(chaiscript::fun(&Recipe::include_path), "include_path");
        engine.add(chaiscript::fun(&Recipe::define_1), "define");
        engine.add(chaiscript::fun(&Recipe::define_2), "define");
        engine.add(chaiscript::fun(&Recipe::data), "data");
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
    : d(std::make_unique<D>(root_book(), this))
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

Result Context::load(const std::string & recipe)
{
    MSS_BEGIN(Result);

    try
    {
        // create and initialize the engine
        include_(recipe);
    }
    // TODO: add better error handling
    catch(Error & error)
    {
        std::cout << error.pretty_print();
        MSS(error.result);
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

std::filesystem::path Context::current_working_directory() const
{
    return d->top_level_path();
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
