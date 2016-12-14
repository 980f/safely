#include "determinator.h"

/** this is a very simple implementation of the determinant process */

Determinator::Determinator(Gater&ignore, double ys[], Column xs[]): ignorer(ignore){
  Y = ys;
  X = xs;
}

double Determinator::compute(int which, bool yish){
  for(int rw = MaxProblem; rw-- > 0; ) { //exclude extraneous data.
    columner[rw] = rower[rw] = ignorer[rw];
  }
  this->which = which;
  this->yish = yish;
  return descend();
}

double Determinator::descend(void){
  for(int cl = MaxProblem; cl-- > 0; ) {
    if(!columner[cl]) { //1st usable column is one we will sum on
      double summer = 0;
      bool flipper = false;
      columner[cl] = true; //descent ignores the current column
      for(int rw = MaxProblem; rw-- > 0; ) {
        if(!rower[rw]) {
          double term;
          if(yish && which == cl) {
            term = Y[rw];
          } else {
            term = X[cl][rw];
          }
          rower[rw] = true;
          term *= descend();
          rower[rw] = false;
          summer += flipper ? -term : term;
          flipper = !flipper;
        }
      }
      columner[cl] = false;
      return summer;
    }
  }
  return 1.0;//for loop does no inner iterations on final step of descent, which is how we know it is the final step.
} /* descend */
