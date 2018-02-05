#include "cook/chai/RunnerInfo.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace view { namespace chai {

    void RunnerInfo::notify_error(const std::string & error_msg)
    {
        throw chaiscript::exception::eval_error(error_msg);
    }

    void RunnerInfo::notify_warning(const std::string & warning_msg)
    {
        std::cout << warning_msg << std::endl;
    }

} } }
