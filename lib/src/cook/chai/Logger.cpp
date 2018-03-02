#include "cook/chai/Logger.hpp"
#include "chaiscript/chaiscript.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace chai {

void Logger::log(LogType type, const Logger::LogFunction & function) const
{
    switch(type)
    {
    case LogType::Error:
        throw chaiscript::exception::eval_error(gubg::stream(function));

    default:
        std::cout << type << ": " << gubg::stream(function) << std::endl;
    }
}

void Logger::log(const Result & result) const
{
    result.each_message([](const auto & type, const auto & reporter)
    {
        std::cout << type << ": ";
        if (reporter)
            reporter(std::cout);
        std::cout << std::endl;
    });
}

} }

