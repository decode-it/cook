#include "cook/generator/HTML.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/mss.hpp"
#include <fstream>
#include <set>
#include <stack>

namespace cook { namespace generator { 

    namespace  { 
        struct Data
        {
            bool has_unresolved_dependencies = false;
            std::set<model::Uri> recipe_uris;

            bool process(const Context &context)
            {
                MSS_BEGIN(bool);
                has_unresolved_dependencies = !context.menu().is_valid();
                if (!has_unresolved_dependencies)
                {
                    for (const auto rcp: context.menu().topological_order_recipes())
                        recipe_uris.insert(rcp->uri());
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
                        cur->each_recipe([&](auto * rcp) { recipe_uris.insert(rcp->uri()); return true; });
                    }
                }
                MSS_END();
            }
            void stream_index(std::ostream &os) const
            {
                os << "<!DOCTYPE html>" << std::endl;
                os << "<html>" << std::endl;
                os << "<head>" << std::endl;
                os << "<title>Recipes overview</title>" << std::endl;
                os << "</head>" << std::endl;
                os << "<body>" << std::endl;
                os << "<h1>Recipes overview</h1>" << std::endl;
                if (has_unresolved_dependencies)
                    os << "<font color=\"red\">Not all recipes could be resolved</font>" << std::endl;
                else
                    os << "<font color=\"green\">All recipes could be resolved</font>" << std::endl;
                os << "<p>Recipes</p>" << std::endl;
                os << "<ul>" << std::endl;
                for (const auto &uri: recipe_uris)
                    os << "<li><a href=\"" << fn(uri, "details") << "\">" << uri << "</a></li>" << std::endl;
                os << "</ul>" << std::endl;
                os << "</body>" << std::endl;
                os << "</html>" << std::endl;
            }
            static std::string fn(const model::Uri &uri, const std::string &name)
            {
                std::string res = "html/";
                res += name;
                res += uri.string('_');
                res += ".html";
                return res;
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

        set_filename("index.html");
        std::ofstream fo;
        MSS(open_output_stream(context, fo));

        data.stream_index(fo);

        MSS_END();
    }
} } 
