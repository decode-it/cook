#ifndef HEADER_cook_Presenter_hpp_ALREADY_INCLUDED
#define HEADER_cook_Presenter_hpp_ALREADY_INCLUDED

#include "cook/Model.hpp"
#include "cook/View.hpp"

namespace cook { 

    class Presenter
    {
    public:
        Presenter(Model &model, View &view): model_(model), view_(view)
        {
            view_.log() << "Cook is starting" << std::endl;
            view_.send = [&](const std::string &key, const std::any &value){receive_(key, value);};
        }
        ~Presenter()
        {
            view_.log() << "Cook is stopping" << std::endl;
        }

    private:
        void receive_(const std::string &key, const std::any &value);

        Model &model_;
        View &view_;
    };

} 

#endif
