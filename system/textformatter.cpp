#include "textformatter.h"
#include "textpointer.h"
#include "stdlib.h"

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
  format(Cstr(mf).violated(),Cstr(mf).length()),
  body(Cstr(mf).violated(),Cstr(mf).length())
{

}

TextFormatter::~TextFormatter(){
  //#nada
}





void TextFormatter::substitute(CharFormatter buf){
  unsigned width=buf.used();
  if(sizing){
    sizer-=2;//dollar and digit
    sizer+=width;
  } else {
    //shove data up
    if(body.move(width-2)){
    //point to '$'
      body.rewind(width+2);
      //overlay
      body.appendUsed(buf);
    }
  }
}

void TextFormatter::substitute(Cstr stringy){
  substitute(CharFormatter::infer(stringy.violated()));
}

void TextFormatter::substitute(TextKey stringy){
  substitute(Cstr(stringy));
}

void TextFormatter::substitute(double value){
  unsigned width=Zguard(nf.needs());
  if(sizing){
    sizer+=width-2;
  } else {
    //shove data up
    if(body.move(width-2)){
    //point to '$'
      body.rewind(width);
      //overlay
      CharFormatter workspace(&body.peek(),width);
      if( !workspace.printNumber(value,nf)) {//if failed to insert anything
        workspace.printChar('?');//replaces '%'
        workspace.printDigit(which);
      }
      //todo: remove excess allocation
      body.skip(workspace.used());
      //pull data back down over unused stuff
      body.removeNext(workspace.freespace());
    } else {
      //leave marker in place, or perhaps overwrite % with '?'
    }
  }
  nf.onUse();
}

void TextFormatter::substitute(const NumberFormat &item){
  //this should not actually get called. we migth still use it to relocate some code from header to here should this ever become more than an assignement.
  nf = item;
}

bool TextFormatter::onSizingCompleted(){
  sizing=false;
  if(sizer>0){
    clear();
    body.wrap(reinterpret_cast<char*>(malloc(sizer)),sizer);
    this->ptr=body.internalBuffer();

    if(body.allocated()>0){
      body.appendAll(format);
      return true;
    }
  }
  return false;
}
