require("gubg/catch")
include Gubg::Catch

test_case("bundle") do
	cook_fn = File.expand_path("cook.exe")

	name = nil

    section("app") do
    	name = "app"
    end
    section("app_icns") do
    	name = "app_icns"
    end
    section("app_plist") do
    	name = "app_plist"
    end
    section("app_params") do
    	name = "app_params"
    end
    section("app_resource") do
    	name = "app_resource"
    end

    section("plugin") do
    	name = "plugin"
    end
    section("plugin_ext") do
    	name = "plugin_ext"
    end

    Dir.chdir("scenario/bundle") do
    	FileUtils.rm_f("CMakeLists.txt")
    	sh("#{cook_fn} -g cmake -f #{name}.chai")
    	FileUtils.cp("CMakeLists.txt", "CMakeLists-#{name}.txt")

    	cmake_build = "cmake_build-#{name}"
    	FileUtils.rm_rf(cmake_build)

        if Gubg.os == :macos
           sh("cmake -S . -B #{cmake_build} -G Xcode")
           case :xcode
           when :xcode then sh("xcodebuild -project #{cmake_build}/*.xcodeproj")
           when :cmake then sh("cmake --build #{cmake_build}")
           end
       end
   end
end
