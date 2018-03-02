#include "catch.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Book.hpp"
#include "cook/model/Library.hpp"
#include "cook/process/Menu.hpp"

using namespace cook::model;

namespace  {

bool is_topological_order(std::list<Recipe *> & recipes)
{
    MSS_BEGIN(bool);

    for (auto recipe_it = recipes.begin(); recipe_it != recipes.end(); ++recipe_it)
    {
        Recipe * recipe = *recipe_it;
        for(Recipe * dep : recipe->dependencies())
        {
            MSS(!!dep);

            // find the dependency, should be earlier in the list
            auto dep_it = std::find(recipes.begin(), recipe_it, dep);
            REQUIRE(dep_it != recipe_it);

            // and not llater
            REQUIRE(std::find(recipe_it, recipes.end(), dep) == recipes.end());
        }
    }

    MSS_END();
}

struct Scenario
{
    bool dependencies_resolved = true;
    bool is_dag = true;

    unsigned int total_topological_order = 0;
    unsigned int subroot_count = 0;
};

Uri make_uri(const std::string & str)
{
    std::pair<Uri, bool> p = Uri::recipe_uri(str);
    REQUIRE(p.second);

    return p.first;
}

Recipe * goc(Library & lib, const std::string & str)
{
    Recipe * ptr = nullptr;
    REQUIRE(lib.goc_recipe(ptr, make_uri(str)));
    return ptr;
}

}


TEST_CASE("Dependency resolving", "[ut][algo][dependency_resolving]")
{
    Library lib;
    std::list<Recipe *> root_recipes;
    Recipe * subroot = nullptr;

    Scenario scn;

        SECTION("empty set") {}
        SECTION("single element")
        {
            auto * recipe = goc(lib, "a");

            SECTION("in root")
            {
                root_recipes.push_back(recipe);
                scn.total_topological_order = 1;

                SECTION("no suborder") { }
                SECTION("suborder") { scn.subroot_count = 1; subroot = recipe; }
            }

            SECTION("outside of root") { }
        }

        SECTION("two unconnected elements")
        {
            auto * recipe_a = goc(lib, "a");
            auto * recipe_b = goc(lib, "b");

            SECTION("single root")
            {
                root_recipes.push_back(recipe_a);
                scn.total_topological_order = 1;

                SECTION("suborder") { scn.subroot_count = 1; subroot = recipe_a; }
            }

            SECTION("both root")
            {
                root_recipes.push_back(recipe_a);
                root_recipes.push_back(recipe_b);
                scn.total_topological_order = 2;

                SECTION("suborder a") { scn.subroot_count = 1; subroot = recipe_a; }
                SECTION("suborder b") { scn.subroot_count = 1; subroot = recipe_b; }
            }
        }

    SECTION("simple DAG")
    {
        auto * recipe_a = goc(lib, "a");
        auto * recipe_b = goc(lib, "b");
        auto * recipe_c = goc(lib, "c");
        auto * recipe_d = goc(lib, "d");
        auto * recipe_e = goc(lib, "e");

        recipe_a->add_dependency(make_uri("b"));
        recipe_a->add_dependency(make_uri("c"));
        recipe_c->add_dependency(make_uri("b"));
        recipe_c->add_dependency(make_uri("d"));
        recipe_e->add_dependency(make_uri("a"));

        SECTION("single root")
        {
            SECTION("a") {scn.total_topological_order = 4; root_recipes.push_back(recipe_a); }
            SECTION("b") {scn.total_topological_order = 1; root_recipes.push_back(recipe_b); }
            SECTION("c") {scn.total_topological_order = 3; root_recipes.push_back(recipe_c); }
            SECTION("d") {scn.total_topological_order = 1; root_recipes.push_back(recipe_d); }
            SECTION("e") {scn.total_topological_order = 5; root_recipes.push_back(recipe_e); }
        }

        SECTION("unresolved dependencies")
        {
            recipe_a->add_dependency(make_uri("q"));

            SECTION("a") { scn.dependencies_resolved = false; root_recipes.push_back(recipe_a); }
            SECTION("b") {scn.total_topological_order = 1; root_recipes.push_back(recipe_b); }
            SECTION("c") {scn.total_topological_order = 3; root_recipes.push_back(recipe_c); }
        }
    }

    SECTION("cyclic dependencies")
    {
        scn.is_dag = false;

        SECTION("single cycle")
        {
            auto * recipe_a = goc(lib, "a");
            recipe_a->add_dependency(make_uri("a"));
            root_recipes.push_back(recipe_a);
        }

        SECTION("larger cycle")
        {
            auto * recipe_a = goc(lib, "a");
            auto * recipe_b = goc(lib, "b");
            auto * recipe_c = goc(lib, "c");
            recipe_a->add_dependency(make_uri("b"));
            recipe_b->add_dependency(make_uri("c"));
            recipe_c->add_dependency(make_uri("a"));

            SECTION("a") { root_recipes.push_back(recipe_a); }
            SECTION("b") { root_recipes.push_back(recipe_b); }
            SECTION("c") { root_recipes.push_back(recipe_c); }
        }
    }



    // resolve the dependencies
    {
        bool all_resolved = false;
        REQUIRE(lib.resolve(&all_resolved));

        if (!scn.dependencies_resolved)
            REQUIRE(!all_resolved);
    }

    cook::process::Menu menu;
    cook::Result rc = menu.construct(gubg::make_range(root_recipes));

    if (false) {}
    else if (!scn.dependencies_resolved)
        REQUIRE(!rc);
    else if (!scn.is_dag)
        REQUIRE(!rc);
    else
        REQUIRE(rc);


    if (rc)
    {

        // check total topological order
        {
            auto order = menu.topological_order();
            REQUIRE(order.size() == scn.total_topological_order);
            REQUIRE(is_topological_order(order));
        }

        /*// check sub topological order
        if (subroot)
        {
            std::list<Recipe *> order;
            REQUIRE(graph.topological_order(subroot, order));
            REQUIRE(order.size() == scn.subroot_count);
            REQUIRE(is_topological_order(order));
        }*/
    }
}
