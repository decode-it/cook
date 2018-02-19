#include "catch.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Book.hpp"
#include "cook/algo/DependencyResolver.hpp"

using namespace cook::model;
using result_type = cook::algo::DependencyResolveResult;

namespace  {

bool is_topological_order(std::vector<Recipe *> & recipes)
{
    MSS_BEGIN(bool);

    for (auto it1 = recipes.begin(); it1 != recipes.end(); ++it1)
    {
        Recipe * recipe = *it1;
        for(const auto & p : recipe->dependencies())
        {
            MSS(!!p.second);
            auto it2 = std::find(recipes.begin(), recipes.end(), p.second);
            MSS(it2 > it1);
            MSS(it2 != recipes.end());
        }
    }

    MSS_END();
}

struct Scenario
{
    result_type expected_result = result_type::Success;
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

            SECTION("no suborder") { scn.subroot_count = 0; subroot = recipe_b; }
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

            SECTION("a") { scn.expected_result = result_type::Unresolved_Dependencies; root_recipes.push_back(recipe_a); }
            SECTION("b") {scn.total_topological_order = 1; root_recipes.push_back(recipe_b); }
            SECTION("c") {scn.total_topological_order = 3; root_recipes.push_back(recipe_c); }
        }
    }

    SECTION("cyclic dependencies")
    {
        auto * recipe_a = goc(root, "a");
        auto * recipe_b = goc(root, "b");
        auto * recipe_c = goc(root, "c");
        recipe_a->add_dependency(make_uri("b"));
        recipe_b->add_dependency(make_uri("c"));
        recipe_c->add_dependency(make_uri("a"));

        SECTION("a") { scn.expected_result = result_type::Cyclic_Dependencies; root_recipes.push_back(recipe_a); }
        SECTION("b") { scn.expected_result = result_type::Cyclic_Dependencies; root_recipes.push_back(recipe_b); }
        SECTION("c") { scn.expected_result = result_type::Cyclic_Dependencies; root_recipes.push_back(recipe_c); }
    }



    // try resolving
    cook::algo::DependencyResolver resolver(&root);
    auto res = resolver.resolve(root_recipes);
    REQUIRE(res == scn.expected_result);

    if (res == result_type::Success)
    {
        // check total topological order
        {
            auto order = resolver.topological_order();
            REQUIRE(order.size() == scn.total_topological_order);
            REQUIRE(is_topological_order(order));
        }

        // check sub topological order
        {
            auto order = resolver.topological_suborder(subroot);
            REQUIRE(order.size() == scn.subroot_count);
            REQUIRE(is_topological_order(order));
        }
    }
}


TEST_CASE("Dependency resolution", "[ut][algo][dependency_resolution]")
{
    struct
    {
        result_type result = result_type::Success;
        std::string dependency;
        Recipe * dep;

    } wanted;

    Book root;

    Recipe * recipe_a = goc(root, "a/b/c/d/e");

    SECTION("circ dependency")
    {
        wanted.dependency = "a/b/c/d/e";
        wanted.result = result_type::Cyclic_Dependencies;
        wanted.dep = recipe_a;
    }

    SECTION("relative local dependency")
    {
        wanted.result = result_type::Success;

        SECTION("same level")
        {
            wanted.dependency = "b";

            SECTION("equilevel")    { wanted.dep = goc(root, "a/b/c/d/b"); }
            SECTION("level-1")      { wanted.dep = goc(root, "a/b/c/b"); }
            SECTION("level-2")      { wanted.dep = goc(root, "a/b/b"); }
            SECTION("root")         { wanted.dep = goc(root, "b"); }
        }

        SECTION("deeper level")
        {
            wanted.dependency = "a/b/c";
            SECTION("equilevel")    { wanted.dep = goc(root, "a/b/c/d/a/b/c"); }
            SECTION("level-1")      { wanted.dep = goc(root, "a/b/c/a/b/c"); }
            SECTION("level-2")      { wanted.dep = goc(root, "a/b/a/b/c"); }
        }
    }

    REQUIRE(recipe_a->add_dependency(make_uri(wanted.dependency)));
    REQUIRE(!recipe_a->add_dependency(make_uri(wanted.dependency)));

    cook::algo::DependencyResolver resolver(&root);
    auto res = resolver.resolve({recipe_a});

    REQUIRE(res == wanted.result);
    if (res == result_type::Success)
    {
        REQUIRE(recipe_a->dependencies().size() == 1);
        REQUIRE(recipe_a->dependencies().begin()->second == wanted.dep);
    }
}
