#ifndef STREAMFORMATTER_H
#define STREAMFORMATTER_H

#include <iostream> //todo: baser stream

class StreamFormatter {
protected:
  std::ios &stream;

  /** for controlling the scope of application of formatting. */
  struct StreamState {
    std::ios_base::fmtflags flags;
    std::streamsize width;
    std::streamsize precision;
    char fill;
    void record(std::ios &stream);
    void restore(std::ios &stream);
  };
  //initial implementation is one save and restore per print session:
  StreamState pushed;

//format string parse states and values:
  bool parsingIndex;
  unsigned argIndex;

  bool parsingFormat;
  unsigned formatValue;

  bool slashed;//next char is fill char
  bool invertOption;
  bool keepFormat;
  //format parse state transition functions:
  /** resets all state to 'no format spec seen', records stream state for later optional restoration. */
  void beginParse();

  /** called after item has been printed, to avert bugs */
  void dropIndex();

/** called after parsing of format is done and any options have been applied */
  void dropFormat();

/** called after 'have format field' has been recognized */
  void startFormat();

/** @returns whether c is a digit,and if so then it has applied it to the accumulator  */
  bool appliedDigit(char c, unsigned &accumulator);
  /** call when the format commands has been fully acted upon, IE after responding to 'DoItem' */
  void afterActing();
  /** begin parsing the item number*/
  void startIndex();
  /** format value has been used, avert applying it to another feature */
  void clearFormatValue();

protected://now for the API:
  enum Action {
    FeedMe, //feed parser more characters
    Pass,   //process the char, for printing: output it; for scanf compare it
    Escaped,//the char was an escape char, e.g. if c==n then emit a newline.
    DoItem, //end of format field, act upon it.
  };
  /** inspects format character @param c and @returns what to do */
  Action applyItem(char c);

// the following did not work as the ostream inherited virtually from the base class that actually had the methods needed:
//  typedef std::streamsize (std::ostream::*Attributor)(std::streamsize);
//  void applyFormat(Attributor func);
  enum FormatValue {Widthly, Precisely};
  void applyFormat(FormatValue whichly);

  /** attach to a stream */
  StreamFormatter(std::ios &ostr);
};
#endif // STREAMFORMATTER_H
