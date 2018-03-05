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

}

void Processor::process(gubg::naft::Node & node, model::Recipe * recipe)
{

}

}

Result Naft::set_option(const std::string & option)
{
    set_filename(option);
}

bool Naft::can_process(const Context & context) const
{
    return context.menu().is_valid();
}

Result Naft::process(std::ostream & oss, const Context & context)
{

}

} }
