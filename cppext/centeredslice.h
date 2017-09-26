#ifndef CENTEREDSLICE_H
#define CENTEREDSLICE_H
#include "buffer.h"
/** a pointer-like thing into an array of integers */
struct CenteredSlice {//python term
protected:
  /** caller must assure that this points to the center of allocated data */
  int *data;
public:
  /** half width of filter, using int rather than unsigned to get rid of warnings */
  const int hwidth;
public:
  CenteredSlice(int *data,unsigned hwidth);
  unsigned width()const ;
  /** @param i is int, not unsigned as 0 is in the middle of the slice allocation*/
  int operator [](int i) const;
  /** @returns datum at lowest address */
  int lowest()const;
  /** @returns datum at highest address */
  int highest() const;

  Indexer<const int> iterator()const;

  CenteredSlice &step(bool up);
  CenteredSlice offset(int delta)const;
  CenteredSlice subslice(int delta, int hwidth)const;
private:
  void constrainClip(int &clip)const ;
public:
  CenteredSlice Half(bool upper)const;
  CenteredSlice Endpoint(bool upper,int newwidth)const;
};

#endif // CENTEREDSLICE_H
