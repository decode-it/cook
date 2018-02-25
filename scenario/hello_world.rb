require("gubg/catch")
include GUBG::Catch

test_case("hello_world") do
    cook_fn = File.expand_path("b0-cook.exe")
    cmd = [cook_fn]
    section("help"){cmd << "-h"}
    sh(cmd.flatten*' ')
end
