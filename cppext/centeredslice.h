#ifndef CENTEREDSLICE_H
#define CENTEREDSLICE_H
#include "buffer.h"
/** a pointer-like thing into an array of integers */
struct CenteredSlice {//python term
protected:
  int *data;
public:
  const int hwidth;
public:
  CenteredSlice(int *data,int hwidth);
  int width()const ;
  int operator [](int i) const;
  int lowest()const;
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
