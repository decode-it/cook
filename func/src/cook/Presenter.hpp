#ifndef HEADER_cook_Presenter_hpp_ALREADY_INCLUDED
#define HEADER_cook_Presenter_hpp_ALREADY_INCLUDED

#include "cook/presenter/Command.hpp"
#include "cook/any.hpp"
#include "cook/Model.hpp"
#include "cook/view/Interface.hpp"

namespace cook {

    class Presenter
    {
    public:
        using Keys = std::vector<presenter::Command>;
        using Values = std::vector<std::any>;

        Presenter(Model &model, view::Interface &view)
            : model_(model),
              view_(view)
        {}

        bool set(const Keys &key, const Values & values = Values());

    private:
        Model &model_;
        view::Interface &view_;
        std::string script_fn_;
    };
}

#endif
