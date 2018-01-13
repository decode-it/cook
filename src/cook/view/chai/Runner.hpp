#ifndef HEADER_cook_view_chai_Runner_hpp_ALREADY_INCLUDED
#define HEADER_cook_view_chai_Runner_hpp_ALREADY_INCLUDED

#include "cook/view/chai/RunnerInfo.hpp"
#include "cook/view/Book.hpp"
#include "cook/view/Recipe.hpp"
#include "cook/view/chai/Engine.hpp"
#include "cook/view/Logger.hpp"
#include "cook/presenter/Interface.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/mss.hpp"
#include <vector>
#include <functional>

namespace cook { namespace view { namespace chai { 

    class Runner
    {
    public:
        Runner(presenter::Reference presenter, Logger &logger): presenter_(presenter), logger_(logger)
        {
            setup_chai_functions_();
        }
        bool execute(const std::string &file_or_dir);

        //Functions called from chaiscript
        void chai_include(const std::string &file_or_dir) {execute(file_or_dir);}

    private:
        void setup_chai_functions_();
        std::filesystem::path expand_(const std::string &file_or_dir);

        presenter::Reference presenter_;
        Logger &logger_;
        RunnerInfo runner_info_{presenter_, logger_};
        Book root_book_{runner_info_};

        cook::view::chai::Engine chai_engine_;

        bool execute_ok_ = true;
    };

} } } 

#endif
