#include "cook/chai/Context.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/Book.hpp"
#include "cook/chai/File.hpp"
#include "cook/chai/KeyValue.hpp"
#include "cook/chai/Flags.hpp"
#include "cook/chai/mss.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/process/toolchain/Interface.hpp"
#include "cook/process/toolchain/Compiler.hpp"
#include "cook/process/toolchain/Archiver.hpp"
#include "cook/process/toolchain/Linker.hpp"
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
          toolchain(&context->toolchain())
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


    Book root;
    Context * context_;
    process::toolchain::Manager * toolchain;
};

struct ToolchainElement
{
    ToolchainElement(process::toolchain::Interface * element)
        : element(element)
    {
    }

    using KVM = process::toolchain::KeyValuesMap;
    using TM = process::toolchain::TranslatorMap;

    KVM & key_values() { return element->key_values_map(); }
    const KVM & key_values_const() const { return element->key_values_map(); }
    TM & translators() { return element->translator_map(); }
    const TM & translators_const() const { return element->translator_map(); }
    
    process::toolchain::Interface * element;
};

struct W_Propagation { };
struct W_Overwrite { };
struct W_Type { };
struct W_Language { };
struct W_OS {};
struct W_Part {};

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

    void initialize_engine_(Context * kitchen)
    {
        engine.add(chaiscript::fun(&Context::include_, kitchen), "include");
        initialize_uri();
        initialize_book();
        initialize_recipe(kitchen);
        initialize_flags();
        initialize_file(kitchen);
        initialize_key_value(kitchen);
        initialize_toolchain();
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
        using Part = process::toolchain::Part;

#define EXPOSE_TOP(TYPE) engine.add_global(chaiscript::var(W_##TYPE()), #TYPE)
        EXPOSE_TOP(Propagation);
        EXPOSE_TOP(Overwrite);
        EXPOSE_TOP(Type);
        EXPOSE_TOP(Language);
        EXPOSE_TOP(OS);
        EXPOSE_TOP(Part);
#undef EXPOSE_TOP

#define EXPOSE(TYPE, NAME) engine.add(chaiscript::fun([](const W_##TYPE & ) { return Flags(TYPE::NAME); }), #NAME)
        EXPOSE(Propagation, Public);
        EXPOSE(Propagation, Private);
        EXPOSE(Overwrite, Always);
        EXPOSE(Overwrite, IfSame);
        EXPOSE(Overwrite, Never);
        EXPOSE(Type, Undefined);
        EXPOSE(Type, Source);
        EXPOSE(Type, Header);
        EXPOSE(Type, Object);
        EXPOSE(Type, ForceInclude);
        EXPOSE(Type, IncludePath);
        EXPOSE(Type, LibraryPath);
        EXPOSE(Type, Library);
        EXPOSE(Type, Dependency);
        EXPOSE(Type, Define);
        EXPOSE(Type, Executable);
        EXPOSE(Language, Undefined);
        EXPOSE(Language, Binary);
        EXPOSE(Language, C);
        EXPOSE(Language, CXX);
        EXPOSE(Language, ASM);
        EXPOSE(Language, Script);
        EXPOSE(Language, UserDefined);
#undef EXPOSE

#define EXPOSE(TYPE, NAME) engine.add(chaiscript::fun([](const W_##TYPE & ) { return TYPE::NAME; }), #NAME)
        EXPOSE(Part, Cli);
        EXPOSE(Part, Pre);
        EXPOSE(Part, Deps);
        EXPOSE(Part, Output);
        EXPOSE(Part, Input);
        EXPOSE(Part, DepFile);
        EXPOSE(Part, Option);
        EXPOSE(Part, Define);
        EXPOSE(Part, IncludePath);
        EXPOSE(Part, ForceInclude);
        EXPOSE(Part, Library);
        EXPOSE(Part, LibraryPath);
        EXPOSE(OS, Windows);
        EXPOSE(OS, Linux);
        EXPOSE(OS, MacOS);
#undef EXPOSE
        engine.add(chaiscript::fun([](const W_OS & ) { return get_os(); }), "my");
        engine.add(chaiscript::fun([](OS lhs, OS rhs){return lhs == rhs;}), "==");
        engine.add(chaiscript::fun([](OS lhs, OS rhs){return lhs != rhs;}), "!=");

        engine.add(chaiscript::fun([](Flags & f, const Flags & to_set) { f.set(to_set); } ), "set");
        engine.add(chaiscript::fun(&Flags::to_string), "to_string");
        engine.add(chaiscript::fun([](const Flags & lhs, const Flags & rhs) { return lhs&rhs; } ), "&");
        engine.add(chaiscript::fun([](const Flags & lhs, const Flags & rhs) { return lhs||rhs; } ), "|");

        engine.add(chaiscript::type_conversion<Flags, bool>());
    }

    void initialize_toolchain()
    {
        using KVM = process::toolchain::KeyValuesMap;
        using Part = process::toolchain::Part;
        using T = process::toolchain::Translator;
        using TM = process::toolchain::TranslatorMap;
        using Toolchain = process::toolchain::Manager;

        {
            // key value maps
            engine.add(chaiscript::user_type<KVM>(), "KeyValueMap");
            engine.add(chaiscript::fun([](KVM & map, Part p) { map[p].clear(); }), "clear");
            engine.add(chaiscript::fun([](KVM & map, Part p, const std::string & key) { map[p].emplace_back(std::make_pair(key, std::string())); }), "append");
            engine.add(chaiscript::fun([](KVM & map, Part p, const std::string & key, const std::string & value) { map[p].emplace_back(std::make_pair(key, value)); }), "append");
        }
        {
            // translator map
            engine.add(chaiscript::user_type<T>(), "Translator");
            engine.add(chaiscript::user_type<TM>(), "TranslatorMap");

            engine.add(chaiscript::fun(static_cast<T & (TM::*)(const Part & )>(&TM::operator[])), "[]");

        }

        {
            engine.add(chaiscript::user_type<ToolchainElement>(), "Compiler");
            engine.add(chaiscript::user_type<ToolchainElement>(), "Linker");
            engine.add(chaiscript::user_type<ToolchainElement>(), "Archiver");
            engine.add(chaiscript::fun(&ToolchainElement::key_values), "key_values");
            engine.add(chaiscript::fun(&ToolchainElement::translators), "translators");
        }

        {
            // toolchain config
            engine.add(chaiscript::fun(&Toolchain::has_config), "has_config");
            engine.add(chaiscript::fun(&Toolchain::config_value), "config_value");
            engine.add(chaiscript::fun([](Toolchain & toolchain, const std::string & key) { return toolchain.configure(key); }), "set_config_value");
            engine.add(chaiscript::fun([](Toolchain & toolchain, const std::string & key, const std::string & value) { return toolchain.configure(key, value); }), "set_config_value");


            auto compiler = [](Toolchain & toolchain, Flags lang)
            {
                CHAI_MSS_BEGIN();
                CHAI_MSS(lang.only({Flag::Language}));

                Language l = lang.language().first;
                auto p = toolchain.compiler(l);
                return ToolchainElement(p);
            };
            engine.add(chaiscript::fun(compiler), "compiler");
            engine.add(chaiscript::fun([](Toolchain & toolchain) { return ToolchainElement(&toolchain.linker()); }), "linker");
            engine.add(chaiscript::fun([](Toolchain & toolchain) { return ToolchainElement(&toolchain.archiver()); }), "archiver");
        }
    }

    void initialize_uri()
    {
        using Uri = model::Uri;

        engine.add(chaiscript::user_type<Uri>(), "Uri");
        engine.add(chaiscript::constructor<Uri(const Uri &)>(), "Uri");
        engine.add(chaiscript::type_conversion<std::string, Uri>());
        engine.add(chaiscript::fun([](const model::Uri & uri) { return uri.string(); }), "to_string");
        engine.add(chaiscript::fun([](const model::Uri & uri, char c) { return uri.string(c); }), "to_string");
        engine.add(chaiscript::fun([](const model::Uri & uri, bool initial_separator) { return uri.string(initial_separator); }), "to_string");
        engine.add(chaiscript::fun([](const model::Uri & uri, bool initial_separator, char c) { return uri.string(initial_separator, c); }), "to_string");
        engine.add(chaiscript::fun([](const model::Uri & lhs, const model::Uri & rhs) { return lhs / rhs; }), "/");
        engine.add(chaiscript::fun(&model::Uri::as_absolute), "as_absolute");
        engine.add(chaiscript::fun(&model::Uri::as_relative), "as_relative");
    }

    void initialize_book()
    {
        engine.add(chaiscript::user_type<Book>(), "Book");
        engine.add(chaiscript::constructor<Book(const Book &)>(), "Book");

        engine.add(chaiscript::fun([](Book & book, const model::Uri & uri) { return book.book(uri); } ), "book");
        engine.add(chaiscript::fun([](Book & book, const model::Uri & uri, const Book::BookFunctor & functor) { book.book(uri, functor); } ), "book");
        engine.add(chaiscript::fun([](Book & book, const model::Uri & uri) { return book.has_recipe(uri); }), "has_recipe");
        engine.add(chaiscript::fun([](Book & book, const model::Uri & uri) { return book.recipe(uri); }), "recipe");
        engine.add(chaiscript::fun([](Book & book, const model::Uri & uri, const std::string & type) { return book.recipe(uri, type); }), "recipe");
        engine.add(chaiscript::fun([](Book & book, const model::Uri & uri, const Book::RecipeFunctor & functor) { book.recipe(uri, functor); }), "recipe");
        engine.add(chaiscript::fun([](Book & book, const model::Uri & uri, const std::string & type, const Book::RecipeFunctor & functor) { book.recipe(uri, type, functor); }), "recipe");
        engine.add(chaiscript::fun(&Book::data), "data");
        engine.add(chaiscript::fun(&Book::uri), "uri");
    }

    void initialize_recipe(Context * context)
    {
        engine.add(chaiscript::user_type<Recipe>(), "Recipe");
        engine.add(chaiscript::constructor<Recipe(const Recipe &)>(), "Recipe");

        engine.add(chaiscript::fun(&Recipe::add), "add");
        engine.add(chaiscript::fun(&Recipe::set_type), "set_type");
        engine.add(chaiscript::fun(&Recipe::working_directory), "working_dir");

        engine.add(chaiscript::fun([](Recipe & recipe, const model::Uri & dep) { recipe.depends_on(dep); }), "depends_on");

        {
            using DepFunction = std::function<bool (const chaiscript::Boxed_Value & vt)>;
            auto lambda = [=](Recipe & recipe, const model::Uri & dep, DepFunction function)
            {
                auto file_dep = [=](LanguageTypePair & ltp, ingredient::File & file)
                {
                    File f(ltp, file, context);
                    if (!function(chaiscript::var(std::ref(f))))
                        return false;

                    ltp = f.language_type_pair();
                    file = f.element();
                    return true;

                };

                auto key_value_dep = [=](LanguageTypePair & ltp, ingredient::KeyValue & key_value)
                {
                    KeyValue kv(ltp, key_value, context);
                    return function(chaiscript::var(std::ref(kv)));

                    ltp = kv.language_type_pair();
                    key_value = kv.element();
                    return true;
                };

                recipe.depends_on(dep, file_dep, key_value_dep);
            };
            engine.add(chaiscript::fun(lambda), "depends_on");
        }


        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & dir, const std::string & pattern) { recipe.add(dir, pattern); }), "add");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & dir, const std::string & pattern, Recipe::GlobFunctor functor) { recipe.add(dir, pattern, Flags(), functor); }), "add");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & dir, const std::string & pattern, const Flags & flags) { recipe.add(dir, pattern, flags); }), "add");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & dir, const std::string & pattern, const Flags & flags, Recipe::GlobFunctor functor) { recipe.add(dir, pattern, flags, functor); }), "add");

        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & command) { recipe.run(command); }), "run");

        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & dir, const std::string & pattern) { recipe.remove(dir, pattern); }), "remove");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & dir, const std::string & pattern, const Flags & flags) { recipe.remove(dir, pattern, flags); }), "remove");

        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & name) { recipe.library(name); } ), "library");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & path) { recipe.library_path(path); } ), "library_path");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & path) { recipe.include_path(path); } ), "include_path");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & name) { recipe.define(name); } ), "define");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & name, const std::string & value) { recipe.define(name, value); } ), "define");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & name, const Flags & flags) { recipe.library(name, flags); } ), "library");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & path, const Flags & flags) { recipe.library_path(path, flags); } ), "library_path");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & path, const Flags & flags) { recipe.include_path(path, flags); } ), "include_path");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & name, const Flags & flags) { recipe.define(name, flags); } ), "define");
        engine.add(chaiscript::fun([](Recipe & recipe, const std::string & name, const std::string & value, const Flags & flags) { recipe.define(name, value, flags); } ), "define");
        engine.add(chaiscript::fun(&Recipe::set_working_directory), "set_working_directory");

        engine.add(chaiscript::fun(&Recipe::uri), "uri");
        engine.add(chaiscript::fun(&Recipe::data), "data");
    }

    void initialize_file(const Context * context)
    {
        engine.add(chaiscript::user_type<File>(), "File");

        engine.add(chaiscript::fun([](const File & f) -> const Flags &  { return f.flags(); }), "flags");
        engine.add(chaiscript::fun([](File & f) -> Flags & { return f.flags(); }), "flags");
        engine.add(chaiscript::fun([](const File & f) { return f.has_owner(); }), "has_owner");
        engine.add(chaiscript::fun([](const File & f) { return f.owner(); }), "owner");
        engine.add(chaiscript::fun(&File::key), "key");
        engine.add(chaiscript::fun(&File::dir), "dir");
        engine.add(chaiscript::fun(&File::rel), "rel");
        engine.add(chaiscript::fun(&File::is_file), "is_file");
        engine.add(chaiscript::fun(&File::is_key_value), "is_key_value");


        {
            auto lambda = [=](const std::string & dir, const std::string & rel)
            {
                return File(LanguageTypePair(Language::Undefined, Type::Undefined), ingredient::File(dir, rel), context);
            };
            engine.add(chaiscript::fun(lambda), "File");
        }
    }

    void initialize_key_value(const Context * context)
    {
        engine.add(chaiscript::user_type<KeyValue>(), "KeyValue");

        engine.add(chaiscript::fun([](const KeyValue & f) -> const Flags & { return f.flags(); }), "flags");
        engine.add(chaiscript::fun([](KeyValue & f) -> Flags & { return f.flags(); }), "flags");
        engine.add(chaiscript::fun([](const KeyValue & f) { return f.has_owner(); }), "has_owner");
        engine.add(chaiscript::fun([](const KeyValue & f) { return f.owner(); }), "owner");
        engine.add(chaiscript::fun(&KeyValue::key), "key");
        engine.add(chaiscript::fun(&KeyValue::has_value), "has_value");
        engine.add(chaiscript::fun(&KeyValue::value), "value");
        engine.add(chaiscript::fun(&KeyValue::is_file), "is_file");
        engine.add(chaiscript::fun(&KeyValue::is_key_value), "is_key_value");

        {
            auto lambda = [=](const std::string & key)
            {
                return KeyValue(LanguageTypePair(Language::Undefined, Type::Undefined), ingredient::KeyValue(key), context);
            };
            engine.add(chaiscript::fun(lambda), "KeyValue");
        }

        {
            auto lambda = [=](const std::string & key, const std::string & value)
            {
                return KeyValue(LanguageTypePair(Language::Undefined, Type::Undefined), ingredient::KeyValue(key, value), context);
            };
            engine.add(chaiscript::fun(lambda), "KeyValue");
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


Result Context::load(const std::string & recipe)
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
