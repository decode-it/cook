test_case("toolchain") do
    cook_fn = File.expand_path("cook.exe")
    should = nil
    cmd = [cook_fn]
    cmd << "-f scenario/hello_world/recipes.chai"
    should = :pass

    sh(cmd.flatten*' ') do |ok, res|
        case should
        when :pass then must(ok)
        when :fail then must(!ok)
        else raise("Unknown should #{should}") end
    end
end
