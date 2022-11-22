require("gubg/catch")
include Gubg::Catch

test_case("userdata") do
    cook_fn = File.expand_path("cook.exe")
    sh("#{cook_fn} -f scenario/userdata") do |ok,res|
        must(ok)
    end
end
