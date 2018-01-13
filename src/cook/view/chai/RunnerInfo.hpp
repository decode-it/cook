#ifndef HEADER_cook_view_chai_RunnerInfo_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_chai_RunnerInfo_hpp_ALREADY_INCLUDED

#include "cook/view/Logger.hpp"
#include "cook/presenter/Interface.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/std/filesystem.hpp"
#include <vector>
#include <chaiscript/chaiscript.hpp>

namespace cook { namespace view { namespace chai { 

    struct RunnerInfo
    {
        using ScriptStack = std::vector<std::filesystem::path>;
        ScriptStack script_stack;
        presenter::Reference presenter;
        Logger &logger;

        RunnerInfo(presenter::Reference presenter, Logger &logger): presenter(presenter), logger(logger) {}

        std::string indent() const {return std::string(script_stack.size()*2, ' ');}
        std::filesystem::path current_script() const { return script_stack.back(); }
        std::filesystem::path working_directory() const
        {
            auto fn = current_script().parent_path();
            if (fn.is_relative())
                fn = std::filesystem::current_path() / fn;
            return fn;
        }
        void notify_error(const std::string & error_msg)
        {
            throw chaiscript::exception::eval_error(error_msg);
        }

        void notify_warning(const std::string & warning_msg)
        {
            std::cout << warning_msg << std::endl;
        }
    };

} } } 

#endif
