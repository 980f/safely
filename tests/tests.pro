TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += .. ../cppext ../system

#our sibling with heapless code
include("../cppext.lib.pro")

#our sibling with heaped code
include("../system.lib.pro")

#actually using sigc stuff locally as well as hidden perhaps in the libs.
include("../sigc.pro")

SOURCES += main.cpp
