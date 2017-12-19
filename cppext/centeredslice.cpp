#include "centeredslice.h"
#include "ignoresignwarnings.h"

CenteredSlice::CenteredSlice(int *data, unsigned hwidth):data(data),hwidth(hwidth){}

unsigned CenteredSlice::width()const {
  return 1+2*hwidth;
}

int CenteredSlice::operator [](int i) const{
  if(i<-hwidth){
    return 0;
  }
  if(i > hwidth){
    return 0;
  }
  return data[i];
}

int CenteredSlice::lowest()const{
  return data[-hwidth];
}

int CenteredSlice::highest() const {
  return data[hwidth];
}

Indexer<const int> CenteredSlice::iterator()const{
  return Indexer<const int>(&data[-hwidth],sizeof(int)*(2*hwidth+1));
}

CenteredSlice &CenteredSlice::step(bool up){
  if(up){
    ++data;
  } else {
    --data;
  }
  return *this;
}

CenteredSlice CenteredSlice::offset(unsigned delta)const{
  return CenteredSlice(data+delta,hwidth);
}

CenteredSlice CenteredSlice::subslice(int delta, unsigned hwidth)const{
  //todo:1 ensure subslice is inside original one!
  return CenteredSlice(data+delta,hwidth);
}

void CenteredSlice::constrainClip(unsigned &clip)const {
  if(clip==BadLength){//signal value
    clip=hwidth;
  }
  if(clip>hwidth){//half-hearted attempt to stay inside bounds
    clip=hwidth;
  }
}


CenteredSlice CenteredSlice::Half(bool upper)const{
  //    constrainClip(clip);
  unsigned newwidth=half(hwidth);
  return subslice((upper?newwidth:-newwidth),newwidth);
}

CenteredSlice CenteredSlice::Endpoint(bool upper, unsigned newwidth)const{
  constrainClip(newwidth);
  return subslice(upper ? hwidth : -hwidth, newwidth);
}
