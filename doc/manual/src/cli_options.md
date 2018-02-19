# Command-line options

`cook [options]* <recipe>*`

## Options

* `-f <filename>`: Zero or more recipes to process. By default, `recipes.chai` in the local path is used.
* `-o <directory>`: Output directory, default is the local path.
* `-O <directory>`: Temporary build cache directory.
* `-t <toolchain>`: Toolchain set of agents to use, `_list_` to list the available values.
* `-g <generator>[=<directory>]`:
  * Generators
    * `build`: Perform internal build, default operation mode.
    * `cmake`: Generate cmake file.
    * `naft`: Generate naft file.
    * `json`: Generate JSON compilation database file.
  * The directory specified with `-o` is used as default directory.
  * The phony directory `-` will put the output on `stdout`.
  * If no generator is specified, `build` is used.
* `-c`: Clean all data for the specified generators.
* `-D <key[=value]>`: Define global chai-script variable to influence the process.
  * `config`=`debug|release`
  * `profile`
* `-h`: Prints this help
* `-v <level>`: Verbosity level
  * Default level is 1.
  * 0 is silent.

## Recipe

* Default generates all recipes.
* `_list_` to list the available recipes.
