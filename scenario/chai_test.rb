require("gubg/catch")
include Gubg::Catch

test_case("chai_test") do
    cook_fn = File.expand_path("cook.exe")
    should = nil
    cmd = [cook_fn]
    should = :pass

    recipe_name = nil
    toolchain_name = nil
    target = nil


    section ("flags")               { recipe_name = :flags }
    section ("uri")                 { recipe_name = :uri }
    section ("ingredient")          { recipe_name = :ingredient }
    section ("recipe") do
    
        recipe_name = :recipe 
        section("fail") do 
        
            section("everything") {  }
            section("test22") { target = "test22" }
            should = :fail
        end
        section("succeed") do
            target = "test" 
            should = :pass
        end
    end
    section ("toolchain element")   { toolchain_name = :toolchain_element }
    section ("toolchain")           { toolchain_name = :toolchain }
    section ("target_namer")        { recipe_name = :target_namer }

    cmd << "-t scenario/chai_test/#{toolchain_name}.chai" if toolchain_name
    recipe_name = recipe_name || "recipes"
    cmd << "-f scenario/chai_test/#{recipe_name}.chai"
    cmd << target if target

    sh(cmd.flatten*' ') do |ok, res|
        case should
        when :pass then must(ok)
        when :fail then must(!ok)
        else raise("Unknown should #{should}") end
    end
end
