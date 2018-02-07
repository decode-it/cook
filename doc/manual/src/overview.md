# Overview

Google describes a recipe as follows
> _noun_ : *recipe*, plural: *recipes*
>
> A recipe is a set of instructions for preparing a particular dish, including a list of the ingredients required.

Analogously cook is a build system for constructing C/C++ libraries and executables and managing complicated builds in an intuitive way: Open up a (cook)book, pick out your recipe and cook will prepare that for you.

Let's start with some basic terminology:

 * **recipe**: A recipe is a conceptual entity in your code project, it can be a library, an executable, but just as well a subset hereof. The ingredients of a recipe are the different files, but also the include paths, macros, etc. Just as in real-life most soups recipes require you to first prepare a broth, a recipe can depend on other recipes. 
 
 * **book**: Recipes are collected in a book, and books can be nested within book. Each book has a name, and nesting occurs similarly as with directories. So, for example, if `book_A` contains `book_B` which in turns contains a recipe `recipe_C`, then absolute identifier for `recipe_C` is `book_A/book_B/recipe_C`.

Consider the following directory structure
~~~
MyBook/
├── lib/                 <-- all files for creating a library
|   ├── process.hpp
|   └── process.cpp
└── app/                 <-- all files for creating the main application
    └── main.cpp
~~~

process.hpp
```c++
#ifndef _process_hpp_
#define _process_hpp_

int add(int a, int b);

#endif
```

process.cpp
```c++
#include "process.hpp"

int add(int a, int b)
{
    return a + b;
}
```

main.cpp
```c++
#include <iostream>
#include "process.hpp"

int main(int argc, char ** argv)
{
    std::cout << "4 + 5 = " << add(4,5) << std::endl;
    return 0;
}
```

```
root.book("myProject", fun(b){
    b.recipe("lib", fun(r) {
        r.add("lib", "**.[hc]pp")
    })
    b.recipe("app", fun(r) {
        r.add("app", "main.cpp")
        r.depends_on("lib")
    })
})
```
 
 
