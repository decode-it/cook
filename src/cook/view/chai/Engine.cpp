#include "cook/view/chai/Engine.hpp"
#include "chaiscript/chaiscript_stdlib.hpp"
#include <memory>

namespace cook { namespace view { namespace chai { 

    using Parser = chaiscript::parser::ChaiScript_Parser<chaiscript::eval::Noop_Tracer, chaiscript::optimizer::Optimizer_Default>;

    Engine::Engine(): engine_(chaiscript::Std_Lib::library(), std::make_unique<Parser>())
    {
    }

    void Engine::eval_file(const std::filesystem::path &fn)
    {
        engine_.eval_file(fn);
    }

} } } 
