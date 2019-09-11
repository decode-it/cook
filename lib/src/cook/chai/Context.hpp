#ifndef HEADER_cook_chai_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Context_hpp_ALREADY_INCLUDED

#include "cook/chai/UserData.hpp"
#include "cook/process/toolchain/Loader.hpp"
#include "cook/Context.hpp"

namespace cook { namespace chai {

class Context : public cook::Context
{
public:
    Context();
    virtual ~Context();

    const cook::Logger & logger() const override;
    Result set_variable(const std::string & name, const std::string & value) override;

    Result load_recipe(const std::string &recipe);
    Result load_toolchain(const std::string & toolchain);
    std::filesystem::path current_working_directory() const;

private:
    Result run_(const std::string & filename);
    void load_script_(const std::filesystem::path & fn);

    bool try_include(std::filesystem::path fn);
    void load_(const std::string & file);
    void include_(const std::string & file);
    void include_relative_(const std::string & file);
    void reset_engine_();

    struct Pimpl;
    std::unique_ptr<Pimpl> pimpl_;
    UserData data_;
    std::set<std::filesystem::path> imported_;
};

} }

#endif

