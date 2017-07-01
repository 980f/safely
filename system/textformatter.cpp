#include "textformatter.h"
#include "textpointer.h"
#include "stdlib.h"

/*
 *  The algorithm:
 *  scan the format string processing FormatControls and computing the length of each item referenced.
 *  sum that all up and allocate once a buffer big enough to hold all. (needs to handle temporary max).
 *  scan the format string again inserting processed strings.
 *
 *  Data lifetimes:
 *  ) the given format is strdup'd in this by the constructor
 *  ) the body member wraps that
 *  ) the sizing iterates over it
 *  ) a new buffer is allocated and its address is held in 'body'
 *  ) the new buffer is overwritten with the originally strdup'd data
 *  ) we release the strdup'd data and retain the allocation.
 *  ) on deletion of this object we delete the allocated block.
 *
 * todo: apply field with info somehow, need new classes for each type if we are going to mimic what was done for doubles.
 */


TextFormatter::TextFormatter(TextKey mf) :
  Text(mf){//strdup argument, so that it may evaporate
  body.wrap(violated(),length());
//  content.clone(body);
}

Indexer<u8> TextFormatter::asBytes(){
  return Indexer<u8> (reinterpret_cast<u8*>(body.internalBuffer()),body.used());
}

TextFormatter::~TextFormatter(){
  //#nada
}

bool TextFormatter::processing(){
  if(sizing) {
    int delta=width - substWidth;
    sizer += delta>0?delta:0;//max temporary size
    termloc +=delta; //actual final size
    return false;
  } else {
    return true;
  }
}

void TextFormatter::substitute(CharFormatter buf){
  width = buf.allocated();

  CharFormatter workspace = makeWorkspace();
  if(workspace.isUseful()) {
    if(!workspace.appendAll(buf)) {
      if(width>=substWidth) {
        onFailure(workspace);
      }
    }
    reclaimWaste(workspace);
  }

} // TextFormatter::substitute

void TextFormatter::substitute(Cstr stringy){
  substitute(CharFormatter::infer(stringy.violated()));
}

void TextFormatter::substitute(TextKey stringy){
  substitute(Cstr(stringy));
}

bool TextFormatter::openSpace(){
  int delta=width - substWidth;
  int section=dataend-body.used();
  if(body.move(delta,section)) {
    dataend += delta;
    //point to '$'
    body.rewind(width);
    return true;
  } else {
    return false;
  }
}

void TextFormatter::reclaimWaste(const CharFormatter &workspace){
  body.skip(workspace.used());
  //pull data back down over unused stuff
  unsigned excess = workspace.freespace();
  dataend -= excess;
  body.removeNext(excess);
}

CharFormatter TextFormatter::makeWorkspace(){
  if(processing()&&openSpace()) {
    return CharFormatter(&body.peek(),width);
  } else {
    return CharFormatter();
  }
}

void TextFormatter::onFailure(CharFormatter workspace){
  workspace.printChar('?');//replaces '%'
  workspace.printDigit(which);
}

void TextFormatter::substitute(double value){
  width = Zguard(nf.needs());
  CharFormatter workspace = makeWorkspace();
  if(workspace.isUseful()) {
    if( !workspace.printNumber(value,nf)) {   //if failed to insert anything
      onFailure(workspace);
    }
    reclaimWaste(workspace);
  }
  nf.onUse();
} // TextFormatter::substitute


void TextFormatter::substitute(u64 value){
  //not punting to double, we don't want its formatting rules applied to actual integers
  width = Zguard(1 + ilog10(value));
  CharFormatter workspace = makeWorkspace();
  if(workspace.isUseful()) {
    if(!workspace.printUnsigned(value)) {  //if failed to insert anything
      onFailure(workspace);
    }
    reclaimWaste(workspace);
  }
}

void TextFormatter::substitute(u8 value){
  substitute(u64(value));//need this to distinguish char * from implied char &
} // TextFormatter::substitute

void TextFormatter::substitute(bool value){
  substitute(u64(value));//need this to distinguish char * from implied char &
} // TextFormatter::substitute


void TextFormatter::substitute(const NumberFormat &item){
  //this should not actually get called. we migth still use it to relocate some code from header to here should this ever become more than an assignment.
  nf = item;//copies fields, the original doesn't need to linger.
}

bool TextFormatter::onSizingCompleted(){
  sizing = false;
  sizer = Zguard(sizer);//because we dropped the null on our format string.
  body.wrap(reinterpret_cast<char*>(malloc(sizer)),sizer);  //wrap new allocation
  body.zguard();
  if(body.isUseful()) {  //if malloc worked
    body.cat(c_str(),length());  //copy in present stuff
    dataend=body.used();
    body.clearUnused();  //prophylactic nulling
    clear(); //release original arg.
    ptr = body.internalBuffer();//needed for the eventual 'free'
    return true;
  }

  return false;
} // TextFormatter::onSizingCompleted
