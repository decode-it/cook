#ifndef HEADER_cook_chai_RunnerInfo_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_RunnerInfo_hpp_ALREADY_INCLUDED

#include "cook/view/RunnerInfo.hpp"

namespace cook { namespace view { namespace chai { 

    struct RunnerInfo : public cook::view::RunnerInfo
    {
        RunnerInfo(Logger &logger) : cook::view::RunnerInfo(logger) {}

        void notify_error(const std::string & error_msg) override;
        void notify_warning(const std::string & warning_msg) override;
    };

} } } 

#endif
