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
