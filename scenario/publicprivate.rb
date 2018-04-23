require("gubg/catch")
include GUBG::Catch

name = "publicprivate"

test_case(name) do
    cook_fn = File.expand_path("cook.exe")
    should = nil
    cmd = [cook_fn]
    cmd << "-v 3"
    section("positive") do
        should = :pass
        section("input file") do
            cmd << "-f scenario/#{name}/recipes.chai"
            section("recipe /app/exe") do
                cmd << "/app/exe"
            end
        end
    end
    sh(cmd.flatten*' ') do |t,args|
        case should
        when :pass then must(t)
        when :fail then must(!t)
        else raise("Unknown should #{should}") end
    end
end