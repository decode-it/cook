begin
    require_relative("gubg.build/bootstrap.rb")
rescue LoadError
    puts("This seems to be a fresh clone: I will update all required submodules for you.")
    sh "git submodule update --init"
    sh "rake uth prepare"
    retry
end
require("gubg/shared")

task :default do
    sh "rake -T"
end

submods = %w[build std math io algo chaiscript].map{|e|"gubg.#{e}"}

desc "Prepare the submods"
task :prepare do
    GUBG::each_submod(submods: submods) do |info|
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
task :uth do
    GUBG::each_submod(submods: submods) do |info|
        sh "git checkout #{info[:branch]}"
        sh "git pull --rebase"
    end
end

task :update => :uth

#Bootstrap level 0: uses manually updated ninja scripts
namespace :b0 do
    def b0_ninja_fn()
        case GUBG::os
        when :linux then "b0-gcc.ninja"
        when :windows then "b0-msvc.ninja"
        end
    end

    desc "bootstrap-level0: Update the ninja scripts (depends on gubg.build)"
    task :update do
        require("gubg/build/expand_templates")
        GUBG::Build::expand_templates("b0-compile.ninja")
    end

    desc "bootstrap-level0: Build and run the unit tests"
    task :ut, [:filter] do |t,args|
        filter = args[:filter] || "ut"
        filter = filter.split(":").map{|e|"[#{e}]"}*""
        sh("ninja -f #{b0_ninja_fn} -v b0-unit_tests.exe")
        sh("./b0-unit_tests.exe -a -d yes #{filter}")
    end

    desc "bootstrap-level0: Build b0-cook.exe"
    task :build do
        sh("ninja -f #{b0_ninja_fn} b0-cook.exe -v")
    end

    desc "bootstrap-level0: Clean"
    task :clean do
        sh("ninja -f #{b0_ninja_fn} -t clean")
    end

    desc "bootstrap-level0: Update rtags"
    task :rtags do
        puts "Make sure you have \"rdm &\" running somewhere."
        sh "ninja -f #{b0_ninja_fn} -t compdb compile > compile_commands.json"
        sh("rc -J"){}
    end

    desc "bootstrap-level0: Update rtags"
    task :dia => [:update, :rtags, :build, :ut]
end
#Bootstrap level 1: uses output from bootstrap level 0
namespace :b1 do
    desc "bootstrap-level1: Build and run the unit tests"
    task :ut do
        sh("ninja -f #{b0_ninja_fn} -v unit_tests.exe")
        sh("./unit_tests.exe -a -d yes")
    end

    desc "bootstrap-level1: Build b1-cook.exe using b0-cook.exe"
    task :build => "b0:build" do
        sh "./b0-cook.exe"
        sh "ninja -v"
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
    rm(FileList.new("compile_commands.json"))
    Rake::Task["b0:clean"].invoke
    Rake::Task["b1:clean"].invoke
end

desc "Install"
task :install, [:bin] => "build" do |task, args|
    bin = args[:bin]
    if bin
        GUBG::mkdir(bin)
        sh("cp cook.exe #{bin}")
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
task :test, [:filter] do |t,args|
    filter = args[:filter]||""
    re = ""
    filter.each_char{|ch|re << ".*#{ch}"}
    re << ".*"
    re = Regexp.new(re)
    Rake::Task["b0:build"].invoke
    cook_exe = File.expand_path("b0-cook.exe")
    scns_per_dir = {
        userdata: {name: "normal", should: :succeed},
        hello_world: [
            {name: "help", help: "", should: :succeed},
            {name: "normal",                                   recipe: "/a/b",   should: :succeed},
            {name: "using file",        input: "recipes.chai", recipe: "/a/b",   should: :succeed},
            {name: "unexisting recipe",                        recipe: "/a/b/c", should: :fail},
        ]
    }
    summary = Hash.new{|h,k|h[k]=0}
    puts("[scenarios]{")
    Dir.chdir("scenario") do
        scns_per_dir.each do |directory, scns|
            if re =~ directory
                [scns].flatten.each do |scn|
                    name = scn[:name]
                    puts("  [scenario](directory:#{directory})(name:#{name}){")
                    Dir.chdir(directory.to_s) do
                        cmd = [cook_exe]
                        args = ->(sym, &block){[scn[sym]||[]].flatten.each{|arg|cmd += [block.call(arg)||[]].flatten}}
                        args.call(:input){|v|["-f", v]}
                        args.call(:output){|v|["-o", v]}
                        args.call(:temp){|v|["-O", v]}
                        args.call(:toolchain){|v|["-t", v]}
                        args.call(:generator){|v|["-g", v]}
                        args.call(:depviz){|v|["-d", v]}
                        args.call(:data){|v|["-D", v]}
                        args.call(:help){|v|["-h", v]}
                        args.call(:vebose){|v|["-v", v]}
                        args.call(:recipe){|recipe|recipe}
                        sh(cmd*' ') do |ok, ret|
                            pf = case scn[:should]
                                 when :succeed then (ok  ? :passed : :failed)
                                 when :fail    then (!ok ? :passed : :failed)
                                 end
                            summary[pf] += 1
                            case pf
                            when :passed then puts("    [ok]")
                            when :failed then puts("    [FAIL]")
                            end
                        end
                    end
                    puts("  }")
                end
            end
        end
    end
    puts("}")
    print("[summary]")
    total = 0
    [:passed, :failed].each do |pf|
        cnt = summary[pf]
        total += cnt
        print("(#{pf}:#{cnt})") if cnt > 0
    end
    print("(total:#{total})")
    puts("")
end
