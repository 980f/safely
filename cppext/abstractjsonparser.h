#ifndef ABSTRACTJSONPARSER_H
#define ABSTRACTJSONPARSER_H

/** an abstract json parser.
 * One migh use this as a base class, although that might be more complex then copy-pasting from a concrete instance such as @see StoredJSONparser */

#include "pushedjsonparser.h"
#include "sequence.h"
#include "localonexit.h"




template <typename Storable, typename TextClass> class AbstractJSONparser;//forward ref for friendliness
/** data source and sink for parsing */
template <typename Storable, typename TextClass>
class JsonConstructor {
  friend class AbstractJSONparser<Storable, TextClass>;
public: //needs accessor
  /** parent of all that is parsed. It can be supplied or the insertNewChild must set it when given a null parent */
  Storable *root=nullptr;
protected:
  /** must supply and track source data, and be able to recover it from values of ordinal */
  virtual bool hasNext(void) = 0;
  virtual char next(void) = 0;

  /** @returns an object suitable for passing to insertNewChild */
  virtual TextClass extract(Span &span)=0;
  /** name and value are here, make a new node.
   * if parent is null then create node out of the blue and record it in root, else add as child to the parent */
  virtual Storable *insertNewChild(Storable *parent,TextClass &name,bool haveValue,TextClass &value,bool valueQuoted)=0;
  /** Illegal character encountered */
  virtual void exclaim(PushedJSON::Parser::Diag &){
//    wtf("Bad char 0x%02X at row:%u, col:%u, offset:%u",d.last,d.row,d.column,d.location);
  }
};

/** non-virtual part of implementation */
template <typename Storable, typename TextClass> class AbstractJSONparser {
public:
  /** parse a block of text into a child of the given node. */
  AbstractJSONparser(JsonConstructor<Storable, TextClass> &data):data(data){}

  /** process the block */
  void parse(){
    parseChild(data.root);
  }

  JsonStats stats;

protected:
  JsonConstructor<Storable, TextClass> &data;
  PushedJSON::Parser parser;

  Storable *assembleItem(Storable *parent,bool evenIfEmpty=false){
    Storable *nova=nullptr;
    //we have a value if it was empty quotes or nonTrivial content. JSON null is an anonymous value at this layer of the parser.
    bool haveValue= parser.wasQuoted || !parser.value.empty();
    if(evenIfEmpty || haveValue){ //checking haveValue here ignores extraneous ',' in the source
      TextClass name(parser.haveName?data.extract(parser.name):"");
      TextClass value(data.extract(parser.value));
      nova=data.insertNewChild(parent,name,haveValue,value,parser.wasQuoted);
      stats.onNode(haveValue);
    }
    parser.itemCompleted();//ensure we don't reuse old data on next item.
    return nova;
  }

  /** @returns whether there are more children, for recursively parsing wads. */
  bool parseChild(Storable *parent){
    JsonStats::DepthTracker doe(stats);
    //look for name
    while(data.hasNext()) {
      switch (parser.next(data.next())) {
      case PushedJSON::BeginWad: //open brace encountered
        for(Storable *nova = assembleItem(parent,true); parseChild(nova); ) {
          //#recurse while there are more to be found
        }
        return true; //end of this wad isn't the same as end of possibly enclosing wad.

      case PushedJSON::EndItem:  //comma between siblings
        assembleItem(parent);
        return true;

      case PushedJSON::EndWad:   //closing wad
        assembleItem(parent);
        return false;

      case PushedJSON::Illegal: //unexpected char
        data.exclaim(parser.d);
        break;
      default:
        break;//to stifle warnings
      } // switch
    }
    auto finial=parser.next(0);
    //at end of file we might have one last item, especially if we only have one item
    if(finial==PushedJSON::EndItem){
      assembleItem(parent);
    }
    return false; //no more data so there are no more children,
  }

};

#endif // ABSTRACTJSONPARSER_H
