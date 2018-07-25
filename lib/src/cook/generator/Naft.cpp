#include "cook/generator/Naft.hpp"
#include "cook/util/File.hpp"
#include "cook/algo/TopologicalOrder.hpp"
#include "cook/Context.hpp"
#include "cook/process/toolchain/Manager.hpp"
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
    if (context.menu().is_valid())
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
    else
    {
        // just add all the books and recipes, we've got unresolved dependencies
        std::stack<model::Book*> todo;
        todo.push(context.root_book());

        while(!todo.empty())
        {
            model::Book * cur = todo.top();
            todo.pop();

            used_books_.insert(cur);
            cur->each_book([&](auto * book){ todo.push(book); return true; });
            cur->each_recipe([&](auto * recipe) { used_recipes_.insert(recipe); return true; });
        }
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
    n.attr("tag", uri.path().back());
    n.attr("display_name", recipe->name());
    n.attr("path", recipe->working_directory().string());
    n.attr("type", recipe->build_target().type);
    n.attr("build_target", recipe->build_target().name);
    
    {
        const auto & bt = recipe->build_target();
        auto nn = n.node("target");
        nn.attr("type", bt.type).attr("name", bt.name);
        if (bt.filename)
            nn.attr("filename", bt.filename->string());
        else
            nn.attr("filename", context.toolchain().get_primary_target(*recipe).string());
    }

    auto base_naft = [](gubg::naft::Node & node, const LanguageTypePair & ltp, const auto & ingredient)
    {
        node.attr("Type", ltp.type)
            .attr("Language", ltp.language)
            .attr("Propagation", ingredient.propagation())
            .attr("Overwrite", ingredient.overwrite())
            .attr("content", ingredient.content())
            .attr("user_generated", is_internal_generated(ingredient.content()) ? "0" : "1");

        if (!!ingredient.owner())
            node.attr("owner", ingredient.owner()->uri());
        else
            node.attr("owner");
    };

    recipe->files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        auto nn = n.node("file");
        base_naft(nn, ltp, file);
        nn.attr("dir", file.dir().string()).attr("rel", file.rel().string());

        return true;
    });
    
    recipe->key_values().each([&](const LanguageTypePair & ltp, const ingredient::KeyValue & key_value)
    {
        auto nn = n.node("key_value");
        base_naft(nn, ltp, key_value);
        nn.attr("key", key_value.key());
        if (key_value.has_value())
            nn.attr("value", key_value.value());

        return true;
    });

    for(const auto & p :recipe->dependency_pairs())
    {
        auto nn  = n.node("dependency");
        nn.attr("key", p.first).attr("resolved", p.second.recipe ? 1 : 0);
        if (p.second.recipe)
            nn.attr("uri", p.second.recipe->uri());
    }
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
    return true;
}

Result Naft::process(const Context & context)
{
    MSS_BEGIN(Result);

    std::ofstream ofs;
    MSS(open_output_stream(context, ofs));

    
    auto doc = gubg::naft::Document(ofs);
    // the config
    {
        auto n = doc.node("configuration");
        auto lambda = [&](const auto & key, const auto & value, bool resolved)
        {
            auto nn = n.node("config");
            nn.attr("key", key).attr("value", value).attr(resolved ? "resolved" : "unresolved");
        };
        context.toolchain().each_config(lambda);
    }

    // the structure
    {
        Processor p(context);
        p.construct_book_list();
        auto n = doc.node("structure");
        p.process(n, context.root_book());
    }

    MSS_END();
}

} }
