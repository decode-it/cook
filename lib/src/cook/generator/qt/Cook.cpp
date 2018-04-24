#include "cook/generator/qt/Cook.hpp"
#include "cook/util/File.hpp"
#include "cook/algo/TopologicalOrder.hpp"
#include "cook/Context.hpp"
#include "gubg/naft/Document.hpp"

namespace cook { namespace generator { namespace qt {

namespace  {

struct Processor
{
    explicit Processor(const Context & context) : context(context) {}

    void construct_book_list();
    void process(gubg::naft::Node & node, model::Book * book);
    void process(gubg::naft::Node & node, model::Recipe * recipe);

    const Context & context;
    std::unordered_set<model::Book *> used_books_;
    std::unordered_set<model::Recipe *> used_recipes_;
};

void Processor::construct_book_list()
{
    const auto & recipes = context.menu().topological_order_recipes();
    used_recipes_.insert(RANGE(recipes));

    for(const model::Recipe * recipe : recipes)
    {
        // get the parent book
        model::Book * book = recipe->parent();

        // go upwards until root or until already present
        while (book != nullptr && used_books_.insert(book).second)
            book = book->parent();
    }
}

void Processor::process(gubg::naft::Node & node, model::Book * book)
{
    if (used_books_.find(book) == used_books_.end())
        return;

    auto n = node.node("book");
    n.attr("uri", book->uri()).attr("display_name", book->name());

    // process the subbooks
    for(model::Book * child : book->books())
        process(n, child);

    // process the recipes
    for(model::Recipe * child: book->recipes())
        process(n, child);
}

void Processor::process(gubg::naft::Node & node, model::Recipe * recipe)
{
    if (used_recipes_.find(recipe) == used_recipes_.end())
        return;

    std::filesystem::path comb = gubg::filesystem::combine({ std::filesystem::current_path(), context.dirs().recipe(), recipe->working_directory() });
    auto make_absolute = [&](const std::filesystem::path & p)
    {
        return gubg::filesystem::combine(comb, p).string();
    };

    const auto & uri = recipe->uri();

    auto n = node.node("recipe");
    n.attr("uri", uri);
    n.attr("tag", uri.name());
    n.attr("display_name", recipe->name());
    n.attr("path", make_absolute(recipe->working_directory().string()));
    n.attr("type", recipe->build_target().type);

    {
        const auto & fn = recipe->build_target().filename;
        n.attr("build_target", fn ? make_absolute(*fn) : "");
    }



    auto write_files = [&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        if (false) {}
        else if (ltp.type == Type::Source || ltp.type == Type::Header)
            n.node("file").attr("type", (ltp.type == Type::Source ? 2 : 1)).attr("path", make_absolute(file.key()));
        else if (ltp.type == Type::IncludePath)
            n.node("include_path").attr("path", make_absolute(file.key()));

        return true;
    };
    recipe->files().each(write_files);


    auto write_key_values = [&](const LanguageTypePair & ltp, const ingredient::KeyValue & key_value)
    {
        if (false) {}
        else if (ltp.type == Type::Define)
        {
            auto nn = n.node("define");
            nn.attr("name", key_value.key());
            if (key_value.has_value())
                nn.attr("value", key_value.value());
        }

        return true;
    };
    recipe->key_values().each(write_key_values);
}

}

Result Cook::set_option(const std::string & option)
{
    MSS_BEGIN(Result);
    MSS_END();
}

bool Cook::can_process(const Context & context) const
{
    return context.menu().is_valid();
}

Result Cook::process(const Context & context)
{
    MSS_BEGIN(Result);

    Processor p(context);
    p.construct_book_list();;

    auto n = gubg::naft::Document(std::cout).node("structure");

    p.process(n, context.root_book());

    MSS_END();
}

} } }
