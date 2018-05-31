# Basic Example

Let us create a basic example, with the following directory structure
~~~
MyBook/
├── lib/
|   ├── process.hpp
|   └── process.cpp
└── app/
    └── main.cpp
~~~

with the following files:

```c++
// MyBook/lib/process.hpp 
#ifndef _process_hpp_
#define _process_hpp_

int add(int a, int b);

#endif
```
```c++
// MyBook/lib/process.cpp
#include "process.hpp"

int add(int a, int b)
{
    return a + b;
}
```
```c++
// MyBook/app/main.cpp
#include <iostream>
#include "process.hpp"

int main(int argc, char ** argv)
{
    std::cout << "4 + 5 = " << add(4,5) << std::endl;
    return 0;
}
```

We can create a book for this project by adding the file `Mybook/recipes.chai` with the following contents:
```ruby
# MyBook/recipes.chai
root.book("myProject", fun(b) {
    b.recipe("lib", TargetType.Archive, fun(r) {
        r.add("lib", "**.[hc]pp")
    })
    b.recipe("app", TargetType.Executable, fun(r) {
        r.add("app", "main.cpp")
        r.depends_on("lib")
    })
})
```

Our fresh and tasty application can be build as follows
```bash
$ cd MyBook
# building
$ cook -g ninja /myProject/app
# executing
$ ninja -v
```

Time to dissect our `recipes.chai`, step by step.

#### Creating a book

Our book starts by creating a new book name _myProject_ in the root book. All recipes in cook are stored in books, and books can be nested within other books. Think of them as C++ namspaces. This way we can
 * organize all recipes in a tree-like structure
 * we avoid having name clashes.

```ruby
root.book("myProject", fun(b) {
    ...
})
```
The second argument to this `book` function is `fun(b){}` is the callback functor, with argument `b` the newly created book. (Note that the scope of `fun(b)` starts at first line and goes up to the final line of our script.)

#### Creating a static library

On line two, add a new recipe named _"lib"_ to `b`. So far, our `recipes.chai` contains a recipe named `/myProject/lib`. 
```ruby
...
    b.recipe("lib", TargetType.Archive, fun(r) {
        r.add("lib", "**.[hc]pp")
    })
...
```
The type of this recipe is `TargetType.Archive` as specified by the second (and optional) argument. The third and final argument is a callback functor, similarly when creating books.
On the third line, we add all the files for our static library: all files stored under lib and having as extension _"hpp"_ or _"cpp"_ our added as ingredients to our recipe. 

> _"**"_ is a globbing expression which means recursing all subfolders. Thus _"**.[hc]pp"_ means _recurse all subdirectories and add all files with extension .hpp or .cpp_. Not recursing the subfolders can be done by using _*.[hc]pp_

If you're wondering why our recipe contains `r.add("lib", "**.[hc]pp")`, and not `r.add("./", "lib/**.[hc]pp")` (or even `r.add("lib/**.[hc]pp")`), bear with us for a moment. 

#### Creating an application

Similarly as for _lib_, we add a new recipe named "app" as follows
```ruby
...
    b.recipe("app", TargetType.Executable, fun(r) {
        r.add("app", "main.cpp")
        r.depends_on("lib")
    })
...
```
The type of this recipe is an `TargetType.Executable` and we add a single main.cpp file to the recipe. Finally the recipe specifies a dependency `r.depends_on("lib")`, meaning that _/myProject/app_ depends on a recipe named _lib_. 

> When cook tries to resolve the dependencies for a recipe, it starts at the current book and tries to resolve that dependency. If that doesn't work, we go to the parent book, and continue, until we reach the root book. So in our case, the dependency _lib_ implies that we first look for _/myProject/lib_, and if that does not exist, consider _/lib_. 

Let us reconsider the recipe.add() function. When given two arguments, the first one is the directory, and the second the pattern. When we write `r.add("lib", "**.[hc]pp")`, cook will add _lib/_ as an include path, if we would write `r.add("./", "lib/**.[hc]pp")`, the _./_ will be added as include path. And that's the reason why the header ""process.hpp" can be resolved in _/myProject/app_.
