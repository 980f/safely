#include "bufferformatter.h"
#include "string.h" //memmove

BufferFormatter::BufferFormatter(const CharFormatter &other, TextKey format):
  body(other)
{
  body.grab(other);
  body.cat(format);
  body.next()=0;//cat doesn't include the null, we do so here to stop the parsing of the format
}

bool BufferFormatter::insert(const char *stringy, unsigned length){
  //body is pointing to char that follows what is to be inserted
  //spec should have the same info, and how much to erase.
  //move data out by the difference
  if(body.move(length-spec.span())){//then there is room for the insertion
    if(length){//in case memmove looks at stringy before length and gets annoyed at a nullptr.
      memmove(body.internalBuffer()+spec.lowest,stringy,length);
    }
    return true;
  } else {
    //couldn't make room, abandon operation
    return false;
  }
}

void BufferFormatter::substitute(Cstr stringy){
  insert(stringy,stringy.length());
}

void BufferFormatter::substitute(Indexer<char> buf){
  insert(buf.internalBuffer(),buf.used());
}

void BufferFormatter::substitute(double value){
  NumberPieces p;
  unsigned space=nf.needs(value,&p);

  if(space<=nf.fieldWidth){//number will fit into space requested
    //todo: enforce fieldWidth if it is valid, need left vs right align.
    if(body.move(space-spec.span())){//then there is room for the insertion
      body.rewind(space-spec.span());

      //print as decimal string
      if(p.negative){
        body.printChar('-');
      }
      body.printUnsigned(p.predecimal);
      for(unsigned zeroes=p.pow10;zeroes-->0;){
        body.printChar('0');
      }
      //and we are done
    }
  } else {
    //print exponential, if possible
  }
  nf.onUse();//whether used successfully or not ...
}
