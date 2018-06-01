#include "cook/generator/HTML.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/mss.hpp"
#include <fstream>
#include <map>
#include <stack>

namespace cook { namespace generator { 

    namespace  { 
        std::string fn(const std::string &name)
        {
            std::string res;
            res += name;
            res += ".html";
            return res;
        }
        std::string fn(const model::Uri &uri, const std::string &name)
        {
            std::string res;
            res += name;
            res += uri.string('_');
            res += ".html";
            return res;
        }

        template <typename Ftor>
        void stream_rcp_links(std::ostream &os, Ftor &&ftor)
        {
            os << "<table>" << std::endl;
            os << "<tr>" << std::endl;
            os << "<th>URI</th>" << std::endl;
            os << "<th>TargetType</th>" << std::endl;
            os << "<th>Name</th>" << std::endl;
            os << "<th>Filename</th>" << std::endl;
            os << "</tr>" << std::endl;
            auto stream_rcp_link = [&](const model::Recipe &rcp){
                const auto uri = rcp.uri();
                const auto &bt = rcp.build_target();
                os << "<tr>" << std::endl;
                os << "<td><a href=\"" << fn(uri, "details") << "\">" << uri << "</a></td>" << std::endl;
                os << "<td>" << bt.type << "</td>" << std::endl;
                os << "<td>" << bt.name << "</td>" << std::endl;
                os << "<td>" << (!!bt.filename ? *bt.filename : "") << "</td>" << std::endl;
                os << "</tr>" << std::endl;
            };
            ftor([&](const model::Recipe &rcp){stream_rcp_link(rcp);});
            os << "</table>" << std::endl;
        }

        void stream_style(std::ostream &os)
        {
            os << R"abc(<style>
table {
    font-family: arial, sans-serif;
    border-collapse: collapse;
    width: 100%;
}

td, th {
    border: 1px solid #dddddd;
    text-align: left;
    padding: 8px;
}

tr:nth-child(even) {
    background-color: #dddddd;
}
</style>)abc" << std::endl;
            }

struct RecipeData
{
    const model::Recipe *rcp = nullptr;

    void stream_details(std::ostream &os) const
    {
        os << "<!DOCTYPE html>" << std::endl;
        os << "<html>" << std::endl;
        os << "<head>" << std::endl;
        stream_style(os);
        if (rcp)
        {
            os << "<title>Recipe details for " << rcp->uri() << "</title>" << std::endl;
        }
        os << "</head>" << std::endl;
        os << "<body>" << std::endl;
        if (rcp)
        {
            os << "<a href=\"" << fn("index") << "\">Back to overview</a>" << std::endl;
            os << "<h1>Recipe details for " << rcp->uri() << "</h1>" << std::endl;

            {
                os << "<h2>Details</h2>" << std::endl;
                os << "<table>" << std::endl;
                os << "<tr>" << std::endl;
                os << "<th>Item</th>" << std::endl;
                os << "<th>Value</th>" << std::endl;
                os << "</tr>" << std::endl;
                const auto &bt = rcp->build_target();
                os << "<tr><td>TargetType</td><td>" << bt.type << "</td></tr>" << std::endl;
                os << "<tr><td>Name</td><td>" << bt.name << "</td></tr>" << std::endl;
                os << "<tr><td>Filename</td><td>" << (!!bt.filename ? *bt.filename : "") << "</td></tr>" << std::endl;
                os << "</table>" << std::endl;
            }

            os << "<h2>Dependencies</h2>" << std::endl;
            stream_rcp_links(os, [&](auto cb){for (const auto *dep: rcp->dependencies()) cb(*dep);});

            os << "<h2>Key-values</h2>" << std::endl;
            os << "<table>" << std::endl;
            os << "<tr>" << std::endl;
            os << "<th>Language</th>" << std::endl;
            os << "<th>Type</th>" << std::endl;
            os << "<th>Key</th>" << std::endl;
            os << "<th>Value</th>" << std::endl;
            os << "</tr>" << std::endl;
            for (const auto &p: rcp->key_values())
            {
                const auto &ltp = p.first;
                for (const auto &kv: p.second)
                {
                    os << "<tr>";
                    os << "<td>" << ltp.language << "</td>";
                    os << "<td>" << ltp.type << "</td>";
                    os << "<td>" << kv.key() << "</td>";
                    os << "<td>" << (kv.has_value() ? kv.value() : "") << "</td>";
                    os << "</tr>" << std::endl;
                }
            }
            os << "</table>" << std::endl;

            os << "<h2>Files</h2>" << std::endl;
            os << "<table>" << std::endl;
            os << "<tr>" << std::endl;
            os << "<th>Language</th>" << std::endl;
            os << "<th>Type</th>" << std::endl;
            os << "<th>Directory</th>" << std::endl;
            os << "<th>Path</th>" << std::endl;
            os << "</tr>" << std::endl;
            for (const auto &p: rcp->files())
            {
                const auto &ltp = p.first;
                const auto &files = p.second;
                for (const auto &file: files)
                {
                    os << "<tr>";
                    os << "<td>" << ltp.language << "</td>";
                    os << "<td>" << ltp.type << "</td>";
                    os << "<td>" << file.dir() << "</td>";
                    os << "<td>" << file.rel() << "</td>";
                    os << "</tr>" << std::endl;
                }
            }
            os << "</table>" << std::endl;

        }
        os << "</body>" << std::endl;
        os << "</html>" << std::endl;
    }
};
struct Data
{
    bool has_unresolved_dependencies = false;
    std::map<model::Uri, RecipeData> recipe_data;

    bool process(const Context &context)
    {
        MSS_BEGIN(bool);
        has_unresolved_dependencies = !context.menu().is_valid();
        if (!has_unresolved_dependencies)
        {
            for (const auto rcp: context.menu().topological_order_recipes())
            {
                RecipeData data;
                data.rcp = rcp;
                recipe_data.emplace(rcp->uri(), data);
            }
        }
        else
        {
            std::stack<model::Book*> todo;
            todo.push(context.root_book());

            while (!todo.empty())
            {
                model::Book * cur = todo.top();
                todo.pop();

                cur->each_book([&](auto * book){ todo.push(book); return true; });
                cur->each_recipe([&](auto * rcp) {
                        RecipeData data;
                        data.rcp = rcp;
                        recipe_data.emplace(rcp->uri(), data);
                        return true;
                        });
            }
        }
        MSS_END();
    }
    void stream_index(std::ostream &os) const
    {
        os << "<!DOCTYPE html>" << std::endl;
        os << "<html>" << std::endl;
        os << "<head>" << std::endl;
        stream_style(os);
        os << "<title>Recipes overview</title>" << std::endl;
        os << "</head>" << std::endl;
        os << "<body>" << std::endl;
        os << "<h1>Recipes overview</h1>" << std::endl;
        if (has_unresolved_dependencies)
            os << "<font color=\"red\">Not all recipes could be resolved</font>" << std::endl;
        else
            os << "<font color=\"green\">All recipes could be resolved</font>" << std::endl;
        os << "<h2>Recipes</h2>" << std::endl;
        stream_rcp_links(os, [&](auto cb){for (const auto &p: recipe_data) cb(*p.second.rcp);});
        os << "</body>" << std::endl;
        os << "</html>" << std::endl;
    }
};
} 

static constexpr const char *logns = "";

Result HTML::set_option(const std::string & option)
{
    MSS_BEGIN(Result, logns);
    L(C(option));
    MSS_END();
}

bool HTML::can_process(const Context & context) const
{
    MSS_BEGIN(bool, logns);
    MSS_END();
}
Result HTML::process(const Context & context)
{
    MSS_BEGIN(Result, logns);

    Data data;
    MSS(data.process(context));

    set_filename(fn("html/index"));
    std::ofstream fo;
    MSS(open_output_stream(context, fo));

    data.stream_index(fo);

    for (const auto &p: data.recipe_data)
    {
        const auto &uri = p.first;
        const auto &rcp = p.second;

        set_filename(fn(uri, "html/details"));
        std::ofstream fo;
        MSS(open_output_stream(context, fo));

        rcp.stream_details(fo);
    }

    MSS_END();
}
} } 
