#ifndef HEADER_cook_API_ALREADY_INCLUDED
#define HEADER_cook_API_ALREADY_INCLUDED

#include "cook/generator/Manager.hpp"
#include "cook/Model.hpp"
#include "cook/Logger.hpp"
#include "cook/API.h"

#include <sstream>

namespace cook {

class API : public Logger, public Model
{
public:
    explicit API(const cook_API_Parameters_t & parameters);
    cook::generator::Manager mgr;

    std::filesystem::path resolve_local(const std::string & fod) const;

private:
    std::filesystem::path project_dir_;
};

}

#endif
