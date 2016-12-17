#include "delimitedinputstream.h"
#include "numberparser.h"

#include <stdlib.h>

//private definition of whitespace, input file may not be local.

static const char *whitespace = "\t\r ";
static bool isWhite(int ch){
  //todo: unicode 'iswhite' trait
  const char *which = strchr(whitespace,ch);
  return which!=nullptr;
}

DelimitedInputStream::DelimitedInputStream(std::istream &s) :
  separator(','),
  s(s),
  lineCount(0),
  dstate(EndOfLine){
  //starting at endOfLine
}

bool DelimitedInputStream::trimTrailing(){
  while(s.good()) {
    Unichar ch(s.get());
    if(ch==separator) {
      dstate = StartOfField;
      return true;
    }
    if(ch=='\n') {
      ++lineCount;
      dstate = EndOfLine;
      return true;
    }
    if(isWhite(ch)) {
      continue;
    }
    //failure, ignore for now:
    {
      s.unget();
      dstate = StartOfField;
      return true;
    }
  }
  return false;
} // DelimitedInputStream::trimTrailing

bool DelimitedInputStream::trimLeading(){
  while(s.good()) {
    Unichar ch(s.get());
    if(ch==separator) {
      dstate = EmptyField;
      return true;
    }
    if(ch=='\n') {
      ++lineCount;
      dstate = EndOfLine;//harrumph-also an empty field
      return true;
    }
    if(isWhite(ch)) {
      continue;
    }
    s.unget();
    dstate = StartOfField;
    return true;
  }
  return false;
} // DelimitedInputStream::trimLeading

bool DelimitedInputStream::hasMoreFields(){
  return dstate!=EndOfLine;
}

bool DelimitedInputStream::hasMoreLines(){
  return trimLeading();
}

bool DelimitedInputStream::endl(){
  if(dstate==EndOfLine) {
    return true; //already there.
  }
  while(s.good()) {
    Unichar ch(s.get());
    if(ch=='\n') {
      dstate = EndOfLine;
      return true;
    }
  }
  return false;
} // DelimitedInputStream::endl

bool DelimitedInputStream::get(double &value){
  if(!&value) {
    return false;
  }
  if(trimLeading()) {
    switch(dstate) {
    case EmptyField:
    case EndOfLine:
      value = 0.0;
      return true;
    case StartOfField:
      s >> value;
      trimTrailing();
      return true;
    default:
      return false;//wtf
    } // switch
  } else {
    return false;
  }
} // DelimitedInputStream::get
