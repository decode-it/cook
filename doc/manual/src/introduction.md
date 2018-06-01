# Introduction

[Cook](https://github.com/decode-it/cook) is a _meta_ build system. It allows concise and precise expression of sets of files and properties (called _recipes_), together with a dependency structure, using the [chaiscript](https://chaiscript.com) programming language (kudos++). 

Based on this information, cook builds a network that fully describes how each input _source_ and _header_ file should be processed to create the desired output. Details like _defines_, _include paths_, _force includes_, _header dependencies_, _libraries_, _frameworks_, _archives_, _shared libraries_, _executables_, _source generating scripts_, _configurable toolchains_ and much more are handled as graceful as possible.
      
This allows cook to create a [ninja](https://ninja-build.org) or [cmake](https://cmake.org) build system, or export its internal information as a [structured naft file](https://github.com/gfannes/gubg.io/blob/master/src/gubg/parser/naft/spec.md) or HTML pages.
