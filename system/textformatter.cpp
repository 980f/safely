#include "textformatter.h"
#include "textpointer.h"
/*
The algorithm:
scan the format string for highest numbered substitution item.
calloc that many pieces
* Until we allow for more than 10 items do a fixed worst case allocation
*
scan the format string processing FormatControls and computing the length of each item referenced.

sum that all up and allocate once a buffer big enough to hold all.

count the number of substitution instances and allocate a list of insertion items

scan the format again, recording which and where for each submarker

scan the list above concatenating pieces linearly. Generated items get generated more than once if they are referenced more than once. That is sub-optimal.

?? can we recurse to dynamically allocate items?
For each number argument we can use the stack and recursion to allocate a buffer that we use a part of.
As we unwind the stack we have to 'use up' the value.

It seems that whatever we do we end up having to do actual insertions OR regenerate numbers. Since multiple references are rare, and in fact might be done with
different formatting contexts we won't actually try to reuse an argument's rendering.

*/


TextFormatter::TextFormatter(TextKey mf):
  format(nullptr,Cstr(mf).length())
{

}

TextFormatter::~TextFormatter(){
  //#nada
}
