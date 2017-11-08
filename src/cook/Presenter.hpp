#ifndef HEADER_cook_Presenter_hpp_ALREADY_INCLUDED
#define HEADER_cook_Presenter_hpp_ALREADY_INCLUDED

#include "cook/presenter/Interface.hpp"
#include "cook/Model.hpp"
#include "cook/View.hpp"

namespace cook { 

    class Presenter: public presenter::Interface
    {
    public:
        Presenter(Model &model, View &view): model_(model), view_(view)
        {
            view_.log() << "Cook is starting" << std::endl;
            view_.inject(static_cast<presenter::Interface*>(this));
        }
        ~Presenter()
        {
            view_.log() << "Cook is stopping" << std::endl;
        }

    protected:
        bool set(const std::string &key, const std::string &value) override;
        bool set(const std::string &key, const Strings &value) override;
        bool set(const std::string &key) override;

    private:
        bool receive_(const std::string &key, const std::string &value);
        bool receive_(const std::string &key, const Strings &value);
        bool receive_(const std::string &key);

        Model &model_;
        View &view_;
    };

} 

#endif
