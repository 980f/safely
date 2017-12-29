## qtcreator pro file fragment for making a non-QT console app
QT       -= core gui
TEMPLATE = app
CONFIG += console

#curious as to why QT thinks there is web stuff in a console app.
DEFINES -= QT_WEBKIT

#best practices libraries:
include("safely.pro")
