#include "cook/chai/Kitchen.hpp"
#include "cook/chai/Book.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/Logger.hpp"
#include "cook/chai/Environment.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/mss.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace chai {

struct Kitchen::D
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

    void initialize_engine_(Kitchen * kitchen)
    {
        auto & chai = engine;

        chai.add(chaiscript::fun(&Kitchen::include_, kitchen), "include");
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

cook::Logger & Kitchen::logger()
{
    return d->logger;
}

Kitchen::Kitchen()
    : d(std::make_unique<D>(&root_))
{
    d->initialize_engine_(this);
}


Kitchen::~Kitchen() = default;


bool Kitchen::load(const std::list<std::string> & recipes)
{
    MSS_BEGIN(bool);

    try
    {
        // create and initialize the engine
        for(const auto & recipe: recipes)
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

std::filesystem::path Kitchen::generate_file_path_(const std::string & file) const
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

void Kitchen::include_(const std::string & file)
{
    const std::filesystem::path script_fn = generate_file_path_(file);

    // push the script
    d->scripts.push(script_fn);
    d->engine.eval_file(script_fn.string());
    d->scripts.pop();
}

std::shared_ptr<model::Environment> Kitchen::create_environment() const
{
    // create a new one, based on nothing
    return std::make_shared<chai::Environment>();
}

} }
