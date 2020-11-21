#ifndef TABLEOFPOINTERS_H
#define TABLEOFPOINTERS_H

/* sometimes you want a bunch of items to be processed in a group, without them having to know of the existence of that group.
 * You could add a base class with a static link to last and a member link to previous making a list.
 * That however requires that all objects be mutable as the mechanism involves a linkedlist that is modified during construction.
 * That also requires that you call constructors for each object, precluding constexpr classes.
 *
 * This set of macro's lets you build a table in rom of pointers to members of the list, the objects can themselves be const (mostly).
 * Pointers are used so that polymorphic assemblages can be made. It is also simpler than trying to pack them into a segment and still find them.
 *
 * The most burdensome requirement is that all such objects be statically allocated, their existence must be known to the linker.
 *
 * The objects can be contained in an outer class, the variants with 'Named' in the macro name must be used for those.
*/

/* the table is of pointers, the pointers are const, the object pointed to is not. The table namer is named ClassT in the macro but that is just one use case, it is not required that it be a class name */
#define TableTag(ClassT,prior) const __attribute((used, section(".table." #ClassT "." #prior )))

/** puts a thing in the list. If the name of the thing has dots in it (a member of some structure) then you will have to provide a nickname to be the pointer name */
#define RegisterAt(ClassX,varble,priority) ClassX * TableTag(ClassX,priority) ClassX##varble (&varble)
#define RegisterAtNamed(ClassX,varble,priority,nickname) ClassX * TableTag(ClassX,priority) ClassX##nickname (&varble)

/** puts a thing in the list at a middling priority */
#define Register(ClassY,varble) RegisterAt(ClassY,varble,50)
#define RegisterNamed(ClassY,varble,nickname) RegisterAtNamed(ClassY,varble,5,nickname)

/** tagging the table, using priority to make these dummy items get allocated before and after the real ones.
 Using a nullptr allows us to iterate while not null, so we don't even need to know the name of the endmarker.
 The extreme tag numbers below are what define the range of priorities, They look like numbers but are text.
 */

#define MakeTable(ClassM) \
ClassM * TableTag(ClassM,0) begin##ClassM##Table(nullptr);\
ClassM * TableTag(ClassM,9999999) end##ClassM##Table(nullptr)

#define ForTable(ClassK) \
for(ClassK * const *it=&begin##ClassK##Table;*(++it);)

//////////////////////////////
/* When all the objects are const and of the same concrete class the objects themselves can be in rom and the table is the objects, not pointers.
 * ObjectTag macro is for internal use here.
 *
 * Usage:  In one file only have MakeObjectTable(classname)
 * For each object to be in that table: MakeObject(classname,variablename, constexpr constructor args);
 *
*/
#define ObjectTag(Classy,prior) __attribute((used, section(".table." #Classy "." #prior )))

//creates a thing in the list
#define MakeObjectAt(Classy,varble,priority,...) const Classy ObjectTag(Classy,priority) varble {__VA_ARGS__}
#define MakeObject(Classy,varble,...) MakeObjectAt(Classy,varble,50,__VA_ARGS__)

/** to find the table we need to make a dummy object in the first slot, then iterate starting after it.
 To not need a null constructor (nor waste rom) we alloc an unsigned at the start of the table's segment presuming an unsigned has natural alignment size. */
#define MakeObjectTable(Classy) \
const unsigned ObjectTag(Classy,0) begin##Classy##sTable=0;\
const unsigned ObjectTag(Classy,999999) end##Classy##sTable=0;

#define ForObjects(Classy) \
for(auto it= reinterpret_cast<const Classy *>(&begin##Classy##sTable+1) ;it< reinterpret_cast<const Classy *>(&end##Classy##sTable);++it)

/** for the above we should be able to fake out an array and compute its length and use an index should that interest someone. */

/////////////////////////////
/* needed this variant for a table of function pointers.
*/
#define MakeRefAt(Classy,varble,priority) const Classy ObjectTag(Classy,priority) RefTo##varble (varble)
#define MakeRef(Classy,varble) MakeRefAt(Classy,varble,5)

#define MakeRefTable(Classy) \
Classy ObjectTag(Classy,0) begin##Classy##sTable(nullptr);\
Classy ObjectTag(Classy,9999999) end##Classy##sTable(nullptr)

#define ForRefs(Classy) \
for(Classy *it=&begin##Classy##sTable;**(++it);)

#endif // TABLEOFPOINTERS_H
