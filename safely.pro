## QT project file fragment with things common to libraries  and applications using the 'safely' project

CONFIG -= app_bundle
CONFIG -= qt
#we want the same language in use everywhere:
CONFIG += c++14
#rpi gcc is 4.9, needs a kick in the std:
QMAKE_CXXFLAGS += -std=c++14

#safely projects put their application specific library altering includes above the module src path.
INCLUDEPATH += .. 
