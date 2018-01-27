#ifndef HEADER_cook_model_Library_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Library_hpp_ALREADY_INCLUDED

#include "cook/model/Book.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Uri.hpp"
#include "gubg/network/DAG.hpp"
#include "gubg/OnlyOnce.hpp"

namespace cook { namespace model { 

    using RecipeDAG = gubg::network::DAG<Recipe>;

    using BookPath = std::vector<Book*>;
    using ConstBookPath = std::vector<const Book*>;

    template <typename Path>
    inline void uri(std::ostream &os, const Path &path, const Recipe *recipe = nullptr)
    {
        Uri u;
        gubg::OnlyOnce skip_root;
        for (auto ptr: path)
        {
            if (skip_root())
                continue;
            u.add_path_part(ptr->name());
        }
        if (recipe)
            u.set_name(recipe->name());
        u.stream(os);
    }

    class Library
    {
    private:
        using RecipeIndex = std::map<std::string, Recipe*>;
    public:
        Library() { }

        bool resolve()
        {
            MSS_BEGIN(bool, "");
            const auto recipe_index = create_recipe_index_();
            auto src_recipe = [&](Recipe &recipe)
            {
                MSS_BEGIN(bool);
                auto resolve_dst = [&](const std::string &uri_str, Recipe *&dst)
                {
                    MSS_BEGIN(bool);


                    const Uri uri{uri_str};
                    if (uri.absolute())
                    {
                        auto p = recipe_index.find(uri_str);
                        MSS(p != recipe_index.end(), std::cout << "Error: Could not resolve absolute " << uri_str << " for " << recipe.uri_hr() << std::endl);
                        dst = p->second;
                    }
                    else
                    {
                        auto left = recipe.uri();
                        for (int size = left.path_size(); size >= 0; --size)
                        {
                            left.resize_path(size);
                            left.append(uri);
                            L(C(size)C(left));
                            auto p = recipe_index.find(left.str());
                            if (p != recipe_index.end())
                            {
                                dst = p->second;
                                MSS_RETURN_OK();
                            }
                        }
                        MSS(false, std::cout << "Error: Could not resolve relative " << uri_str << " for " << recipe.uri_hr() << std::endl);
                    }
                    MSS_END();
                };
                MSS(recipe.each_dependency(resolve_dst));
                MSS_END();
            };


            auto src_recipes_only = [&](const BookPath &path, Recipe *recipe)
            {
              if (!recipe)
                  return true;

              return src_recipe(*recipe);
            };

            MSS(each(src_recipes_only));
            MSS_END();
        }

        bool get(RecipeDAG &dag, const std::string &target_uri)
        {
            MSS_BEGIN(bool);

            Recipe *target_recipe = nullptr;

            dag.clear();
            auto add_to_dag = [&](const BookPath &path, Recipe *recipe){
                MSS_BEGIN(bool);

                if (!recipe) MSS_RETURN_OK();

                MSS(dag.add_vertex(recipe, false));

                if (target_uri == recipe->uri_hr(true) || target_uri == recipe->uri_hr(false))
                    target_recipe = recipe;

                auto add_edge = [&](const std::string &dst_rn, Recipe *dst_recipe)
                {
                    MSS_BEGIN(bool);
                    L("Adding edge from " << recipe->uri_hr() << " to " << dst_rn);
                    MSS(!!dst_recipe, std::cout << "Error: Dependency " << dst_rn << " was not resolved" << std::endl);
                    MSS(dag.add_edge(recipe, dst_recipe), (std::cout << "Error: Failed to add edge from " << recipe->uri_hr() << " to " << dst_recipe->uri_hr() << std::endl, dag.stream(std::cout, [](const Recipe &r){return r.uri_hr();})));
                    MSS_END();
                };
                MSS(recipe->each_dependency(add_edge));

                MSS_END();
            };
            MSS(each([&](const BookPath &path, Recipe *recipe){ return add_to_dag(path, recipe); }));

            if (!target_uri.empty())
            {
                MSS(!!target_recipe, std::cout << "Error: Could not find target recipe \"" << target_uri << "\"" << std::endl);
                MSS(dag.remove_unreachables(target_recipe));
            }

            auto distribute = [&](Recipe &from, const Recipe &to){ return from.merge(to); };
            MSS(dag.each_edge<gubg::network::Direction::Backward>(distribute));
            
            MSS_END();
        }

        Book * goc_book(Book * book, const Uri & uri)
        {
            uri.each_path_part([&](const auto &name) { book = &book->goc_book(name); });
            return book;
        }

        Book * goc_book(Book * book, const std::string & uri_str)
        {
            return goc_book(book, Uri::book_uri(uri_str));
        }

        Book *goc_book(const Uri &uri)
        {
            return goc_book(&root_book_, uri);
        }
        Book *goc_book(const std::string &uri_str)
        {
            return goc_book(Uri::book_uri(uri_str));
        }

        Recipe *create_recipe(Book * book, const Uri & uri)
        {
            Book * parent = goc_book(book, uri);
            if(!parent)
                return nullptr;
            return book->create_recipe(uri.name());
        }

        Recipe *create_recipe(Book * book, const std::string &uri_str)
        {
            return create_recipe(book, Uri::recipe_uri(uri_str));
        }

        Recipe *create_recipe(const Uri & uri)
        {
            auto book = goc_book(uri);
            if (!book)
                return nullptr;
            return book->create_recipe(uri.name());
        }

        Recipe *create_recipe(const std::string &uri_str)
        {
            return create_recipe(Uri::recipe_uri(uri_str));
        }

        Recipe *get_recipe(Book * book, const Uri & uri)
        {
            auto parent = goc_book(book, uri);
            if (!parent)
                return nullptr;
            return parent->get_recipe(uri.name());
        }

        Recipe *get_recipe(Book * book, const std::string &uri_str)
        {
            return get_recipe(book, Uri::recipe_uri(uri_str));
        }

        Recipe *get_recipe(const Uri & uri)
        {
            auto book = goc_book(uri);
            if (!book)
                return nullptr;
            return book->get_recipe(uri.name());
        }

        Recipe *get_recipe(const std::string &uri_str)
        {
            return get_recipe(Uri::recipe_uri(uri_str));
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

        const Book &root() const    { return root_book_; }
        Book &root()                { return root_book_; }

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
        RecipeIndex create_recipe_index_()
        {
            RecipeIndex index;
            auto add_recipe_to_index = [&](const BookPath &path, Recipe *recipe){
                if (recipe)
                    index[recipe->uri_hr()] = recipe;
                return true;
            };
            each([&](const BookPath &path, Recipe *recipe){ return add_recipe_to_index(path, recipe); });
            return index;
        }
        template <typename Ftor, typename Path>
        static bool each_(Ftor ftor, Path &path)
        {
            MSS_BEGIN(bool);

            MSS(!path.empty());
            auto * book = path.back();

            // add the current path
            MSS(ftor(path, nullptr));

            // add all the recipes at this path
            for (auto &p: book->recipe_per_name())
                MSS(ftor(path, &p.second));

            // visit the subbooks
            for (auto &p: book->book_per_name())
            {
                path.push_back(&p.second);
                MSS(each_(ftor, path));
                path.pop_back();
            }
            MSS_END();
        }

        Book root_book_;
    };

} } 

#endif
