#include "cook/chai/Runner.hpp"
#include "gubg/std/filesystem.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace chai {

struct Runner::D
{
    using Parser = chaiscript::parser::ChaiScript_Parser<chaiscript::eval::Noop_Tracer, chaiscript::optimizer::Optimizer_Default>;
    using Engine = chaiscript::ChaiScript_Basic;

    D()
        : engine(chaiscript::Std_Lib::library(), std::make_unique<Parser>())
    {
    }

    Engine engine;
    std::stack<std::filesystem::path> scripts;
};

Runner::Runner()
    : d(std::make_unique<D>())
{

}
Runner::~Runner() = default;

void Runner::report_error(const std::string & message)
{
    throw chaiscript::exception::eval_error(message);

}

void Runner::load(const std::list<std::string> & recipes)
{
    // create and initialize the engine
    for(const auto & recipe: recipes)
        include_(recipe);
}

void Runner::include_(const std::string & file)
{

}

} }


