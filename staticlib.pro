## qtcreator pro file fragment for making a static lib

QT       -= core gui

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

#system options include files are above the lib dirs
INCLUDEPATH += .. 
#dump the libs above the lib dirs, for easy scripting
DESTDIR = ..

#libraries have unused functions:
CCFLAG += -W-unused-function

