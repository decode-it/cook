# Installation

## Building from source

* Install `git`
    * Windows: https://git-scm.com/
* Install `ruby` and `rake`
    * Windows: https://rubyinstaller.org
* Install `ninja`
    * OSX: `brew install ninja`
    * Windows: https://github.com/ninja-build/ninja/releases
* Install compiler toolchain
    * OSX: `brew install --with-clang llvm` and `export PATH=/usr/local/opt/llvm/bin:$PATH` to make this compiler available
    * Windows: install [visual studio community 2017](https://www.visualstudio.com/vs/community/) and `C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat x86`
* Clone the [cook](https://github.com/decode-it/cook) repo: `git clone https://github.com/decode-it/cook`
* Update all its submodules: `rake uth`
* Build the _bootstrap level 0_ binary: `rake b0:build`
* Build the _bootstrap level 1_ binary: `rake b1:build`
* Install the cook binary: `rake install`
