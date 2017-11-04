#include "cook/View.hpp"
#include "cook/view/Options.hpp"

namespace cook { 

    std::ostream &View::log(LogType lt)
    {
        auto &os = std::cout;
        switch (lt)
        {
            case Info: os << "Info: "; break;
            case Message: break;
            case Error: os << "Error: "; break;
        }
        return os;
    }

    bool View::send(const std::string &key, const std::any &value)
    {
        MSS_BEGIN(bool);
        MSS(!!send_);
        MSS(send_(key, value));
        MSS_END();
    }

    bool View::process(int argc, const char **argv)
    {
        MSS_BEGIN(bool);

        view::Options options;
        MSS(options.parse(argc, argv));

        MSS(send("help.message", options.help_message));
        MSS(send("toolchain.config", options.config));
        MSS(send("toolchain.arch", options.arch));
        if (options.print_help)
            MSS(send("action", std::string{"print_help"}));

        MSS_END();
    }

} 
