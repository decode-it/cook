# Overview

A general overview of the model namespace is shown here:

![alternative text](http://www.plantuml.com/plantuml/proxy?src=https://raw.githubusercontent.com/gfannes/cook/clean/doc/design/src/model/model.puml?ttt=6)


Collection: Collection is a generic container which a) is ordered and b) contains no two elements with the same key.


Data is contained in three types of elements:
 * `Files = Collection<File>`: Each File 


A recipe contains all the 

### File

 Files in a recipe are specified wrt that recipe's working directory (if user specified the the file was specified in a relative way)

 * content file: for a content file, the dir is the part 

Data storage is done as follows:
 * File: represents a single file, this can be a source file, an include path, etc
 * Properties: Things like defines, all things that can be represented with a key value pair

 * **Recipe**: Data storage in

 * **UserData**: All user specified data which does not fit the Property or File interface, should be stored in a single object of type
