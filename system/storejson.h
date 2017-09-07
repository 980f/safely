#ifndef STOREJSON_H
#define STOREJSON_H "(C) Andrew L. Heilveil, 2017"

#include "storable.h"
#include "buffer.h"
#include "textpointer.h"

#include "abstractjsonparser.h"

/** must supply and track source data, and be able to extract data via ordinals */
class StoreJsonConstructor: public JsonConstructor<Storable,Text> {
public:
  /** pointer to data source, exposed for module testing convenience */
  Indexer<char> data;
  StoreJsonConstructor(Indexer<char> &data);

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

struct StoreJsonParser: public AbstractJSONparser<Storable, Text> {
  StoreJsonConstructor core;
  StoreJsonParser(Indexer<char>&data);
};

#endif // STOREJSON_H
