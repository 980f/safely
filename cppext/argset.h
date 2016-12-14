#ifndef ARGSET_H
#define ARGSET_H

#include "buffer.h"
//formerly inlined code? #include "cheapTricks.h"
#include "safely.h"   		//#library options
class ArgSet : public Indexer <double> {
public:
  ArgSet(double *d, int sizeofd);
  /**@return whether @param changed*/
  bool applyto(double&d);
  /**@return whether @param changed*/
  bool applyto(int&eye);
/**@return whether @param changed*/
  bool applyto(bool&b);
  /** add an array of const data, can't just wrap them without lots of syntax*/
  void cat(const double *prefilled,int qty);
  bool equals(ArgSet args)const;//#yes, construct/copy on call else all callers must 'freeze'
};

#define makeArgs(qty) double argv[qty]; fillObject(argv, sizeof(argv), 0); ArgSet args(argv, sizeof(argv))

//the following macros expect you to have #defined ArgsPerMessage, which is usually done in art.h
#define MessageArgs makeArgs(ArgsPerMessage)
//for those rare occasions where two guys are in play at the same time.
#define MessageArgs2 double argv2[ArgsPerMessage]; ArgSet args2(argv2, sizeof(argv2))


class ConstArgSet : public Indexer <const double> {
public:
  ConstArgSet(const double *d, int sizeofd);
  ConstArgSet(const ArgSet &other);
  ConstArgSet(const ConstArgSet &other);

//  bool equals(ArgSet args)const;//#yes, construct/copy on call else all callers must 'freeze'
};



#endif // ARGSET_H
