## // *INDENT-OFF* 
## fragment of a qtcreator .pro ject file for accessing sigc signals library

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sigc++-2.0
# the PKGCONFIG line above should have taken care of the following: //todo: remove explicit includepath and debug pkgconfig for sigc.
INCLUDEPATH += /usr/include/sigc++-2.0

