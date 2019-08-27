#ifndef HEADER_cook_chai_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Recipe_hpp_ALREADY_INCLUDED

#include "cook/chai/UserData.hpp"
#include "cook/chai/UserData.hpp"
#include "cook/chai/Flags.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/Logger.hpp"
#include <functional>

namespace cook { namespace chai {

class Context;
class File;
class KeyValue;

class Recipe
{
public:
    using GlobFunctor = std::function<bool (File &)>;
    using DepFileFilter = model::Recipe::DependencyFileFilter;
    using DepKeyValueFilter = model::Recipe::DependencyKeyValueFilter;
    using ConfigCallback1 = std::function<void ()>;
    using ConfigCallback2 = std::function<void (Recipe)>;

    Recipe(model::Recipe * recipe, const Context *context);

    void add(const std::string & dir, const std::string & pattern, const Flags & flags = Flags(), const GlobFunctor & functor = GlobFunctor());
    void remove(const std::string & dir, const std::string & pattern, const Flags & flags = Flags(), const GlobFunctor & functor = GlobFunctor());
    void depends_on(const model::Uri &dependency, const DepFileFilter & file_filter = DepFileFilter(), const DepKeyValueFilter & key_value_filter = DepKeyValueFilter());
    void set_type(TargetType type);
    TargetType type() const;
    void set_working_directory(const std::string & dir);
    std::string working_directory() const;

    void library(const std::string & library, const Flags & flags = Flags());
    void framework(const std::string & framework, const Flags & flags = Flags());
    void library_path(const std::string & path, const Flags & flags= Flags());
    void framework_path(const std::string & path, const Flags & flags= Flags());
    void include_path(const std::string & path, const Flags & flags= Flags());
    void define(const std::string & name, const Flags & flags= Flags());
    void define(const std::string & name, const std::string & value, const Flags & flags = Flags());
    void run(const std::string & command);
    const model::Uri & uri() const;

    bool add_file(const std::string & dir, const std::string & rel, const Flags & flags = Flags());
    bool add_key_value(const std::string & key, const Flags & flags = Flags());
    bool add_key_value(const std::string & key, const std::string & value, const Flags & flags = Flags());
    void each_file(const std::function<void (File &)> & functor);
    void each_key_value(const std::function<void (KeyValue &)> & functor);

    bool operator==(const Recipe & rhs) const;
    bool operator!=(const Recipe & rhs) const;

    void set_callback1(Hook hook, const ConfigCallback1 & callback);
    void set_callback2(Hook hook, const ConfigCallback2 & callback);

    std::string & build_target_name();
    std::string build_target_filename() const;

    UserData data() const { return data_; }
    const Context * context() { return context_; }

private:
    model::Recipe * recipe_;
    const Context * context_;
    UserData data_;
};

} }

#endif
