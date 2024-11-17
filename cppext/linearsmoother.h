#ifndef LINEARSMOOTHER_H
#define LINEARSMOOTHER_H
#include "minimath.h"
#include "cycler.h"

/**
  * SVG order 1 smoothing of integer equally-spaced data stream
  * templated for filter width for execution efficiency, especially as the
  * 'memory' is part of this object.
  */

template <int hwidth> class LinearSmoother {
  enum{ fullWidth = 1 + 2 * hwidth};

  constexpr static double invS0 = 1.0 / fullWidth;
  constexpr static const double invS1 = 1.0 / ((hwidth + 1) * fullWidth); //== invS0/(hw+1)
  constexpr static const double invS2 = 3.0 / (hwidth * (hwidth + 1) * fullWidth); //==invS0*3/()(+1)

  int memory[fullWidth];

  Cycler phaser;
  int Y0;
  int Y1;

  LinearSmoother(): phaser(fullWidth){
    init(0);//4debug
  }
  /** initialize with a nominal dc value, after 1+2*hwidth updates the value won't matter*/
  void init(int dc){
    Y0 = dc * (fullWidth);
    Y1 = 0;
    for(int i = fullWidth; i-- > 0; ) {
      memory[i] = dc;
    }
  }

//  /** todo: return whether value is reasonable compared to what is expected from smoothing*/
//  bool copacetic(int newvalue){
//    //todo: whether abs(newvalue-now()) is less than some multiple of the std error.
//    return true;
//  }

  void update(int Ynew){
    int Yold = memory[phaser];

    memory[phaser++] = Ynew;
    Y0 -= Yold;
    Y1 -= Y0;
    Y1 += hwidth * (Ynew + Yold);
    Y0 += Ynew;
  }

  double mean(void){
    return Y0 * invS0;
  }

  double drift(void){
    return Y1 * invS2;
  }

  //compute central fit coefficients, then apply them to hwidth
  double now(void){
    //return mean()+drift()*hwidth;
    return (Y0 * (hwidth + 1) + 3 * Y1) * invS1; //same as above, minimizing integer -> float conversions.
  }
};

#endif // LINEARSMOOTHER_H
