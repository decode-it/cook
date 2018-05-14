#include "cook/chai/Context.hpp"

#include "cook/chai/module/Basic.hpp"
#include "cook/chai/module/Flags.hpp"
#include "cook/chai/module/Uri.hpp"
#include "cook/chai/module/Ingredient.hpp"
#include "cook/chai/module/ToolchainElement.hpp"
#include "cook/chai/module/Toolchain.hpp"
#include "cook/chai/module/Book.hpp"
#include "cook/chai/module/Recipe.hpp"

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

struct Cook
{
    Cook(model::Book * book, Context * context, Logger * logger)
        : root(book, context),
          context_(context),
          toolchain_(&context->toolchain())
    {
    }

    Book operator[](const std::string & uri)
    {
        return root.book(uri);
    }


    std::string working_directory() const
    {
        return context_->current_working_directory().string();
    }

    std::string working_directory(bool absolute) const
    {
        if (absolute)
            return gubg::filesystem::combine(std::filesystem::current_path(), context_->current_working_directory()).string();
        else
            return working_directory();
    }

    std::string project_name() const
    {
        return context_->project_name();
    }
    void set_project_name(const std::string & name)
    {
        context_->set_project_name(name);
    }
 
    Toolchain toolchain() const
    {
        return toolchain_;
    }


    Book root;
    Context * context_;
    Toolchain toolchain_;
};

struct W_OS {};

struct Context::Pimpl
{
    using Parser = chaiscript::parser::ChaiScript_Parser<chaiscript::eval::Noop_Tracer, chaiscript::optimizer::Optimizer_Default>;
    using Engine = chaiscript::ChaiScript_Basic;

    Pimpl(model::Book * book, Context * context)
        : engine(chaiscript::Std_Lib::library(), std::make_unique<Parser>()),
          cook(book, context, &logger)
    {
        set_logger(&logger);
        gubg::chai::inject<gubg::chai::Regex>(engine);
        gubg::chai::inject<std::string>(engine);
        gubg::chai::inject<gubg::chai::File>(engine);
        gubg::chai::inject<gubg::chai::Date>(engine);
    }

    Logger logger;
    Engine engine;
    std::stack<std::filesystem::path> scripts;
    Cook cook;
    using ElementType = process::toolchain::Element::Type; 

    void initialize_engine_(Context * kitchen)
    {
        engine.add(chaiscript::fun(&Context::include_, kitchen), "include");

        engine.add(module::flags());
        engine.add(module::basic());
        engine.add(module::uri());
        engine.add(module::toolchain_element());
        engine.add(module::toolchain());
        engine.add(module::ingredient());
        engine.add(module::book());
        engine.add(module::recipe());
        

        initialize_flags();
        add_ingredient_constructors(kitchen);
        engine.add(user_data_module());
        engine.add_global(chaiscript::var(std::ref(cook.root)), "root");
        engine.add_global(chaiscript::var(std::ref(cook)), "cook");
        engine.add(chaiscript::fun(&Cook::operator []), "[]");
        engine.add(chaiscript::fun([](const Cook & c) { return c.working_directory(); }), "working_directory");
        engine.add(chaiscript::fun([](const Cook & c, bool absolute) { return c.working_directory(absolute); }), "working_directory");
        engine.add(chaiscript::fun(&Cook::project_name), "project_name");
        engine.add(chaiscript::fun(&Cook::set_project_name), "set_project_name");
        engine.add(chaiscript::fun(&Cook::toolchain), "toolchain");
    }

    void initialize_flags()
    {
#define EXPOSE_TOP(TYPE) engine.add_global(chaiscript::var(W_##TYPE()), #TYPE)
        EXPOSE_TOP(OS);
#undef EXPOSE_TOP



#define EXPOSE(TYPE, NAME) engine.add(chaiscript::fun([](const W_##TYPE & ) { return TYPE::NAME; }), #NAME)
        EXPOSE(OS, Windows);
        EXPOSE(OS, Linux);
        EXPOSE(OS, MacOS);
#undef EXPOSE

#define EXPOSE_ENUM_FUNC(NAME) engine.add(chaiscript::user_type<NAME>(), #NAME); engine.add(chaiscript::fun([](NAME lhs, NAME rhs){return lhs == rhs;}), "=="); engine.add(chaiscript::fun([](NAME lhs, NAME rhs){return lhs != rhs;}), "!=")

        EXPOSE_ENUM_FUNC(OS);

        engine.add(chaiscript::fun([](const W_OS & ) { return get_os(); }), "my");

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
            return std::filesystem::path();
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

    std::filesystem::path fn;
    MSS(loader_.load(toolchain, fn));

    MSS(load_(fn.string()));
    MSS_END();
}

Result Context::load_recipe(const std::string & recipe)
{
    MSS_BEGIN(Result);
    MSS(load_(recipe));
    MSS_END();
}


Result Context::load_(const std::string & recipe)
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

std::filesystem::path Context::generate_file_path_(const std::string & file) const
{
    // make the path to the file
    std::filesystem::path script_fn(file);

    if (script_fn.is_relative())
        script_fn = pimpl_->top_level_path() / script_fn;

    if (script_fn.empty())
        script_fn = "recipes.chai";
    else if (std::filesystem::is_directory(script_fn))
        script_fn /= "recipes.chai";

    return script_fn;
}

std::filesystem::path Context::current_working_directory() const
{
    return pimpl_->top_level_path();
}

void Context::include_(const std::string & file)
{
    const std::filesystem::path script_fn = generate_file_path_(file);

    auto ss = log::scope("chai script", [&](auto & n) {
        n.attr("filename", script_fn.string());
    });

    // push the script
    pimpl_->scripts.push(script_fn);
    pimpl_->engine.eval_file(script_fn.string());
    pimpl_->scripts.pop();
}

} }
