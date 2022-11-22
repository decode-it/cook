require("gubg/catch")
include Gubg::Catch

test_case("regex") do
    cook_fn = File.expand_path("cook.exe")
    puts Dir.pwd
    Dir.chdir("scenario/regex") do
        puts Dir.pwd
        cmd = [cook_fn]
        cmd << "-v 3 -f ./ -g ninja"
        should = nil
        section("pass"){should = :pass; cmd << "/regex/exe_pass"}
        section("fail"){should = :fail; cmd << "/regex/exe_fail"}
        sh(cmd.flatten*' ') { |ok, res| must(ok) }
        sh("ninja") do |ok, res|
            case should
            when :pass then must(ok)
            when :fail then must(!ok)
            else raise("Unknown should #{should}") end
        end
    end
end

