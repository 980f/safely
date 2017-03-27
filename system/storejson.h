#ifndef STOREJSON_H
#define STOREJSON_H

#include "storable.h"
#include "buffer.h"
#include "textpointer.h"

#include "abstractjsonparser.h"

/** must supply and track source data, and be able to recover it from values of ordinal */
class StoreJsonConstructor: public JsonConstructor<Storable,Text> {
public:
  /** pointer to data source, exposed for module testing convenience */
  Indexer<const char> data;
  StoreJsonConstructor(Indexer<const char> &data);

  virtual ~StoreJsonConstructor()=default;

  bool hasNext(void) override{
    return data.hasNext();
  }

  char next(void) override {
    return data.next();
  }

  /** @returns text according to saved stream position info from@param span */
  Text extract(Span &span)override;

  /** name and value are here, make a new node.
   * if parent is null then create node out of the blue and record it in root, else add as child to the parent */
  Storable *insertNewChild(Storable *parent,Text &name,bool haveValue,Text &value,bool valueQuoted) override;

  /** Illegal character encountered */
  void exclaim(PushedJSON::Parser::Diag &d) override;

};

class StoreJsonParser: public AbstractJSONparser<Storable, Text> {
public: //accessing root when done, should also set it for some usages.
  StoreJsonConstructor core;
public:
  StoreJsonParser(Indexer<const char> &data);
} ;

#endif // STOREJSON_H
