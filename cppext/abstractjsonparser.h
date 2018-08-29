#ifndef ABSTRACTJSONPARSER_H
#define ABSTRACTJSONPARSER_H "(C) 2017 Andrew L. Heilveil"

/** an abstract json parser.
 *  @see StoredJSONparser.
todo:1 replace template with abstract base classes.
*/

#include "pushedjsonparser.h"
#include "sequence.h"
#include "localonexit.h"

template <typename Storable, typename TextClass> class AbstractJSONparser;//forward ref for friendliness

/** base class for data source and sink for parsing */
template <typename Storable, typename TextClass> class JsonConstructor {
  friend class AbstractJSONparser<Storable, TextClass>;
public: //needs accessor
  /** parent of all that is parsed. It can be supplied or the insertNewChild must set it when given a null parent */
  Storable *root=nullptr;
  bool treatRootSpecial=true;//for loading a file onto an existing node we want the file content to be wrappped with braces that do not cause a new child to be made.
protected:
  /** must supply and track source data, and be able to recover it from values of ordinal */
  virtual bool hasNext(void) = 0;
  virtual char next(void) = 0;

  /** @returns an object suitable for passing to insertNewChild */
  virtual TextClass extract(Span &span)=0;
  /** name and value are here, apply to node, creating a new child as needed.
   * if parent is null then use root as parent (deals with parsing top element in a file)*/
  virtual Storable *applyToChild(Storable *parent,TextClass &name,bool haveValue,TextClass &value,bool valueQuoted)=0;
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
    while(parseChild(data.root)){
      //todo: added looping here to deal with the given node already having seen its opening brace.
    }
  }

  /** some useless info about the parsed data */
  JsonStats stats;

protected:
  JsonConstructor<Storable, TextClass> &data;
public: //to expose rules, add a tunnel for that.
  PushedJSON::Parser parser;
protected:
  Storable *assembleItem(Storable *parent,bool evenIfEmpty=false){
    Storable *nova=nullptr;
    //we have a value if it was empty quotes or nonTrivial content. JSON null is an anonymous value at this layer of the parser.
    bool haveValue= parser.wasQuoted || !parser.value.empty();
    if(evenIfEmpty || haveValue){ //checking haveValue here ignores extraneous ',' in the source
      TextClass name(parser.haveName?data.extract(parser.name):"");
      TextClass value(data.extract(parser.value));
      nova=data.applyToChild(parent,name,haveValue,value,parser.wasQuoted);
      stats.onNode(haveValue);
    }
    parser.itemCompleted();//ensure we don't reuse old data on next item.
    return nova;
  }

  /** @returns whether there are more children, for recursively parsing wads. This exists for the BeginWad clause herein. */
  bool parseChild(Storable *parent){
    JsonStats::DepthTracker doe(stats);
    //look for name
    while(data.hasNext()) {
      switch (parser.next(data.next())) {

      case PushedJSON::BeginWad: {//open brace encountered
        //special treatment for node that matches top level of file
        Storable *nova = ((parent==data.root)&&flagged(data.treatRootSpecial))? parent : assembleItem(parent,true);
        if(parser.orderedWad && nova){
          nova->isOrdered=true;
          nova->parserstate=0;//next item will be 0th item.
        }
        while(parseChild(nova)) {
          //#recurse while there are more to be found
        }
      } return true; //end of this wad isn't the same as end of possibly enclosing wad.

      case PushedJSON::EndItem:  //comma between siblings
        assembleItem(parent);
        return true;

      case PushedJSON::Done:
        //we probably don't get this as the 'while' will exit instead of passing an EOF to the parser.
        //#JOIN to try to finish off a trailing not quite closed wad. If multiple open ???
      case PushedJSON::EndWad:   //closing wad
        assembleItem(parent);
        //can detect mismatched brace types here.
        if(!parser.orderedWad && parent->parserstate!=BadIndex){
          //end curly used with open square
        }
        parent->parserstate=BadIndex;
        return false;

      case PushedJSON::Illegal: //unexpected char
        data.exclaim(parser.d);
        break;
      case PushedJSON::Continue:
        continue;
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
