#include "catch.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Book.hpp"
#include "cook/model/DependencyGraph.hpp"

using namespace cook::model;

namespace  {

bool is_topological_order(std::list<Recipe *> & recipes)
{
    MSS_BEGIN(bool);

    for (auto it1 = recipes.begin(); it1 != recipes.end(); ++it1)
    {
        Recipe * recipe = *it1;
        for(Recipe * dep : recipe->dependencies())
        {
            MSS(!!dep);

            REQUIRE(std::find(recipes.begin(), it1, dep) == it1);

            auto it2 = std::find(it1, recipes.end(), dep);
            REQUIRE(it2 != it1);
            REQUIRE(it2 != recipes.end());
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

Recipe * goc(Book & root, const std::string & str)
{
    Recipe * ptr = nullptr;
    REQUIRE(goc_recipe(ptr, &root, make_uri(str)));
    return ptr;
}

}


TEST_CASE("Dependency resolving", "[ut][algo][dependency_resolving]")
{
    Book root;
    std::list<Recipe *> root_recipes;
    Recipe * subroot = nullptr;

    Scenario scn;

    SECTION("empty set") {}
    SECTION("single element")
    {
        auto * recipe = goc(root, "a");

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
        auto * recipe_a = goc(root, "a");
        auto * recipe_b = goc(root, "b");

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
        auto * recipe_a = goc(root, "a");
        auto * recipe_b = goc(root, "b");
        auto * recipe_c = goc(root, "c");
        auto * recipe_d = goc(root, "d");
        auto * recipe_e = goc(root, "e");

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
            auto * recipe_a = goc(root, "a");
            recipe_a->add_dependency(make_uri("a"));
            root_recipes.push_back(recipe_a);
        }

        SECTION("larger cycle")
        {
            auto * recipe_a = goc(root, "a");
            auto * recipe_b = goc(root, "b");
            auto * recipe_c = goc(root, "c");
            recipe_a->add_dependency(make_uri("b"));
            recipe_b->add_dependency(make_uri("c"));
            recipe_c->add_dependency(make_uri("a"));

            SECTION("a") { root_recipes.push_back(recipe_a); }
            SECTION("b") { root_recipes.push_back(recipe_b); }
            SECTION("c") { root_recipes.push_back(recipe_c); }
        }
    }



    // construct the graph
    cook::model::DependencyGraph graph(&root);
    REQUIRE(graph.construct(gubg::make_range(root_recipes)));

    REQUIRE(graph.all_dependencies_resolved() == scn.dependencies_resolved);

    if (!graph.all_dependencies_resolved())
        return;

    REQUIRE(graph.is_acyclic() == scn.is_dag);
    if (!graph.is_acyclic())
        return;


    // check total topological order
    {
        auto order = graph.topological_order();
        REQUIRE(order.size() == scn.total_topological_order);
        REQUIRE(is_topological_order(order));
    }

    // check sub topological order
    if (subroot)
    {
        std::list<Recipe *> order;
        REQUIRE(graph.topological_order(subroot, order));
        REQUIRE(order.size() == scn.subroot_count);
        REQUIRE(is_topological_order(order));
    }
}
