#ifndef PERMALLOC_H
#define PERMALLOC_H

/** allocate a chunk that will never be released, ever.
that is a bad thing to do with local variables, so only use this in
constructors of statically constructed things.

This was created to share some code that requires 'malloc' in a system that doesn't allow for that (reliable mcu programming)

On platforms that do allow malloc this is a wrapper for malloc.

*/
void *permAlloc(unsigned quantity,unsigned sizeofone=1);
void permFree(void *calloced);

#define PermAlloc(type, quantity) reinterpret_cast<type *>(permAlloc((quantity),sizeof(type)))

//argument list for Indexer<type> usage
#define StaticBuffer(type, quantity) PermAlloc(type, (quantity)),((quantity)*sizeof (type))
#define StaticFree(entity) permFree(entity)

#endif // PERMALLOC_H
