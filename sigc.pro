## qtcreator .pro ject file for accessing sigc signals library

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += sigc++-2.0

message("Including sigc++ stuff via system package management")
