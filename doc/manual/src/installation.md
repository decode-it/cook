# Installation

## Building from source

* Install `git`
    * Windows: https://git-scm.com/
* Install `ruby` and `rake`
    * Windows: https://rubyinstaller.org
* Install `ninja`
    * OSX: `brew install ninja`
    * Windows: https://github.com/ninja-build/ninja/releases
* Clone the [cook](https://github.com/decode-it/cook) repo: `git clone https://github.com/decode-it/cook`
* Update all its submodules: `rake uth`
* Build the _bootstrap level 0_ binary: `rake b0:build`
* Build the _bootstrap level 1_ binary: `rake b1:build`
* Install the cook binary: `rake install`
