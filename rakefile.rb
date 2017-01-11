gubg_dir = File.join(Dir.pwd, ".gubg")
ENV["gubg"] = gubg_dir

begin
    require("./gubg.build/shared.rb")
rescue LoadError
    puts("This seems to be a fresh clone: I will update all required submodules for you.")
    sh "git submodule update --init"
    retry
end

task :default do
    sh "rake -T"
end

namespace :gubg do
    def each_mod()
        mods = %w[build std io chaiscript].map{|e|"gubg.#{e}"}
        mods.each do |mod|
            Dir.chdir(mod) do
                yield
            end
        end
    end

    task :proper do
        rm_rf gubg_dir
    end
    desc "Updates all submodules the their respective heads"
    task :uth do
        each_mod do
            sh "git checkout master"
            sh "git pull --rebase"
        end
    end
    task :declare do
        each_mod do
            sh "rake declare"
        end
    end
    task :define => :declare do
        each_mod do
            sh "rake define"
        end
    end
    task :ut => :define do
        require("gubg/build/Executable")
        ut = Build::Executable.new("unit_tests")
        ut.add_sources(FileList.new("gubg.io/src/test/file/System.cpp"))
        ut.add_sources("#{gubg_dir}/source/catch_runner.cpp")
        ut.add_include_path("#{gubg_dir}/include")
        ut.add_library_path("#{gubg_dir}/lib")
        ut.add_library("gubg.std", "gubg.io", "stdc++fs")
        case :debug
        when :debug
            ut.add_option('g')
        else
            ut.add_define('NDEBUG')
        end
        options = %w[-a -d yes]
        ut.run(options)
    end
end

namespace :cook do
    exe = nil
    task :init => "gubg:define" do
        require("gubg/build/Executable")
        exe = Build::Executable.new("cook")
        exe.add_sources(FileList.new("src/**/*.[hc]pp"))
        exe.add_sources(FileList.new(File.join(gubg_dir, "include/**/*.hpp")))
        exe.add_include_path("src")
        %w[std io].each{|e|exe.add_include_path("gubg.#{e}/src")}
        exe.add_include_path("#{gubg_dir}/include")
        exe.add_library("dl", "stdc++fs")
        case :debug
        when :debug
            exe.add_option('g')
        else
            exe.add_define('NDEBUG')
        end
    end
    task :clean do
        rm_rf(".cache")
    end
    task :build => :init do
        exe.build
    end
    task :run => :init do
        exe.run("cook#exe")
        sh "make cook.exe"
    end
end
desc "Builds cook"
task :build => "cook:build"
desc "Tests cook"
task :test => ["cook:build", "cook:run"]

task :clean => "cook:clean" do
    rm_f(FileList.new("**/*.obj"))
end
