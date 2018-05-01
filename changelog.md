Cook changelog
==============

## 1.0.0

Proof-of-concept version.

## 1.0.1

* Support for generating ninja files from chaiscript
* Support for naft-formatted information for the cook-qtcreator-plugin
* Rework towards model-view-presenter framework
* Support for static libraries and executables
* DAG-based recipe dependencies with limited data exchange between recipes
* Rough MSVC support

## 1.1.0

* Extensive rework, cleanup, restructuring and kitchen nomenclature
* Split into library and app
* Recipe and build graphs
* Support for multiple chaiscript input files
* Support for ninja, naft and cmake output
* Support for user data for books and recipes
* Speed-up of file globbing
* Support for stack-based error messages
* Support for force includes
* Support for chaiscript regular expressions, file io and os

## 1.1.1 (open)

* Added `-C void` chef to speed-up generation of naft output when only the recipe names are required.
* Support for debug/release/rtc/profile and x86/x64/x32 builds via the toolchain option
* [wip] Support for gcc, clang and msvc toolchains
* [wip] Support for windows and osx
* [wip] Support for recursive header dependency detection

## Next

* Support for `COOK_PATH` search for scripts
* Chaiscript-based toolchain
* Internal #include-based dependency detection for compilers that cannot output dependencies
* Arduino support: compilation, linking and programming
