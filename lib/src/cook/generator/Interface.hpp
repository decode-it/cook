#ifndef HEADER_cook_generator_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_Interface_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"
#include "cook/Model.hpp"

namespace cook { namespace generator {

class Processor
{
public:
    Processor(const Logger & logger)
        : logger_(logger) {}
    virtual ~Processor() {}

    virtual bool visit(cook::Model & api, const std::vector<std::string> & recipes) = 0;
    virtual bool process() = 0;

protected:
    Logger & logger() { return logger_; }

private:
    Logger logger_;
};

class Factory
{
public:
    virtual ~Factory();
    virtual std::shared_ptr<Processor> construct(const Logger & logger) const = 0;
};

inline Factory::~Factory() {}


} }

#endif
