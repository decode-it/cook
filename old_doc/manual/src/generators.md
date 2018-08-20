# Generators

Cook can output its internal dependency network in different formats. Some result in a build system, others are meant to inter-operate with other tools.

## Ninja

The `-g ninja` generator will create a single `build.ninja` file. The `-o <dir>` controls where this `build.ninja` file will be created, by default, in the current working directory.

Once the `build.ninja` is generated, [ninja](https://ninja-build.org) should be used to execute the actual build commands. Cook is responsible for creating the correct file dependencies, ninja is responsible for deciding which files need to be (re)build based on timestamp info. The intermediate files that ninja will create -- object files etc. -- will be stored in the `-O <dir>`, if specified.

## CMake

The `-g cmake` generator will create a [cmake](https://cmake.org)-based build system. It will create the necessary `CMakeLists.txt` files that can be converted processed by cmake.

By default, the cmake generator will create this `CMakeLists.txt` hierarchy in the current working directory. By passing an additional `<path>`, the cmake generator can be instructed to generate the `CMakeLists.txt` hierarchy in a different location: `-g cmake=<path>`.

## HTML

The `-g html` generator will create a set of HTML web pages that allows easy inspection of the different recipes and their interdependencies.

By default, the HTML pages are written in the output directory specified via the `-o <directory>` option. Within this output directory, the `html` subdirectory is used to not litter the output directory too much. The HTML generator takes an optional value to change the `html` subdirectory into a user-specified subdirectory via `-g html=<dir>`.

## Naft

The `-g naft` generator creates a single `recipes.naft` file that contains all the details for the book/recipe hierarchy in [naft format](https://github.com/gfannes/gubg.io/blob/master/src/gubg/parse/naft/spec.md). This information _can_ be read with a [regular text editor](https://neovim.io/) for detailed inspection, but can also be parsed easily by external tools. As such, they have all the information that was collected by cook, and can eg. create releases etc.

If the only goal is to extract basic recipe information from cook, without all the filenames, you can specify the `-C void` chef in addition with the `-g naft` generator. This instructs cook to only read the book/recipe hierachy, but not collect all the information necessary to create a build, like finding all the files based on the globbing patterns, running the external scripts, guessing the file types, ...

## Graphviz

The `-g graphviz.dependency` and `-g graphviz.component` generators create [graphviz](https://graphviz.gitlab.io/documentation/) output in `dot` format. The respective output files `recipes.graphviz` and `component.graphviz` can be visualized using any of the layout generators from graphviz (dot, neato, ...) with a command like `neato -Tpng recipes.graphviz -orecipes.png`.
