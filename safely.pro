## QT project file fragment with things common to libraries  and applications using the 'safely' project

CONFIG -= app_bundle
CONFIG -= qt
#we want the same language in use everywhere:
CONFIG += c++14

#safely projects put their application specific includes above the application src path.
INCLUDEPATH += .. 
