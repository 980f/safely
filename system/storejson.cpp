#include "storejson.h"
#include "textpointer.h"


bool StoredJSONparser::parse(){
  parseChild(nullptr);
  return root!=nullptr;
}

Storable *StoredJSONparser::insertNewChild(Storable *parent,TextKey name){
  ++totalNodes;
  if(parent){
    return &parent->addChild(name);
  } else { //is root node
    return root=new Storable(name);
  }
}

Storable *StoredJSONparser::makeNamelessChild(Storable *parent){
  return insertNewChild(parent,"");
}

Storable *StoredJSONparser::makeChild(Storable *parent){
  Text name(data.internalBuffer(),parser.name.begin,parser.name.end);
  return insertNewChild(parent,name);
}

void StoredJSONparser::setValue(Storable &nova){
  Text value(data.internalBuffer(),parser.value.begin,parser.value.end);
  nova.setImage(value,Storable::Parsed);
  nova.setType(Storable::Uncertain);//mark for deferred interpretation
}

bool StoredJSONparser::handleNameEnd(Storable *&nova, Storable *parent){
  if(nova){ //failed!  name1: name2: xxxx
    seeking=Recover; //try to resync somehow, like seek end of line
    nova->setQuality(Storable::Quality::Empty);
    return true;
  } else { //have a name
    nova = makeChild(parent);
    return false;
  }
}

bool StoredJSONparser::handleWadStart(Storable *&nova, Storable *parent){
  if(nova){
    //named wad
  } else {
    //nameless wad
    nova=makeNamelessChild(parent);
  }
  while(parseChild(nova));
  return false;
}

bool StoredJSONparser::handleValueEnd(Storable *&nova, Storable *parent, char termchar){
  if(nova==nullptr){//if node doesn't exists
    nova=makeNamelessChild(parent);
  }

  return termchar==',';
}

bool StoredJSONparser::onTerminator(char termchar,Storable *&nova,Storable *parent){
  UTF8 ch(termchar);
  if(ch.is(':')){ //end name
    if(handleNameEnd(nova,parent)){
      return true;//only get here on defect
    }
  } else if(ch.in("},")){
    //nameless member of parent wad
  }
  return ch.is(',');
}

bool StoredJSONparser::parseChild(Storable *parent){
  maxDepth.inspect(++nested);//heuristics

  Storable *nova=nullptr;
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
        return handleWadStart(nova,parent);
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
      if(!ch.numAlpha()){
        lookahead.previous()=0;//null terminate the string
        if(ch.is(':')){ //end name
          if(handleNameEnd(nova,parent)){
            return nova;//on parsing failure give up here.
          }
        }
        if(ch.isWhite()){
          seeking=SeekTerminator;
        } else {

        }
      }
      break;
    case NumberEnd:
      if(!ch.isInNumber()){
        lookahead.previous()=0;//null terminate the string
        seeking=SeekTerminator;
      }
    case SeekTerminator:
      if(ch.isWhite()){
        continue; // don't advance token pointer
      }

      if(ch.is(':')){ //end name
        if(handleNameEnd(nova,parent)){
          return true;//only get here on defect
        }
      } else if(ch.in("},")){
        //nameless member of parent wad
      }

      break;
    case Recover:
      if(ch.in("\n\r")){
        //todo: null node with initial name
        return false;
      }
      break;
    }
  }
  return nova;
}

StoredJSONparser::StoredJSONparser(const CharScanner &loaded, Storable *parent):
  data(loaded,~0),
  root(parent)
{
  //just store info, don't parse until we are in a position to make Storables.
}
