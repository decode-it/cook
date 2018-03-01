#include "catch.hpp"
#include "cook/model/GlobInfo.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Book.hpp"
#include "cook/process/souschef/Resolver.hpp"

#define BASE_DIR "generated/"

namespace  {



void create_files()
{
    std::filesystem::create_directories(BASE_DIR "subdir");
    std::ofstream(BASE_DIR "file1.cpp");
    std::ofstream(BASE_DIR "file1.hpp");
    std::ofstream(BASE_DIR "other.txt");
    std::ofstream(BASE_DIR "subdir/file2.cpp");
    std::ofstream(BASE_DIR "subdir/file2.hpp");
}

void clear_files()
{
    std::filesystem::remove_all(BASE_DIR);
}

struct Scenario
{
    std::set<std::string>               expected_files;
    std::optional<cook::Language>       language;
    std::optional<cook::Type>           type;
    std::optional<cook::Overwrite>      overwrite;
    std::optional<cook::Propagation>    propagation;
    bool block_all_filter = false;
};

template <typename T>
bool check_if_equal(std::optional<T> t, T value, T unset_value)
{
    MSS_BEGIN(bool);

    bool optional_set = t.has_value();
    bool value_set = (value != unset_value);

    MSS(optional_set == value_set);
    if (t)
        MSS(*t == value);
    MSS_END();
}

}



TEST_CASE("glob resolve tests", "[ut][glob]")
{
    create_files();

    cook::model::GlobInfo globber;

    Scenario scn;


    SECTION("dir/pattern tests")
    {
        SECTION("dir as base dir")
        {
            globber.dir = BASE_DIR;



            SECTION("all files")
            {
                globber.pattern = "**";
                scn.expected_files = {"subdir", "file1.cpp", "file1.hpp", "other.txt", "subdir/file2.cpp", "subdir/file2.hpp"};
            }

            SECTION("non-recursive files")
            {
                globber.pattern = "*";
                scn.expected_files = {"subdir", "file1.cpp", "file1.hpp", "other.txt"};
            }

            SECTION("extension files")
            {
                globber.pattern = "**.[hc]pp";
                scn.expected_files = {"file1.cpp", "file1.hpp", "subdir/file2.cpp", "subdir/file2.hpp"};
            }

            SECTION("non-recursive, extension files")
            {
                globber.pattern = "*.[hc]pp";
                scn.expected_files = {"file1.cpp", "file1.hpp"};
            }

            SECTION("subdir files")
            {
                globber.pattern = "subdir/*.[hc]pp";
                scn.expected_files = {"subdir/file2.cpp", "subdir/file2.hpp"};
            }

            SECTION("non-matching pattern")
            {
                globber.pattern = "*.c";
            }
        }
        SECTION("subdir")
        {
            globber.dir = BASE_DIR "subdir";
            globber.pattern = "*";
            scn.expected_files = {"file2.cpp", "file2.hpp"};
        }

        SECTION("non-existing dir")
        {
            globber.dir = BASE_DIR "/tralalal";
        }
    }

    SECTION("properties")
    {
        globber.dir = BASE_DIR;
        globber.pattern = "**.[hc]pp";
        scn.expected_files = {"file1.cpp", "file1.hpp", "subdir/file2.cpp", "subdir/file2.hpp"};

        SECTION("specifying language")      { scn.language = cook::Language::CXX; }
        SECTION("specifying type")          { scn.type = cook::Type::Source; }
        SECTION("specifying overwrite")     { scn.overwrite = cook::Overwrite::Always; }
        SECTION("specifying propagation")   { scn.propagation = cook::Propagation::Public; }
        SECTION("blocking everything")      { scn.block_all_filter = true; scn.expected_files.clear(); }
    }


    {
        {
            if (scn.language) globber.language = *scn.language;
            if (scn.type) globber.type= *scn.type;
            if (scn.overwrite) globber.overwrite = *scn.overwrite;
            if (scn.propagation) globber.propagation = *scn.propagation;
        }

        const static cook::Language my_own_language = cook::Language::UserDefined;

        std::set<std::string> files;

        globber.filter_and_adaptor = [&](cook::LanguageTypePair & key, cook::ingredient::File & f)
        {
            REQUIRE(check_if_equal(scn.language, key.language, cook::Language::Undefined));
            REQUIRE(check_if_equal(scn.type, key.type, cook::Type::Undefined));
            REQUIRE(check_if_equal(scn.overwrite, f.overwrite(), cook::Overwrite::Never));
            REQUIRE(check_if_equal(scn.propagation, f.propagation(), cook::Propagation::Private));

            if (scn.block_all_filter)
                return false;

            files.insert(f.rel());
            return true;
        };


        // test the actual resolving
        {
            auto p = cook::model::Uri::recipe_uri("test");
            REQUIRE(p.second);

            cook::model::Book book;
            cook::model::Recipe & recipe = book.goc_recipe(*p.first.name());

            recipe.set_working_directory(std::filesystem::current_path());

            cook::rules::RuleSet ruleset;
            cook::process::souschef::Resolver resolver(&ruleset);

            REQUIRE(resolver.process_one(recipe, globber));
        }

        REQUIRE(files == scn.expected_files);
    }

    clear_files();

}
