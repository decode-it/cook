#include "cook/chai/Logger.hpp"
#include "chaiscript/chaiscript.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace chai {

void Logger::log(LogType type, const Logger::LogFunction & function)
{
    switch(type)
    {
    case LogType::Error:
        throw chaiscript::exception::eval_error(gubg::stream(function));

    default:
        std::cout << type << ": " << gubg::stream(function) << std::endl;
    }
}

} }

