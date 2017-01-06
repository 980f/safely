include("../consoleapp.pro")

#actually using sigc stuff locally as well as hidden perhaps in the libs.
include("../sigc.pro")
#our sibling with heaped code
include("../system/lib.pro")
#our sibling with heapless code
include("../cppext/lib.pro")
#our sibling with OS code
include("../posix/lib.pro")


SOURCES += main.cpp \
    jsontests.cpp
