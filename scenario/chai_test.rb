require("gubg/catch")
include GUBG::Catch

test_case("chai_test") do
    cook_fn = File.expand_path("cook.exe")
    should = nil
    cmd = [cook_fn]
    should = :pass

    recipe_name = nil
    toolchain_name = nil


    section ("flags") { recipe_name = :flags }
    section ("uri") { recipe_name = :uri }
    section ("toolchain element") { toolchain_name = :toolchain_element }
    section ("toolchain") { toolchain_name = :toolchain }

    cmd << "-t scenario/chai_test/#{toolchain_name}.chai" if toolchain_name
    recipe_name = recipe_name || "recipes"
    cmd << "-f scenario/chai_test/#{recipe_name}.chai"

    sh(cmd.flatten*' ') do |ok, res|
        case should
        when :pass then must(ok)
        when :fail then must(!ok)
        else raise("Unknown should #{should}") end
    end
end
