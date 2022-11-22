require("gubg/catch")
include Gubg::Catch

test_case("globbing_resolving") do
    cook_fn = File.expand_path("cook.exe")
    should = nil
    cmd = [cook_fn]
    section("positive") do
        should = :pass
        cmd << "-f scenario/globbing_resolving/recipes.chai -g ninja -o scenario/build"
    end
    sh(cmd.flatten*' ') do |ok, res|
        case should
        when :pass 
            then must(ok)
            sh("ninja -f scenario/build/build.ninja")
        when :fail 
            then must(!ok)
        else raise("Unknown should #{should}") end
    end

end
