#ifndef ARGSET_H
#define ARGSET_H

#include "buffer.h"
#include "safely.h"       //ArgsPerMessage maximum copyable set

/** used as an intermediate representation of a struct made of only numbers (no text) */
class ArgSet : public Indexer<double> {
public:
/** wrap an existing array of doubles */
  ArgSet(double *d, unsigned sizeofd);
/** @return whether @param changed*/
  bool applyto(double&d);
/** @return whether @param changed*/
  bool applyto(int&eye);
/** @return whether @param changed*/
  bool applyto(bool&b);
/** append an array of const data, can't just wrap them without lots of syntax*/
  void cat(const double *prefilled,int qty);
/** @returns whether this and @param args are same size, and whether corresponding entries are nearly equal.
 *  @see nearly for @param bits value */
  bool equals(const ArgSet &args, int bits = 18) const;
  //lint makes me do this:
  virtual ~ArgSet();
}; // class ArgSet

#define makeArgs(qty) double argv[qty]; fillObject(argv, sizeof(argv), 0); ArgSet args(argv, sizeof(argv))

//the following macros expect you to have #defined ArgsPerMessage, which is usually done in art.h
#define MessageArgs makeArgs(ArgsPerMessage)
//for those rare occasions where two guys are in play at the same time.
#define MessageArgs2 double argv2[ArgsPerMessage]; ArgSet args2(argv2, sizeof(argv2))


/** appears to be incomplete, need to look for usages */
class ConstArgSet : public Indexer<const double> {
public:
  ConstArgSet(const double *d, int sizeofd);
  ConstArgSet(const ArgSet &other);
  ConstArgSet(const ConstArgSet &other);
};


#endif // ARGSET_H
