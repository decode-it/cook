#include "cook/chai/Engine.hpp"
#include "gubg/mss.hpp"
#include "chaiscript/chaiscript_stdlib.hpp"
#include <memory>
#include <iostream>

namespace cook { namespace chai { 

    using Parser = chaiscript::parser::ChaiScript_Parser<chaiscript::eval::Noop_Tracer, chaiscript::optimizer::Optimizer_Default>;

    Engine::Engine(): engine_(chaiscript::Std_Lib::library(), std::make_unique<Parser>())
    {
    }

    ReturnCode Engine::eval_file(const std::filesystem::path &fn)
    {
        MSS_BEGIN(ReturnCode);
        MSS(std::filesystem::is_regular_file(fn), std::cerr << "Cannot eval " << fn << ", it does not exist" << std::endl);
        engine_.eval_file(fn);
        MSS_END();
    }

} } 
