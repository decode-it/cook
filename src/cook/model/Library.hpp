#ifndef HEADER_cook_model_Library_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Library_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"
#include "cook/model/Recipe.hpp"
#include "gubg/network/DAG.hpp"

namespace cook { namespace model { 

    using RecipeDAG = gubg::network::DAG<Recipe>;

    class Library
    {
    public:
        Library()
        {
            path_.push_back(&root_);
        }

        bool get(RecipeDAG &dag, const std::string &rn)
        {
            MSS_BEGIN(bool);
            dag.clear();
            std::ostringstream oss;
            std::map<std::string, Recipe*> recipe_per_uri;
            auto setup_uris = [&](const BookPath &path, Recipe *recipe){
                MSS_BEGIN(bool);

                if (!recipe) MSS_RETURN_OK();

                recipe->set_path(path);
                recipe_per_uri[recipe->uri_hr()] = recipe;

                MSS_END();
            };
            auto add_to_dag = [&](const BookPath &path, Recipe *recipe){
                MSS_BEGIN(bool);

                if (!recipe) MSS_RETURN_OK();

                MSS(dag.add_vertex(recipe, false));

                auto add_edge = [&](const std::string &rn)
                {
                    MSS_BEGIN(bool);
                    auto to = recipe_per_uri[rn];
                    std::cout << C(recipe->uri_hr())C(to)C(rn) << std::endl;
                    MSS(!!to);
                    MSS(dag.add_edge(recipe, to), (std::cout << "Failed to add edge from " << recipe->uri_hr() << " to " << to->uri_hr() << std::endl, dag.stream(std::cout, [](const Recipe &r){return r.uri_hr();})));
                    MSS_END();
                };
                MSS(recipe->each_dependency(add_edge));

                MSS_END();
            };
            {
                bool ok = true;
                each([&](const BookPath &path, Recipe *recipe){ ok = ok && setup_uris(path, recipe); });
                each([&](const BookPath &path, Recipe *recipe){ ok = ok && add_to_dag(path, recipe); });
                MSS(ok);
            }

            MSS(dag.remove_unreachables(recipe_per_uri[rn]));

            auto distribute = [&](Recipe *from, const Recipe *to){ return from->merge(*to); };
            MSS(dag.depth_first(distribute));
            
            MSS_END();
        }

        Recipe *current_recipe() {return current_recipe_;}

        void push(const std::string &name)
        {
            Book &book = path_.back()->goc_book(name);
            path_.push_back(&book);
        }
        void pop()
        {
            path_.pop_back();
        }

        bool create_recipe(const std::string &name)
        {
            Book &book = *path_.back();
            current_recipe_ = book.create_recipe(name);
            return !!current_recipe_;
        }
        void close_recipe()
        {
            current_recipe_ = nullptr;
        }

        template <typename Ftor>
        void each(Ftor ftor) const
        {
            ConstBookPath path = {&root_};
            each_(ftor, path);
        }
        template <typename Ftor>
        void each(Ftor ftor)
        {
            BookPath path = {&root_};
            each_(ftor, path);
        }

        void stream(std::ostream &os) const
        {
            os << "Library" <<std::endl;
            each([&](const ConstBookPath &path, const Recipe *recipe){
                    os << (!!recipe ? "Recipe: " : "Book: "); uri(os, path, recipe); os << std::endl;
                    if (!!recipe)
                    recipe->stream(os);
                    else
                    path.back()->stream(os);
                    });
        }

    private:
        template <typename Ftor, typename Path>
        static void each_(Ftor ftor, Path &path)
        {
            for (auto &p: path.back()->book_per_name())
            {
                path.push_back(&p.second);
                ftor(path, nullptr);
                for (auto &p: p.second.recipe_per_name())
                {
                    ftor(path, &p.second);
                }
                each_(ftor, path);
                path.pop_back();
            }
        }

        Book root_{"ROOT_BOOK"};
        BookPath path_;
        Recipe *current_recipe_ = nullptr;
    };

} } 

#endif
