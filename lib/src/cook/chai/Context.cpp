#include "cook/chai/Context.hpp"

#include "cook/chai/module/Basic.hpp"
#include "cook/chai/module/Flags.hpp"
#include "cook/chai/module/Uri.hpp"
#include "cook/chai/module/Ingredient.hpp"
#include "cook/chai/module/ToolchainElement.hpp"
#include "cook/chai/module/Toolchain.hpp"
#include "cook/chai/module/Book.hpp"
#include "cook/chai/module/Recipe.hpp"
#include "cook/chai/module/Cook.hpp"
#include "cook/chai/Cook.hpp"

#include "cook/chai/Toolchain.hpp"
#include "cook/chai/Book.hpp"
#include "cook/chai/File.hpp"
#include "cook/chai/KeyValue.hpp"
#include "cook/chai/mss.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/process/toolchain/Element.hpp"
#include "gubg/std/filesystem.hpp"
#include "gubg/mss.hpp"
#include "gubg/chai/inject.hpp"
#include "chaiscript/chaiscript.hpp"
#include <stack>
#include <functional>

namespace cook { namespace chai {

namespace {

struct Error : public chaiscript::exception::eval_error
{
    explicit Error(const Result & result)
        : chaiscript::exception::eval_error(""),
          result(result)
    {
    }

    Result result;
};

struct Logger : public cook::Logger
{
    void log(const Result & result) const override
    {
        if (result.test_flag(Message::Error) || result.test_flag(Message::InternalError))
            throw Error(result);
    }
};

}


struct W_OS {};

struct Context::Pimpl
{
    using Parser = chaiscript::parser::ChaiScript_Parser<chaiscript::eval::Noop_Tracer, chaiscript::optimizer::Optimizer_Default>;
    using Engine = chaiscript::ChaiScript_Basic;

    Pimpl(model::Book * book, Context * context)
        : engine(chaiscript::Std_Lib::library(), std::make_unique<Parser>()),
          cook(book, context)
    {
        set_logger(&logger);
        gubg::chai::inject<gubg::chai::Regex>(engine);
        gubg::chai::inject<std::string>(engine);
        gubg::chai::inject<gubg::chai::File>(engine);
        gubg::chai::inject<gubg::chai::Date>(engine);
        gubg::chai::inject<gubg::chai::Time>(engine);
    }

    Logger logger;
    Engine engine;
    std::stack<std::filesystem::path> scripts;
    Cook cook;
    using ElementType = process::toolchain::Element::Type; 

    void initialize_engine_(Context * kitchen)
    {
        engine.add(chaiscript::fun([=](const std::string & name) { kitchen->load_(name); }), "load");
        engine.add(chaiscript::fun([=](const std::string & name) { kitchen->include_(name); }), "include");
        engine.add(chaiscript::fun([=](const std::string & name) { kitchen->include_relative_(name); }), "include_relative");
        engine.add(module::flags());
        engine.add(module::basic());
        engine.add(module::uri());
        engine.add(module::toolchain_element());
        engine.add(module::toolchain());
        engine.add(module::ingredient());
        engine.add(module::book());
        engine.add(module::recipe());
        engine.add(module::cook());

        add_ingredient_constructors(kitchen);
        engine.add(user_data_module());
        engine.add_global(chaiscript::var(cook.root()), "root");
        engine.add_global(chaiscript::var(std::ref(cook)), "cook");
    }

    void add_ingredient_constructors(const Context * context)
    {
        {
            auto lambda = [=](const std::string & dir, const std::string & rel)
            {
                return File(LanguageTypePair(Language::Undefined, Type::Undefined), ingredient::File(dir, rel), context);
            };
            engine.add(chaiscript::fun(lambda), "File");
            engine.add(chaiscript::fun(lambda), "make_file");
        }

        {
            auto lambda = [=](const std::string & key)
            {
                return KeyValue(LanguageTypePair(Language::Undefined, Type::Undefined), ingredient::KeyValue(key), context);
            };
            engine.add(chaiscript::fun(lambda), "KeyValue");
            engine.add(chaiscript::fun(lambda), "make_key_value");
        }

        {
            auto lambda = [=](const std::string & key, const std::string & value)
            {
                return KeyValue(LanguageTypePair(Language::Undefined, Type::Undefined), ingredient::KeyValue(key, value), context);
            };
            engine.add(chaiscript::fun(lambda), "KeyValue");
            engine.add(chaiscript::fun(lambda), "make_key_value");
        }
    }


    std::filesystem::path top_level_path() const
    {
        if (scripts.empty())
            return std::filesystem::current_path();
        else
            return scripts.top().parent_path();
    }
};

const cook::Logger & Context::logger() const
{
    return pimpl_->logger;
}

Context::Context()
    : pimpl_(std::make_unique<Pimpl>(root_book(), this))
{
    pimpl_->initialize_engine_(this);
}


Context::~Context() = default;

Result Context::set_variable(const std::string & name, const std::string & value)
{
    MSS_BEGIN(Result);

    MSG_MSS(data_.set_variable(name, value), Warning, "Could not set variable '" << name << "' to '" << value << "': Variable already exists");

    MSS_END();
}

namespace  {

template <typename Error, typename StackIt, typename Functor>
Result recursive_process_(Error & error, StackIt it, Functor && f)
{
    MSS_BEGIN(Result);

    if (it < error.call_stack.begin() || error.call_stack.empty())
    {
        MSS(f(error));
    }
    else
    {
        const chaiscript::AST_Node_Trace & t =  *it;

        auto s = log::scope("chai script", [&](auto & n) {
            n.attr("type", chaiscript::ast_node_type_to_string(t.identifier)).attr("filename", t.filename()).attr("start_line", t.start().line).attr("start_col", t.start().column).attr("end_line", t.end().line).attr("end_col", t.end().column);
        });

        MSS(recursive_process_(error, --it, std::forward<Functor>(f)));
    }

    MSS_END();
}

Result process_chai_error_(chaiscript::exception::eval_error & error)
{
    MSS_BEGIN(Result);

    MSG_MSS(false, Error, error.reason << std::endl << error.detail);

    MSS_END();
}

Result process_Error(Error & error)
{
    return error.result;
}


}

Result Context::load_toolchain(const std::string & toolchain)
{
    MSS_BEGIN(Result);

    process::toolchain::Loader loader;
    for(const auto & d : dirs().include_dirs())
        loader.append_include_path(d);

    std::filesystem::path fn;
    MSS(loader.load(toolchain, fn));

    MSS(run_(fn.string()));
    MSS_END();
}

Result Context::load_recipe(const std::string & recipe)
{
    MSS_BEGIN(Result);
    MSS(run_(recipe));
    MSS_END();
}


Result Context::run_(const std::string & recipe)
{
    MSS_BEGIN(Result);

    try
    {
        // create and initialize the engine
        include_(recipe);
    }
    // TODO: add better error handling
    catch(Error & error)
    {
        auto it = error.call_stack.empty() ? error.call_stack.begin() : error.call_stack.end();
        MSS(recursive_process_(error, it - 1, &process_Error));
    }
    catch(chaiscript::exception::eval_error & error)
    {
        auto it = error.call_stack.empty() ? error.call_stack.begin() : error.call_stack.end();
        MSS(recursive_process_(error, it - 1, &process_chai_error_));
    }
    catch(std::runtime_error & error)
    {
        std::cout << "Error: caught std::runtime_error: " << error.what() << std::endl;
        std::cout << " in \"" << pimpl_->scripts.top().string() << "\"" << std::endl;
        MSG_MSS(false, InternalError, error.what());
    }
    catch(chaiscript::Boxed_Value & error)
    {
        std::cout << "Error: caught chaiscript::Boxed_Value: ";
        auto ptr = chaiscript::boxed_cast<std::shared_ptr<std::string>>(error);
        if (ptr)
            std::cout << *ptr;
        std::cout << " in \"" << pimpl_->scripts.top().string() << "\"" << std::endl;
        MSG_MSS(false, InternalError, "Boxed_Value");
    }

    MSS_END();
}

std::filesystem::path Context::current_working_directory() const
{
    return pimpl_->top_level_path();
}

void Context::load_(const std::string & file)
{
    // make the path to the file
    std::filesystem::path script_fn = gubg::filesystem::combine(pimpl_->top_level_path(), file);

    if (script_fn.empty())
        script_fn = "recipes.chai";
    else if (std::filesystem::is_directory(script_fn))
        script_fn /= "recipes.chai";

    load_script_(script_fn);

}

void Context::load_script_(const std::filesystem::path & fn)
{
    auto ss = log::scope("chai script", [&](auto & n) {
        n.attr("filename", fn.string());
    });

    // push the script
    pimpl_->scripts.push(fn);
    pimpl_->engine.eval_file(fn.string());
    pimpl_->scripts.pop();

}

bool Context::try_include(std::filesystem::path fn)
{
    if (std::filesystem::is_directory(fn))
        fn /= "recipes.chai";
    
    // does the file exist?
    if (!std::filesystem::exists(fn))
    {
        fn += ".chai";
        if (!std::filesystem::exists(fn))
            return false;
    }

    std::filesystem::path p = std::filesystem::canonical(fn);
    if (imported_.insert(p).second)
        load_script_(p);

    return true;
}
    
void Context::include_(const std::string & file)
{
    // see if we can find it in the import directories
    for(const auto & d : dirs().include_dirs())
    {
        if (try_include(gubg::filesystem::combine(d, file)))
            return;
    }

    include_relative_(file);
}

void Context::include_relative_(const std::string& file)
{
    if (try_include(gubg::filesystem::combine(pimpl_->top_level_path(), file)))
        return;

    Result r;
    r << Message(Message::Type::Error, "Could not import file");
    throw Error(r);
}

} }
