//"(C) Andrew L. Heilveil, 2017"
#include "bufferformatter.h"
#include "string.h" //memmove

BufferFormatter::BufferFormatter(const Indexer<uint8_t> &other, TextKey format):
  body(reinterpret_cast<char *>(other.peek()),other.freespace())
{
  setFormat(format);
}

BufferFormatter::BufferFormatter(CharFormatter &buffer, TextKey format):body(buffer){
  setFormat(format);
}

BufferFormatter::BufferFormatter(char *raw, unsigned sizeofraw, TextKey format):body(raw,sizeofraw){
  setFormat(format);
}

BufferFormatter::BufferFormatter(unsigned char *raw, unsigned sizeofraw, TextKey format):
    BufferFormatter(reinterpret_cast<char *>(raw),sizeofraw,format){
//  setFormat(format);
}

BufferFormatter& BufferFormatter::setFormat(TextKey format){
  body.rewind();
  body.cat(format);
  body.clearUnused();//easiest way to ensure proper termination.
  which=0;
  return *this;
}

bool BufferFormatter::insert(const char *stringy, unsigned length){
  //FYI: body is pointing to char that follows what is to be inserted, and for simplicity should point past the insertion when it is completed.
  //spec is chars to remove
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

void BufferFormatter::substitute(TextKey stringy){
  substitute(Cstr(stringy));
}

void BufferFormatter::substitute(Indexer<char> buf){
  insert(buf.internalBuffer(),buf.used());
}

void BufferFormatter::substitute(double value){
  char maxprintable[2+19+19+1];//sign, dp, 19 digits before, 19 digits after, null
  CharFormatter workspace(maxprintable,sizeof(maxprintable));
  workspace.zguard();//insert failsafe null
  if(workspace.printNumber(value,nf,false)) {//testing so that we don't add units if number failed to print
    insert(maxprintable,workspace.used());
  }
  nf.onUse();//whether used successfully or not ...
}

void BufferFormatter::substitute(uint64_t value){
  //not punting to double, we don't want its formatting rules applied to actual integers
  char maxprintable[19+1];//19 digits , null
  CharFormatter workspace(maxprintable,sizeof(maxprintable));
  workspace.zguard();//insert failsafe null
  if(workspace.printUnsigned64(value)) {  //if formatted ok
    insert(maxprintable,workspace.used());
  }
}

void BufferFormatter::substitute(uint32_t value){
  //not punting to double, we don't want its formatting rules applied to actual integers
  char maxprintable[19+1];//19 digits , null
  CharFormatter workspace(maxprintable,sizeof(maxprintable));
  workspace.zguard();//insert failsafe null
  if(workspace.printUnsigned(value)) {  //if formatted ok
    insert(maxprintable,workspace.used());
  }
}

void BufferFormatter::substitute(uint16_t value){
  substitute(uint32_t(value));
}

void BufferFormatter::substitute(uint8_t value){
  substitute(uint32_t(value));
}

void BufferFormatter::substitute(char value){
  insert(&value,1);
}


void BufferFormatter::substitute(int32_t value){
  //not punting to double, we don't want its formatting rules applied to actual integers
  char maxprintable[19+1];//19 digits , null
  CharFormatter workspace(maxprintable,sizeof(maxprintable));
  workspace.zguard();//insert failsafe null
  if(workspace.printSigned(value)) {  //if formatted ok
    insert(maxprintable,workspace.used());
  }
}

void BufferFormatter::substitute(int16_t value){
  substitute(int32_t(value));
} //

void BufferFormatter::substitute(int8_t value){
  substitute(int32_t(value));//need this to distinguish char * from implied char &
}

void BufferFormatter::substitute(bool value){
  substitute(uint8_t(value));
} // TextFormatter::substitute


void BufferFormatter::substitute(const NumberFormat &item){
  //this should not actually get called. we migth still use it to relocate some code from header to here should this ever become more than an assignment.
  nf = item;//copies fields, the original doesn't need to linger.
}
