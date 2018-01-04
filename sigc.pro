## qtcreator .pro ject file for accessing sigc signals library

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sigc++-2.0

message("sigc++ stuff is included via system package management")
