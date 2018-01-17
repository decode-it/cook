#ifndef HEADER_cook_view_chai_Runner_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_chai_Runner_hpp_ALREADY_INCLUDED

#include "cook/view/Logger.hpp"
#include "cook/Presenter.hpp"
#include "gubg/std/filesystem.hpp"
#include <vector>
#include <functional>
#include <memory>

namespace cook { namespace view { namespace chai { 

    class Runner
    {
        struct D;

    public:
        Runner(Presenter * presenter, Logger &logger);

        ~Runner();

        bool execute(const std::string &file_or_dir);

        //Functions called from chaiscript
        void chai_include(const std::string &file_or_dir) {execute(file_or_dir);}

    private:
        void setup_chai_functions_();
        std::filesystem::path expand_(const std::string &file_or_dir);

        std::unique_ptr<D> d_;
    };

} } } 

#endif
