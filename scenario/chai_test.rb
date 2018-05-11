require("gubg/catch")
include GUBG::Catch

test_case("chai_test") do
    cook_fn = File.expand_path("cook.exe")
    should = nil
    cmd = [cook_fn]
    should = :pass

    name = nil


    section ("flags") { name = :flags }
    section ("uri") { name = :uri }

    cmd << "-f scenario/chai_test/#{name}.chai"

    sh(cmd.flatten*' ') do |ok, res|
        case should
        when :pass then must(ok)
        when :fail then must(!ok)
        else raise("Unknown should #{should}") end
    end
end
