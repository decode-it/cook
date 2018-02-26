require("gubg/catch")
include GUBG::Catch

test_case("userdata") do
    cook_fn = File.expand_path("cook.exe")
    sh("#{cook_fn} -f scenario/userdata") do |t,args|
        must(t)
    end
end
