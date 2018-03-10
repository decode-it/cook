require("gubg/catch")
include GUBG::Catch

test_case("hello_world") do
    cook_fn = File.expand_path("cook.exe")
    should = nil
    cmd = [cook_fn]
    cmd << "-v 3"
    section("positive") do
        should = :pass
        section("help"){cmd << "-h"}
        section("input file") do
            cmd << "-f scenario/hello_world/recipes.chai"
            section("recipe /a/b"){cmd << "/a/b"}
        end
    end
    section("negative") do
        should = :fail
        section("input file") do
            cmd << "-f scenario/hello_world/recipes.chai"
            section("recipe /unknown/recipe"){cmd << "/unknown/recipe"}
        end
    end
    sh(cmd.flatten*' ') do |t,args|
        case should
        when :pass then must(t)
        when :fail then must(!t)
        else raise("Unknown should #{should}") end
    end
end
