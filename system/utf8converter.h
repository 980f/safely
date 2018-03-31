#ifndef UTF8CONVERTER_H
#define UTF8CONVERTER_H
/** mate utf8 logic to Indexer<char>
*/
#include "converter.h"
#include "utf8.h"
#include "utf8transcoder.h"

/** utf8 to \u etc. */
class Utf8ConverterOut: public Converter {
public:
  virtual ~Utf8ConverterOut()=default;
  /** length required by conversion of source */
  unsigned length(const char * source) const override ;
  /** convert source into @param packer */
  bool operator()(const char * source,Indexer<char> &packer) override ;
};

/** escape to utf8 */
class Utf8ConverterIn: public Converter {
public:
  virtual ~Utf8ConverterIn()=default;
  /** length required by conversion of source */
  unsigned length(const char * source) const override;
  /** convert source into @param packer */
  bool operator()(const char * source,Indexer<char> &packer) override ;
};

/** class for sequentially accessing unicode chars (32 bits each) from utf8 encoded stream */
class UnicharScanner:public ReadonlySequence<Unichar> {
  /** utf8 data source*/
  Indexer<const char> utf8;
  /** utf8 to unicode state machine */
  Utf8Escaper ex;
public:
  virtual ~UnicharScanner()=default;//there is a virtual base class.
  /** for @param rewind see Indexer class, ~0 if accessing already used, 0 for the whole thing 1 for backup and then remainder of @param utf8 */
  UnicharScanner(const Indexer<const char>&utf8, int rewind = 0);
  /** @returns whether there is another unicode char available to be read via @see next() */
  bool hasNext(void) override;
  /** @returns unicode char, consumeing */
  Unichar next(void) override;
};

/** from escaped sequence return unicode chars */
class UnicharReader:public ReadonlySequence<Unichar> {
  /** utf8 data source*/
  Indexer<const char> utf8;
  /** utf8 to unicode state machine */
  Utf8Decoder ex;

  // ReadonlySequence interface
public:
  virtual ~UnicharReader()=default;
  bool hasNext() override;
  Unichar next() override;
};

#endif // UTF8CONVERTER_H
