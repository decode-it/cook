begin
    require_relative("gubg.build/bootstrap.rb")
rescue LoadError
    puts("This seems to be a fresh clone: I will update all required submodules for you.")
    sh "git submodule update --init"
    sh "rake uth prepare"
    retry
end
require("gubg/shared")
require_relative("extern/boost/boost.rb")

task :default do
    sh "rake -T"
end

gubg_submods = %w[build std math io algo chaiscript].map{|e|"gubg.#{e}"}

desc "Prepare the submods"
task :prepare do
    Rake::Task["boost:update"].invoke

    GUBG::each_submod(submods: gubg_submods) do |info|
        sh "rake prepare"
    end
end
task :run

namespace :setup do
    desc "Setup for ubuntu"
    task :ubuntu do
        #We rely on ninja
        sh "sudo apt install ninja-build"
        #Fixes problems with #including bits/c++config.h
        sh "sudo apt install gcc-multilib g++-multilib"
        Rake::Task["gubg:run"].invoke
    end
end

desc "Update submodules to head"
task :uth => ["boost:load", "boost:update"] do
    GUBG::each_submod(submods: gubg_submods) do |info|
        sh "git checkout #{info[:branch]}"
        sh "git pull --rebase"
    end
end

task :update => :uth

#Bootstrap level 0: uses manually updated ninja scripts
namespace :b0 do
    def b0_ninja_fn()
        case GUBG::os
        when :linux then "build/b0/linux/gcc.ninja"
        when :windows then "build/b0/windows/msvc.ninja"
        when :osx then "build/b0/osx/clang.ninja"
        end
    end
    
    desc "bootstrap-level0: generater the ninja scripts (depends on gubg.build)"
    task :generate => "boost:load" do
        require("gubg/build/expand_templates")
        GUBG::Build::expand_templates("build/b0/compile.ninja")
    end


    desc "bootstrap-level0: Build and run the unit tests"
    task :ut, [:filter] do |t,args|
        filter = args[:filter] || "ut"
        filter = filter.split(":").map{|e|"[#{e}]"}*""
        sh("ninja -f #{b0_ninja_fn} -v b0-unit_tests.exe")
        sh("./b0-unit_tests.exe -a -d yes #{filter}")
    end

    desc "bootstrap-level0: Build b0-cook.exe"
    task :build => :generate do
        sh("ninja -f #{b0_ninja_fn} b0-cook.exe -v")
    end
    
    task :install => :build do
        case GUBG::os
        when :linux then sh("sudo cp b0-cook.exe /usr/local/bin/cook")
        when :windows then cp("b0-cook.exe", "../bin/")
        end
    end

    desc "bootstrap-level0: Clean"
    task :clean do
        sh("ninja -f #{b0_ninja_fn} -t clean")
        rm_rf(".b0")
    end

    desc "bootstrap-level0: Update rtags"
    task :rtags do
        puts "Make sure you have \"rdm &\" running somewhere."
        sh "ninja -f #{b0_ninja_fn} -t compdb compile > compile_commands.json"
        sh("rc -J"){}
    end

    desc "bootstrap-level0: Update rtags"
    task :dia => [:rtags, :build, :ut]
end
#Bootstrap level 1: uses output from bootstrap level 0
namespace :b1 do
    desc "bootstrap-level1: Build and run the unit tests"
    task :ut do
        sh("ninja -f #{b0_ninja_fn} -v unit_tests.exe")
        sh("./unit_tests.exe -a -d yes")
    end

    desc "bootstrap-level1: Build b1-cook.exe using b0-cook.exe"
    task :build => ["b0:build"] do
        odir = File.join("build", "b1", "ninja")
        tdir = File.join(".cook", "b1", "ninja")
        
        sh "./b0-cook.exe -f ./ -g ninja -o #{odir} -O #{tdir} cook/app/exe"
        sh "ninja -f #{odir}/build.ninja"
        cp "#{odir}/cook.app.exe", "b1-cook.exe"
    end
    
    desc "bootstrap-level1-cmake: Build b1-cook.exe using b0-cook.exe"
    task :build_cmake => ["b0:update", "b0:build"] do
        odir = File.join("build", "b1", "cmake")
        tdir = File.join(".cook", "b1", "cmake")
        
        sh "./b0-cook.exe -f ./ -g CMake -o #{odir} -O #{tdir} cook/app/exe"
        Dir::chdir(odir) do
            sh "cmake ./"
            sh "make -j8"
        end
        cp "#{odir}/cook_app_exe", "b1-cook.exe"
        
        
    end

    desc "bootstrap-level1: Clean"
    task :clean do
    end
end

desc "Build cook.exe"
task :build => "b1:build" do
    cp "b1-cook.exe", "cook.exe"
end

desc "Clean"
task :clean do
    rm(FileList.new("**/*.obj"))
    rm(FileList.new("*.exe"))
    rm_f(FileList.new("compile_commands.json"))
    Rake::Task["b0:clean"].invoke
    Rake::Task["b1:clean"].invoke
end

desc "Install"
task :install, [:path] => "build" do |task, args|
    path = args[:path]
    if path
        GUBG::mkdir(path)
        sh("cp cook.exe #{path}")
    else
        case GUBG::os
        when :linux then sh("sudo cp cook.exe /usr/local/bin/cook")
        when :windows then cp("cook.exe", "../bin/")
        end
    end
end

namespace :doc do
    mdbook_exe = nil
    task :setup do
        mdbook_exe = File.join(ENV["HOME"], ".cargo", "bin", "mdbook")
        if !File.exists?(mdbook_exe)
            sh "yaourt rustup"
            sh "rustup install stable"
            sh "rustup default stable"
            sh "cargo install mdbook"
        end
    end
    desc "Manual documentation"
    task :manual => :setup do
        Dir.chdir("doc/manual") do
            sh(mdbook_exe, "serve")
        end
    end
    desc "Desgin documentation"
    task :design => :setup do
        Dir.chdir("doc/design") do
            sh(mdbook_exe, "serve")
        end
    end
    desc "Publish the static pages on the web"
    task :publish, [:filter] do |t,args|
        infos = {
            site: {src: "site", dst: ""},
            doc:  {src: "doc",  dst: "doc"},
        }
        infos.each do |name,info|
            name = name.to_s
            puts("[#{name}](src:#{info[:src]})(dst:#{info[:dst]})")
            Dir.chdir(info[:src]) do
                sh "scp -r * web-gfannes@ssh.linuxsystems.be:fannes.com/cook/#{info[:dst]}"
            end if args[:filter] == name
        end
    end
end
task :old_doc do
    Dir.chdir("doc") do
        %w[abc complex].each do |name|
            sh "dot -T png -o #{name}.png #{name}.dot"
            sh "mimeopen #{name}.png"
        end
    end
end

desc "Test"
task :test, [:filter] => "build" do |t,args|
    FileList.new("scenario/*.rb").each{|fn|require_relative(fn)}
    GUBG::Catch::run(args[:filter])
end
