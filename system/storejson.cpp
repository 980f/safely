#include "storejson.h"
#include "utf8.h"
#include "textpointer.h"

//static
//UTF8 skipwhite(CharScanner &scanner){
//  while (scanner.hasNext()) {//space leading name or value
//    UTF8 ch = scanner.next();
//    if(!ch.isWhite()) {
//      return ch;
//    }
//  }
//  return 0;
//}

//JSONparser::JSONparser(char *buffer, unsigned length, Storable &parent) :
//  token(buffer,length),
//  lookahead(buffer,length),
//  parent(parent){
//  parse();
//}

bool StoredJSONparser::parse(){
  parent=parseChild(nullptr);
  return parent!=nullptr;
}

//TextKey StoredJSONparser::terminateField(){
//  TextKey token.
//}


Storable *StoredJSONparser::makeNamelessChild(Storable *parent){
  if(parent){
    return &parent->addChild("");
  } else { //is root node
    return new Storable("");
  }
}

Storable *StoredJSONparser::makeChild(Storable *parent){
  TextKey name=token.internalBuffer();
  if(parent){
    return &parent->addChild(name);
  } else { //is root node
    return new Storable(name);
  }
}


Storable *StoredJSONparser::parseChild(Storable *parent){
  enum {Start, TextEnd, QuotedEnd, NumberEnd, SeekTerminator, Recover } seeking=Start;
  Storable *nova=nullptr;
  token=lookahead;//mark start
  //look for name
  while(lookahead.hasNext()){
    UTF8 ch=lookahead.next();
    switch (seeking) {
    case Start:
      if(ch.isWhite()){
        continue; // don't advance token pointer
      } else if(ch.is('"')){
        token.trimLeading(lookahead.used());
        seeking=QuotedEnd;
      } else if(ch.startsName()){//name or text value
        token.trimLeading(lookahead.used()-1);
        seeking=TextEnd;
      } else if(ch.startsNumber()){//nameless numerical value
        token.trimLeading(lookahead.used()-1);
        seeking=NumberEnd;
      } else if(ch.is('{')){//begin wad value
        if(nova){
          //named wad
        } else {
          //nameless wad
          nova=makeNamelessChild(parent);
        }
        while(Storable *child=parseChild(nova)){
          if(lookahead.previous()=='}'){
            break;
          }
        }
        return nova;
      }
      break;
    case QuotedEnd:
      if(ch.is('"')){
        //found end
        lookahead.previous()=0;//null terminate the string
        seeking=SeekTerminator;
      }
      break;
    case TextEnd:

      if(!ch.isAlnum()){
        lookahead.previous()=0;//null terminate the string
        seeking=SeekTerminator;
      }
      break;
    case NumberEnd:
      if(!ch.isNum()){
        lookahead.previous()=0;//null terminate the string
        seeking=SeekTerminator;
      }
    case SeekTerminator:
      if(ch.isWhite()){
        continue; // don't advance token pointer
      } else if(ch.is(':')){ //end name
        if(nova){
          //failed!
          seeking=Recover; //try to resync somehow, like seek end of line
          nova->setQuality(Storable::Quality::Empty);
          break;
        } else { //have a name
          nova=makeChild(parent);
        }
      } else if(ch.in("},")){
        if(nova){//end named value

        } else {//end nameless value

        }
      }

      break;
    }
    if(ch.startsName()){

    }
  }
  //while lookahead is white keep looking
  //if not white then
  //capture it, and create node
  //no name, must be an array node, create nameless one.

  //look for value
  //if wad then loop
  return nova;
}

StoredJSONparser::StoredJSONparser(const CharScanner &loaded, Storable *parent):
  token(loaded,~0),
  lookahead(loaded,~0),
  parent(parent)
{
  //just store info, don't parse until we are in a position to make Storables.
}
