## qtcreator pro file fragment for making a static lib

QT       -= core gui

TEMPLATE = lib
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

INCLUDEPATH += .. 

