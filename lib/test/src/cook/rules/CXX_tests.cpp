#include "catch.hpp"
#include "cook/rules/CXX.hpp"
#include "cook/model/Recipe.hpp"
using namespace cook;

TEST_CASE("cook::rules::CXX::type_from_extension tests", "[ut][rules][CXX][type_from_extension]")
{
    struct Scn
    {
        std::string extension;
        Type type = Type::Undefined;
    };
    Scn scn;

    struct Should
    {
        bool be_valid = false;
        Type type = Type::Undefined;
    };
    Should should;

    SECTION("positive")
    {
        should.be_valid = true;

        SECTION("type not known upfront")
        {
            SECTION("source")
            {
                should.type = Type::Source;
                SECTION(".cpp") { scn.extension = ".cpp"; }
                SECTION(".cxx") { scn.extension = ".cxx"; }
                SECTION(".CPP") { scn.extension = ".CPP"; }
                SECTION(".CXX") { scn.extension = ".CXX"; }
            }
            SECTION("header")
            {
                should.type = Type::Header;
                SECTION(".hpp") { scn.extension = ".hpp"; }
                SECTION(".hxx") { scn.extension = ".hxx"; }
                SECTION(".HPP") { scn.extension = ".HPP"; }
                SECTION(".HXX") { scn.extension = ".HXX"; }
            }
            SECTION("object")
            {
                should.type = Type::Object;
                SECTION(".o") { scn.extension = ".o"; }
                SECTION(".a") { scn.extension = ".a"; }
                SECTION(".lib") { scn.extension = ".lib"; }
            }
        }
        SECTION("type already known upfront")
        {
            scn.type = Type::Source;
            should.type = scn.type;
        }

        REQUIRE(should.type != Type::Undefined);
    }
    SECTION("negative")
    {
        should.be_valid = false;

        SECTION("default") { }
        SECTION("unknown type") { scn.extension = ".unkown"; }
    }

    auto result = rules::CXX::type_from_extension(scn.extension, scn.type);

    REQUIRE(!!result == should.be_valid);
    if (should.be_valid)
    {
        REQUIRE(*result == should.type);
    }
}

namespace  {

    struct Scenario
    {
        std::filesystem::path file_to_create;

        std::filesystem::path rel;
        LanguageTypePair key;

        bool accepts = false;

        struct {
            std::optional<Language> language;
            std::optional<Type> type;
            std::optional<Overwrite> overwrite;
            std::optional<Propagation> propagation;
        } resolved;

        unsigned int num_added_files = 0;

        std::function<void (const model::Recipe &)> additional_check;
    };

}

TEST_CASE("CXX glob rules tests", "[ut][glob][CXX]")
{
    rules::CXX cxx;
    REQUIRE(cxx.language() == Language::CXX);

    Scenario scn;

    SECTION("accepts")
    {
        scn.accepts = true;
        scn.resolved.language = Language::CXX;

        SECTION("source file")
        {
            SECTION(".cpp") { scn.file_to_create = "test.cpp"; }
            SECTION(".cxx") { scn.file_to_create = "test.cxx"; }

            scn.resolved.type = Type::Source;
            scn.resolved.overwrite = Overwrite::Never;
            scn.resolved.propagation = Propagation::Private;
            scn.num_added_files = 1;
        }

        SECTION("header file")
        {
            SECTION(".hpp") { scn.file_to_create = "test.hpp"; }
            SECTION(".hxx") { scn.file_to_create = "test.hxx"; }

            scn.resolved.type = Type::Header;
            scn.resolved.overwrite = Overwrite::Never;
            scn.resolved.propagation = Propagation::Private;
            scn.num_added_files = 2;

            scn.additional_check = [](const model::Recipe & recipe) {
                auto ptr = recipe.pre().find((LanguageTypePair(Language::CXX, Type::IncludePath)), "./");

                REQUIRE(ptr);
                REQUIRE(ptr->propagation() == Propagation::Public);
            };
        }

        SECTION("object file")
        {
            SECTION(".o") { scn.file_to_create = "test.o"; }

            scn.resolved.type = Type::Object;
            scn.resolved.overwrite = Overwrite::Never;
            scn.resolved.propagation = Propagation::Private;
            scn.num_added_files = 2;

            scn.additional_check = [](const model::Recipe & recipe) {
                auto ptr = recipe.pre().find((LanguageTypePair(Language::CXX, Type::LibraryPath)), "./");

                REQUIRE(ptr);
                REQUIRE(ptr->propagation() == Propagation::Private);
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
    auto p = model::Uri::recipe_uri("test");
    REQUIRE(p.second);
    model::Recipe recipe(p.first);

    {
        // check the accept function
        LanguageTypePair key = scn.key;
        property::File file("./", scn.rel);

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

