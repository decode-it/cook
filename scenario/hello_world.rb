require("gubg/catch")
include Gubg::Catch

test_case("hello_world") do
    cook_fn = File.expand_path("cook.exe")
    should = nil
    cmd = [cook_fn]
    cmd << "-v 3"
    section("negative") do
        should = :fail
        section("input file") do
            cmd << "-f scenario/hello_world/recipes.chai"
            section("recipe /unknown/recipe"){cmd << "/unknown/recipe"}
        end
    end
    section("positive") do
        should = :pass
        section("help"){cmd << "-h"}
        section("input file") do
            cmd << "-f scenario/hello_world/recipes.chai"
            section("recipe /book/main") do
                section("naft"){cmd << "-g naft"}
                section("naft"){cmd << "-g CMake"}
                section("naft"){cmd << "-g ninja"}
                cmd << "/book/main"
            end
        end
    end
    sh(cmd.flatten*' ') do |ok, res|
        case should
        when :pass then must(ok)
        when :fail then must(!ok)
        else raise("Unknown should #{should}") end
    end
end
