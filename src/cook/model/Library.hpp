#ifndef HEADER_cook_model_Library_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Library_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/network/DAG.hpp"

namespace cook { namespace model { 

    using RecipeDAG = gubg::network::DAG<Recipe>;

    using BookPath = std::vector<Book*>;
    using ConstBookPath = std::vector<const Book*>;

    template <typename Path>
    inline void uri(std::ostream &os, const Path &path, const Recipe *recipe = nullptr)
    {
        Uri u;
        for (auto ptr: path)
            u.add_path_part(ptr->name());
        if (recipe)
            u.set_name(recipe->name());
        u.stream(os);
    }

    class Library
    {
    public:
        Library() { }

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
                //We add the version _without_ root as well to make the matching more user-friendly
                recipe_per_uri[recipe->uri_hr(false)] = recipe;

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
                    MSS(!!to, std::cout << "Error: Could not find dependency " << rn << std::endl);
                    MSS(dag.add_edge(recipe, to), (std::cout << "Error: Failed to add edge from " << recipe->uri_hr() << " to " << to->uri_hr() << std::endl, dag.stream(std::cout, [](const Recipe &r){return r.uri_hr();})));
                    MSS_END();
                };
                MSS(recipe->each_dependency(add_edge));

                MSS_END();
            };
            {
                bool ok = true;
                each([&](const BookPath &path, Recipe *recipe){ ok = ok && setup_uris(path, recipe); return true; });
                each([&](const BookPath &path, Recipe *recipe){ ok = ok && add_to_dag(path, recipe); return true; });
                MSS(ok);
            }

            MSS(dag.remove_unreachables(recipe_per_uri[rn]));

            auto distribute = [&](Recipe &from, const Recipe &to){ return from.merge(to); };
            MSS(dag.each_edge<gubg::network::Direction::Backward>(distribute));
            
            MSS_END();
        }

        Book *goc_book(const Uri &uri)
        {
            Book *book = &root_book_;
            uri.each_path_part([&](const auto &name){
                    book = &book->goc_book(name);
                    });
            return book;
        }
        Book *goc_book(const std::string &uri_str)
        {
            const model::Uri uri(uri_str);
            return goc_book(uri);
        }

        Recipe *create_recipe(const std::string &uri_str)
        {
            const model::Uri uri(uri_str);
            auto book = goc_book(uri);
            if (!book)
                return nullptr;
            return book->create_recipe(uri.name());
        }
        Recipe *get_recipe(const std::string &uri_str)
        {
            const model::Uri uri(uri_str);
            auto book = goc_book(uri);
            if (!book)
                return nullptr;
            return book->get_recipe(uri.name());
        }

        template <typename Ftor>
        bool each(Ftor ftor) const
        {
            ConstBookPath path = {&root_book_};
            return each_(ftor, path);
        }
        template <typename Ftor>
        bool each(Ftor ftor)
        {
            BookPath path = {&root_book_};
            return each_(ftor, path);
        }

        const Book &root() const {return root_book_;}

        void stream(std::ostream &os) const
        {
            os << "Library" <<std::endl;
            each([&](const ConstBookPath &path, const Recipe *recipe){
                    os << (!!recipe ? "Recipe: " : "Book: "); uri(os, path, recipe); os << std::endl;
                    if (!!recipe)
                    recipe->stream(os);
                    else
                    path.back()->stream(os);
                    return true;
                    });
        }

    private:
        template <typename Ftor, typename Path>
        static bool each_(Ftor ftor, Path &path)
        {
            MSS_BEGIN(bool);
            for (auto &p: path.back()->book_per_name())
            {
                path.push_back(&p.second);
                MSS(ftor(path, nullptr));
                for (auto &p: p.second.recipe_per_name())
                {
                    MSS(ftor(path, &p.second));
                }
                MSS(each_(ftor, path));
                path.pop_back();
            }
            MSS_END();
        }

        Book root_book_{"ROOT_BOOK"};
    };

} } 

#endif
