#include "cook/Result.hpp"
#include "cook/chai/Logger.hpp"
#include "chaiscript/chaiscript.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace chai {

void Logger::log(const Result & result) const
{
    result.each_message([](const Message & msg)
    {
        std::cout << msg.type_ << ": " << msg.msg_ << std::endl;
    });
}

} }

