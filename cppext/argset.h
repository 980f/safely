#ifndef ARGSET_H
#define ARGSET_H
//ArgsPerMessage maximum copyable set
#include "index.h"
#include "buffer.h"

/** used as an intermediate representation of a struct made of only numbers (no text) */
class ArgSet : public Indexer<double> {
public:
/** wrap an existing array of doubles */
  ArgSet(double *d, unsigned sizeofd);

  ArgSet(const ArgSet &other, int portion);

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
  virtual ~ArgSet()=default;
}; // class ArgSet


/** replacing macro with real class */
template<unsigned qty> class ArgBlock : public ArgSet {
  double argv[qty];

public:
  ArgBlock() : ArgSet(argv,sizeof(argv)){
  }

  /** @returns a reference to a value, the first if arg is bad. */
  double &operator [](unsigned which){
    return argv[which<qty ? which : 0];
  }

  void reset(){
    wrap(argv,sizeof(argv));
  }

}; // class ArgBlock

/** exists so that we can wrap const arrays of doubles for firmware rom based config. */
class ConstArgSet : public Indexer<const double> {
public:
  ConstArgSet(const double *d, unsigned numArgs);
  ConstArgSet(const ArgSet &other);
  ConstArgSet(const ConstArgSet &other);
  ~ConstArgSet() = default;
};

//FakeArgs takes a root name and creates a const array and a wrapper for it: nameArgs
#define FakeArgs(name, ...)\
static constexpr double name##Scaling[] = { __VA_ARGS__ };\
static ConstArgSet name##Args(&name##Scaling[0], sizeof(name##Scaling))


#endif // ARGSET_H
