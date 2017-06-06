require "mkmf"

if !find_executable("cook.exe")
    puts("Cook not found, I will build and install it")
    Dir.chdir(File.dirname(__FILE__)) do
        sh "rake install[#{ENV["gubg"]}/bin]"
    end
end
