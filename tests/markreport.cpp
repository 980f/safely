#include "markreport.h"

void MarkReport::getParams(ArgSet &args)const{
  args.next() = onMark;
  args.next() = width;
  args.next() = skew;
  for(int i=countof(edge); i-- > 0; ) {
    args.next() = edge[i];
  }
}

bool MarkReport::setParams(ArgSet &args){
  set(onMark,args);
  set(width,args);
  set(skew,args);
  for(int i=countof(edge); i-- > 0; ) {
    set(edge[i],args);
  }
  return isModified();
}
