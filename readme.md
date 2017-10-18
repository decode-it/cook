Cook
====

Fast, simple and tasty recipes for modern C++.

`cook` is a build system for C++.

## QtCreator plugin

### Build QtCreator for Qt5/ubuntu

sudo apt install qmake
sudo apt install qtcreator
sudo apt install qt5-default
sudo apt install qtcore
sudo apt install qtscript5-dev
sudo apt install qtdeclarative5-dev

git clone --recursive https://code.qt.io/qt-creator/qt-creator.git
mkdir build-debug
cd build-debug/
qmake CONFIG=debug ../qt-creator/qtcreator.pro 
make -j 9

## Nodes

* Arrow: from user to supplier
* Downstream: with arrows
* Upstream: against arrows

### Include path
### Library
### Library path

### Macro

By default: do not propagate upstream. For windows defines, this will be needed: NOMINMAX, LEAN_AND_MEAN, ANSI, ...

### Unspecified

* Meant to create structure
* Collection of files and settings
* Public and private include paths are communicated upstream
* Supplier inc paths, libs and lib paths are com upstream

### Static library

* Collects recursively all unspecified suppliers
* Provides a level of abstraction upstream: only public include paths are distributed upstream
* Only provides public items upstream
* Does not allow direct addition of include paths, libraries or lib paths

### Prebuilt library

* Only allows direct addition of include paths, libraries or lib paths
* Does not allow addition of files

