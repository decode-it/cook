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
{
    var my_book = cook["myProject"]
    my_book.recipe("lib", TargetType.Archive, fun(r) {
        r.add("lib", "**.[hc]pp")
    })

    {
        var app = my_book.recipe("app", TargetType.Executable)
        app.add("app", "main.cpp")
        app.depends_on("lib")
    }
}
```

Our fresh and tasty application can be built as follows
```bash
$ cd MyBook
# building
$ cook -g ninja /myProject/app
# executing
$ ninja -v
```

Time to dissect our `recipes.chai`, step by step.

#### Creating a book

Our book starts by asking cook to create a new book, named _myProject_. All recipes in cook are stored in books, and books can be nested within other books (as we'll see lateron). Think of them as C++ namspaces. This way we can
 * organize all recipes in a tree-like structure
 * we avoid having name clashes.
We store a reference to our book in the variable `my_book`
```ruby
{
    var my_book = cook["myProject"]
    ...
}
```
Why did wel place everything between semicolons? Well, that way, we are sure that the variable `my_book` will not clash, and thus we can reuse the name of that variable. Again, similar as in C/C++. 

#### Creating a static library

On line two, add a new recipe named _lib_ to `my_book`. So far, our `recipes.chai` contains a single recipe named `/myProject/lib`. 
```ruby
...
    my_book.recipe("lib", TargetType.Archive, fun(r) {
        r.add("lib", "**.[hc]pp")
    })
...
```
The type of this recipe is `TargetType.Archive` as specified by the second (and optional) argument. The third argument is a callback functor, which is called with our recipe. This way, we can configure our recipe by means of the variable `r`. On the third line, we add all the files for our static library: all files stored under lib and having as extension _"hpp"_ or _"cpp"_ are added as ingredients to our recipe. 

> _"**"_ is a globbing expression which means recursing all subfolders. Thus _"**.[hc]pp"_ means _recurse all subdirectories and add all files with extension .hpp or .cpp_. Not recursing the subfolders can be done by using _*.[hc]pp_

If you're wondering why our recipe contains `r.add("lib", "**.[hc]pp")`, and not `r.add("./", "lib/**.[hc]pp")` (or even `r.add("lib/**.[hc]pp")`), bear with us for a moment. 

#### Creating an application

Similarly as for _lib_, we add a new recipe named "app" as follows
```ruby
...
    {
        var app = my_book.recipe("app", TargetType.Executable)
        app.add("app", "main.cpp")
        app.depends_on("lib")
    }
...
```
The type of this recipe is an `TargetType.Executable`. However, this time we did not supply a configuration callback function, and thus, similarly when we created our book, we get a reference to our recipe (which we store in the variable app). We add a single main.cpp file to the recipe. Finally the recipe specifies a dependency `r.depends_on("lib")`, meaning that _/myProject/app_ depends on a recipe named _lib_. 

> When cook tries to resolve the dependencies for a recipe, it starts at the current book and tries to resolve that dependency. If that doesn't work, we go to the parent book, and continue, until we reach the root book. So in our case, the dependency _lib_ implies that we first look for _/myProject/lib_, and if that does not exist, consider _/lib_. 

Let us reconsider the recipe.add() function. When given two arguments, the first one is the directory, and the second the pattern. When we write `r.add("lib", "**.[hc]pp")`, cook will add _lib/_ as an include path, if we would write `r.add("./", "lib/**.[hc]pp")`, the _./_ will be added as include path. And that's the reason why the header ""process.hpp" can be resolved in _/myProject/app_.


## Nesting books 

We mentioned that books can be nested, for clearer organization. There are many ways we can achieve this. Let us created a new book `test`, nested inside `myProject`, and a recipe `exe` in `/myProject/test`. This can be done as follows:
```ruby
{
    # 1st alternative
    var b1 = cook["myProject"]
    var b2 = b1.book("test")
    var r1 = b2.recipe("exe")
}

{
    # 2nd alternative
    var b = cook["myProject/test"]
    var r = b.recipe("exe")
}

{
    # 3th alternative
    var r = cook["myProject/test"].recipe("exe")
}

{
    # 4th alternative
    var r = cook.recipe("myProject/test/exe")
}

{
    # 5th alternative
    var r = cook["myProject"].book("test").recipe("exe")
}

{
    # 6th alternative
    cook.book("myProject", fun(b1)
    {
        b1.book("test", fun(b2) {
            b2.recipe("exe")
        })
    })
}
```
These are some of the alternatives. Most alternatives, are quiet selfexplanatory, but take a look at the 4th. We can see that `cook` itself, acts as a book, on which we can create a recipe or a book (and a lot more than that). And thus `cook["myProject/test"]` is just shorthand for `cook.book("myProject/test")`
