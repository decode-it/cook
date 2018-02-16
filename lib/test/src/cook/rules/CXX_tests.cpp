#include "catch.hpp"
#include "cook/rules/CXX.hpp"
#include "cook/model/Recipe.hpp"

namespace  {

struct Scenario
{
    std::filesystem::path file_to_create;

    std::filesystem::path rel;
    cook::LanguageTypePair key;

    bool accepts = false;

    struct {
        std::optional<cook::Language> language;
        std::optional<cook::Type> type;
        std::optional<cook::Overwrite> overwrite;
        std::optional<cook::Propagation> propagation;
    } resolved;

    unsigned int num_added_files = 0;

    std::function<void (const cook::model::Recipe &)> additional_check;
};

}

TEST_CASE("CXX glob rules tests", "[ut][glob][cxx]")
{
    cook::rules::CXX cxx;
    REQUIRE(cxx.language() == cook::Language::CXX);

    Scenario scn;

    SECTION("accepts")
    {
        scn.accepts = true;
        scn.resolved.language = cook::Language::CXX;

        SECTION("source file")
        {
            SECTION(".cpp") { scn.file_to_create = "test.cpp"; }
            SECTION(".cxx") { scn.file_to_create = "test.cxx"; }

            scn.resolved.type = cook::Type::Source;
            scn.resolved.overwrite = cook::Overwrite::Never;
            scn.resolved.propagation = cook::Propagation::Private;
            scn.num_added_files = 1;
        }

        SECTION("header file")
        {
            SECTION(".hpp") { scn.file_to_create = "test.hpp"; }
            SECTION(".hxx") { scn.file_to_create = "test.hxx"; }

            scn.resolved.type = cook::Type::Header;
            scn.resolved.overwrite = cook::Overwrite::Never;
            scn.resolved.propagation = cook::Propagation::Private;
            scn.num_added_files = 2;

            scn.additional_check = [](const cook::model::Recipe & recipe) {
                auto ptr = recipe.pre().find((cook::LanguageTypePair(cook::Language::CXX, cook::Type::IncludePath)), "./");

                REQUIRE(ptr);
                REQUIRE(ptr->propagation() == cook::Propagation::Public);
            };
        }

        SECTION("object file")
        {
            SECTION(".o") { scn.file_to_create = "test.o"; }

            scn.resolved.type = cook::Type::Object;
            scn.resolved.overwrite = cook::Overwrite::Never;
            scn.resolved.propagation = cook::Propagation::Private;
            scn.num_added_files = 2;

            scn.additional_check = [](const cook::model::Recipe & recipe) {
                auto ptr = recipe.pre().find((cook::LanguageTypePair(cook::Language::CXX, cook::Type::LibraryPath)), "./");

                REQUIRE(ptr);
                REQUIRE(ptr->propagation() == cook::Propagation::Private);
            };
        }



        scn.rel = scn.file_to_create;
    }

    bool create_file = !scn.file_to_create.empty();
    const std::string & str = scn.file_to_create.string();


    // create file if necessary
    if (create_file)
        std::ofstream ofs(str.c_str());

    // create the recipe
    auto p = cook::model::Uri::recipe_uri("test");
    REQUIRE(p.second);
    cook::model::Recipe recipe(p.first);

    {
        // check the accept function
        cook::LanguageTypePair key = scn.key;
        cook::property::File file("./", scn.rel);

        bool does_accept = cxx.accepts_file(key, file);
        REQUIRE(does_accept == scn.accepts);

        // if accept, then continue
        if (does_accept)
        {
            REQUIRE(cxx.resolve_file(key, file));

            if (scn.resolved.language)      REQUIRE(key.language == *scn.resolved.language);
            if (scn.resolved.type)          REQUIRE(key.type == *scn.resolved.type);
            if (scn.resolved.overwrite)     REQUIRE(file.overwrite() == *scn.resolved.overwrite);
            if (scn.resolved.propagation)   REQUIRE(file.propagation() == *scn.resolved.propagation);

            REQUIRE(cxx.add_file(recipe, key, file));


            {
                unsigned int count = 0;
                REQUIRE(recipe.each_file([&](const auto & key, const auto & file){
                    ++count; return true;
                }));

                REQUIRE(count == scn.num_added_files);
            }


            if (scn.additional_check)
                scn.additional_check(recipe);
        }
    }

    // remove file if created
    if (create_file)
        std::filesystem::remove(scn.file_to_create);
}

