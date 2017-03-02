## qtcreator .pro file fragment for this safely sub library
##presumes SAFEDIR has been set to path of safely base directory
PRE_TARGETDEPS += $$SAFEDIR/libcppext.a
DEPENDPATH += $$SAFEDIR/cppext
INCLUDEPATH += $$SAFEDIR/cppext
## libs are built into safely project root, -l options are in path declared by -L options
LIBS += -L$$SAFEDIR -lcppext

