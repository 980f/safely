## qtcreator pro file fragment for making a static lib
QT       -= core gui
TEMPLATE = lib
CONFIG += staticlib

include("safely.pro")

#dump the libs above the lib dirs, for easy scripting
DESTDIR = ..
#libraries have unused functions:
CCFLAG += -W-unused-function


