require("gubg/catch")
include Gubg::Catch

test_case("app_arch_func") do
    cook_fn = File.expand_path("cook.exe")
    should = nil
    cmd = [cook_fn]
    # cmd << "-v 3"
    section("positive") do
        should = :pass
        cmd << "-f scenario/app_arch_func/recipes.chai"
        cmd << "-o scenario/app_arch_func"
        section("ninja") do
            cmd << "-g ninja"
        end
        section("cmake") do
            cmd << "-g cmake"
        end
    end
    sh(cmd.flatten*' ') do |ok, res|
        case should
        when :pass then must(ok)
        when :fail then must(!ok)
        else raise("Unknown should #{should}") end
    end
end
