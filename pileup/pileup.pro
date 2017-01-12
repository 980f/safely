TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


##our sibling with heaped code
#include("../system/lib.pro")

#our sibling with heapless code
include("../cppext/lib.pro")


SOURCES += main.cpp
