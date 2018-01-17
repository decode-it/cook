#ifndef HEADER_cook_view_RunnerInfo_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_RunnerInfo_hpp_ALREADY_INCLUDED

#include "cook/view/Logger.hpp"
#include "cook/presenter/Interface.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/stream.hpp"
#include <vector>

namespace cook { namespace view {

    template <typename Functor>
    struct LogObject
    {
        LogObject(std::ostream & str, unsigned int indent, Functor && oss_functor)
            : str_(str), indent_(indent), functor_(oss_functor)
        {
            str_ << std::string(indent_, ' ') << ">> ";
            functor_(str_);
            str_ << std::endl;
        }

        ~LogObject()
        {
            str_ << std::string(indent_, ' ') << "<< ";
            functor_(str_);
            str_ << std::endl;
        }

    private:
        std::ostream & str_;
        unsigned int indent_;
        Functor functor_;
    };

    struct RunnerInfo
    {
        using ScriptStack = std::vector<std::filesystem::path>;
        ScriptStack script_stack;
        presenter::Interface * presenter;
        Logger &logger;

        RunnerInfo(presenter::Interface * presenter, Logger &logger): presenter(presenter), logger(logger) {}
        virtual ~RunnerInfo() {}

        template <typename OSSFunctor>
        LogObject<OSSFunctor> log_object(LogType type, OSSFunctor && functor)
        {
            return LogObject<OSSFunctor>(logger.log(type), script_stack.size()*2, std::forward<OSSFunctor>(functor));
        }

        std::string indent() const {return std::string(script_stack.size()*2, ' ');}
        std::filesystem::path current_script() const { return script_stack.back(); }
        std::filesystem::path working_directory() const
        {
            auto fn = current_script().parent_path();
            if (fn.is_relative())
                fn = std::filesystem::current_path() / fn;
            return fn;
        }

        virtual void notify_error(const std::string & error_msg) = 0;
        virtual void notify_warning(const std::string & warning_msg) = 0;
    };

} }

#endif
