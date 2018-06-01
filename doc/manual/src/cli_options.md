# Command-line options

`cook [options]* <recipe>*`

## Working options

* `-f <filename>`: Zero or more recipes to process. By default, `recipes.chai` in the local path is used.
* `-o <directory>`: Directory for output created by cook, default is the local path.
* `-O <directory>`: Temporary build cache directory.
* `-t <toolchain>`: Zero or more toolchains to use. If the `<toolchain>.chai` file cannot be found, cook will try to use any of the predefined toolchains (gcc|clang|msvc).
* `-T <key>[=<value>]`: Zero or more toolchain options, as a _key_ or _key_-_value_-pair. These options are processed by the toolchain.
  * `[config=]debug|release|rtc|profile`: Enable a specific configuration.
  * `[arch=]x86|x64|x32|x64|armv7|arm64`: Enable a specific CPU architecture.
  * `c++.std=03|11|14|17`: Enable a specific C++ standard.
  * `c.std=99|11`: Enable a specific C standard.
  * `pic`: Enable _position independent code_
* `-g <generator>[=<directory>]`:
  * Generators
    * `ninja`: Create a [ninja](https://ninja-build.org)-based build system.
    * `cmake[=subdir]`: Create a [cmake](https://cmake.org)-based build system.
    * `naft`: Generate detailed book/recipe structure and information in `recipes.naft`, using the [naft](https://github.com/gfannes/gubg.io/blob/master/src/gubg/parser/naft/spec.md) format.
    * `html[=subdir]`: Generate HTML output
    * `graphviz.compontent`: [Graphviz](http://www.graphviz.org/)-based component output.
    * `graphviz.dependency`: [Graphviz](http://www.graphviz.org/)-based dependency output.
  * The directory specified with `-o` is used as default directory.
* `-C <chef>`: Chef to use
  * `scal`: Script-Compile-Archive-Link (default)
  * `cal`: Compile-Archive-Link: same as "scal", but will not run the script recipes.
  * `void`: No chef
* `-D <key[=value]>`: Define global chai-script variable to influence the process.
* `-h`: Prints this help
* `-v <level>`: Verbosity level
  * Default level is 1.
  * 0 is silent.

## Recipe

* Default generates all recipes.

## Future options

* `-c`: Cleans the output directories.

