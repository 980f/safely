#ifndef TESTABSTRACTJSONPARSER_H
#define TESTABSTRACTJSONPARSER_H

#include "abstractjsonparser.h"
#include "storable.h"
#include "buffer.h"
#include "textpointer.h"

/** duplicate StoredJSONparser using the later created abstract base class */



class TestAbstractJsonConstructor: public JsonConstructor<Storable,Text>
{
public:
  Indexer<char> data;

  TestAbstractJsonConstructor(Indexer<char> &data);
  virtual ~TestAbstractJsonConstructor()=default;

  /** must supply and track source data, and be able to recover it from values of ordinal */
  bool hasNext(void) override{
    return data.hasNext();
  }
  char next(void) override {
    return data.next();
  }

  /** @returns an object suitable for passing to insertNewChild */
  Text extract(Span &span)override;

  /** name and value are here, make a new node.
   * if parent is null then create node out of the blue and record it in root, else add as child to the parent */
  Storable *applyToChild(Storable *parent,Text &name,bool haveValue,Text &value,bool valueQuoted) override;

  /** Illegal character encountered */
  void exclaim(PushedJSON::Parser::Diag &d) override;

};

class TAJParser: public AbstractJSONparser<Storable, Text> {
public: //accessing root when done, should also set it for some usages.
  TestAbstractJsonConstructor core;
public:
  TAJParser(Indexer<char> &data);
} ;

#endif // TESTABSTRACTJSONPARSER_H
