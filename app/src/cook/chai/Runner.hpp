#ifndef HEADER_cook_chai_Runner_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Runner_hpp_ALREADY_INCLUDED

#include "cook/Handle.h"
#include "cook/view/Logger.hpp"
#include "gubg/std/filesystem.hpp"
#include <vector>
#include <functional>
#include <memory>
#include <list>

namespace cook { namespace chai {

    class Runner
    {
        struct D;

    public:
        Runner(cook_Handle_t api , view::Logger &logger);
        ~Runner();

        bool process(const std::list<std::string> & input_files);

        //Functions called from chaiscript
        void chai_include(const std::string &file_or_dir) { execute_(file_or_dir); }

    private:
        bool execute_(const std::string &file_or_dir);
        void setup_chai_functions_();
        std::filesystem::path expand_(const std::string &file_or_dir);

        std::unique_ptr<D> d_;
    };

} }

#endif
