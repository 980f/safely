# // *INDENT-OFF*  in case we run uncrustify on this flie

#magic for making a static lib
include ("../staticlib.pro")
#our sibling with heapless code
include("../cppext/lib.pro")
#our sibling with mallocating code, but no OS per se:
include("../system/lib.pro")

##for async IO add to your executable project:
#LIBS += -lrt

include("posix.files")

