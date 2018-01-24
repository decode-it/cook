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

        bool process(const std::list<std::string> & input_files);

        //Functions called from chaiscript
        void chai_include(const std::string &file_or_dir) { execute_(file_or_dir); }

    private:
        bool execute_(const std::string &file_or_dir);
        void setup_chai_functions_();
        std::filesystem::path expand_(const std::string &file_or_dir);

        std::unique_ptr<D> d_;
    };

} } } 

#endif
