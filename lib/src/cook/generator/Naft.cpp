#include "cook/generator/Naft.hpp"
#include "cook/util/File.hpp"
#include "cook/algo/TopologicalOrder.hpp"
#include "cook/Context.hpp"
#include "gubg/naft/Document.hpp"

namespace cook { namespace generator {

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

    const auto & uri = recipe->uri();

    auto n = node.node("recipe");
    n.attr("uri", uri);
    n.attr("tag", uri.name());
    n.attr("display_name", recipe->name());
    n.attr("path", recipe->working_directory().string());
    n.attr("type", recipe->type());
    n.attr("build_target", "");
}

}

Result Naft::set_option(const std::string & option)
{
    MSS_BEGIN(Result);
    set_filename(option);
    MSS_END();
}

bool Naft::can_process(const Context & context) const
{
    return context.menu().is_valid();
}

Result Naft::process(const Context & context)
{
    MSS_BEGIN(Result);

    std::ofstream ofs;
    MSS(open_output_stream(context, ofs));

    Processor p(context);
    p.construct_book_list();;

    auto n = gubg::naft::Document(ofs).node("structure");

    p.process(n, context.root_book());

    MSS_END();
}

} }
