#ifndef HEADER_cook_Kitchen_hpp_ALREADY_INCLUDED
#define HEADER_cook_Kitchen_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"
#include "cook/model/Book.hpp"
#include "cook/toolchain/Interface.hpp"

namespace cook {

class Kitchen
{
public:
    using ToolChainPtr = std::shared_ptr<toolchain::Interface>;

    Kitchen(Logger & logger)
        : logger_(logger)
    {
    }

    bool register_toolchain(ToolChainPtr toolchain);

private:
    Logger & logger_;
    std::map<std::string, ToolChainPtr> toolchains_;
};

}

#endif
