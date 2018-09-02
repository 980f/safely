# // *INDENT-OFF*  in case we run uncrustify on this flie

#magic for making a static lib
#who brought this back? causes a build loop. include ("../staticlib.pro")

#our sibling with heapless code
#who brought this back? part of static lib builds, which are deprecated: include("../cppext/lib.pro")
#we'll use system sigc code
include("../sigc.pro")

include("system.files")

