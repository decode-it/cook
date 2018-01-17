#ifndef HEADER_cook_Presenter_hpp_ALREADY_INCLUDED
#define HEADER_cook_Presenter_hpp_ALREADY_INCLUDED

#include "cook/presenter/Interface.hpp"
#include "cook/Model.hpp"
#include "cook/View.hpp"

namespace cook { 

    class Presenter: public presenter::Interface
    {
    public:
        Presenter(Model &model, View &view)
            : model_(model), view_(view)
        {
            view_.inject(static_cast<presenter::Interface*>(this));
        }

        ~Presenter()
        {
        }

    protected:
        bool set(const std::string &key, const std::string &value) override;
        bool set(const std::string &key, const Strings &value) override;
        bool set(const std::string &key) override;
        bool get(const std::string &key, std::string & result) const override;
        bool get(const std::string &key, const Strings & args, std::string & result) const override;

    private:
        Model &model_;
        View &view_;
        std::string script_fn_;
    };

} 

#endif
