# // *INDENT-OFF*  in case we run uncrustify on this flie

#magic for making a static lib
include ("../staticlib.pro")

#our sibling with heapless code
include("../cppext/lib.pro")
#we'll use system sigc code
include("../sigc.pro")

include("system.files")

