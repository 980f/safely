#ifndef MARKREPORT_H
#define MARKREPORT_H

#include "argset.h"
#include "hassettings.h"
struct MarkReport :public Settable{
  int onMark; //wasActive;
  int width;//netwidth();
  int skew;//netskew();
  int edge[4]; //edge[direc][polar];

  int numParams()const{
    return 7;
  }
  bool setParams(ArgSet&args);
  void getParams(ArgSet&args)const;
};

#endif // MARKREPORT_H
