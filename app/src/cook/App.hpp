#ifndef HEADER_cook_App_hpp_ALREADY_INCLUDED
#define HEADER_cook_App_hpp_ALREADY_INCLUDED

#include "cook/app/Options.hpp"
#include "cook/chai/Context.hpp"

namespace cook {

class App
{
public:
    bool initialize(const app::Options & options);
    Result process();

private:
    Result extract_root_recipes_(std::list<model::Recipe *> & result) const;
    Result load_recipes_();
    Result process_generators_() const;
    Result process_generator_(const std::string & name, const std::string & value) const;

    cook::Indent indent_;
    app::Options options_;
    cook::chai::Context kitchen_;
};

}

#endif
