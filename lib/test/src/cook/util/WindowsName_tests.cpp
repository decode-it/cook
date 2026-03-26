#include "catch.hpp"
#include "cook/util/WindowsName.hpp"

TEST_CASE("Windows naming helpers", "[ut][util][windows_name]")
{
    SECTION("fragment sanitization is stable and bounded")
    {
        REQUIRE(cook::util::sanitize_fragment("api-query.c") == "api_query_c");
        REQUIRE(cook::util::sanitize_fragment("___", 16) == "x");
        REQUIRE(cook::util::sanitize_fragment("ABCDEFGHIJKLMNOPQRSTUVWXYZ", 8) == "abcdefgh");
    }

    SECTION("rule names are shortened deterministically")
    {
        const auto a = cook::util::shortened_rule_name("api/static/a3deng2/n0", 0);
        const auto b = cook::util::shortened_rule_name("api/static/a3deng2/n0", 0);
        const auto c = cook::util::shortened_rule_name("api/static/a3deng2/n0", 1);

        REQUIRE(a == b);
        REQUIRE(a != c);
        REQUIRE(a.find("cmd_") == 0);
        REQUIRE(a.size() <= 40);
    }

    SECTION("object paths distinguish identical basenames from different directories")
    {
        const cook::model::Uri recipe("/api/static/demo");
        const auto a = cook::util::shortened_object_relpath(recipe, "src/a", "src/a/api_query.c", "src/a/api_query.c.obj");
        const auto b = cook::util::shortened_object_relpath(recipe, "src/b", "src/b/api_query.c", "src/b/api_query.c.obj");

        REQUIRE(a != b);
        REQUIRE(a.extension() == ".obj");
        REQUIRE(a.filename().string().find("api_query_") == 0);
        REQUIRE(a.parent_path().filename().string().size() == 16);
    }
}
