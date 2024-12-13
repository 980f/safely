#pragma once  //(C) 2017 by Andrew L Heilveil, github/980f

#include "minimath.h"
#include "cycler.h"

/**
  * SVG order 1 smoothing of integer equally-spaced data stream
  * templated for filter width for execution efficiency, especially as the
  * 'memory' is part of this object.
  */

template<unsigned hwidth> class LinearSmoother {
  enum {
    fullWidth = 1 + 2 * hwidth,
    S1 = (hwidth + 1) * fullWidth,
    S2 = hwidth * S1,
  };

  /** the filter can work in integers, with integer scaling parameters that apply to all points and as such can often be ignored.
    'inv' stands for 'inverse' Sx is "sum of integers raised to the power x over the range -hw to +hw".
  */
  constexpr static double invS0 = 1.0 / fullWidth;
  constexpr static double invS1 = 1.0 / S1;
  constexpr static double invS2 = 3.0 / S2;//todo: should the 3 be a 5?


  /** Yx is the sum of the input amplitudes times the xth power of the integer position in the filter.
  Y0 is simple the sum of the inputs over the range, Y1 is the sum with each input multiplied by relative position in the filtered range. 
  */
  int Y0;
  int Y1;
  /** pointer into 'memory'*/
  Cycler phaser;
  /** we are allowing for bipolar input data, although all testing was done with positive-only data. */
  int memory[fullWidth];

  LinearSmoother(): phaser(fullWidth) {
    init(0); //4debug set the memory to all zero, which also lets us set Y0 and Y1 to zero.
  }

  /** initialize with a nominal dc value, after 1+2*hwidth updates the value won't matter*/
  void init(int dc) {
    Y0 = dc * fullWidth;
    Y1 = 0;
    for (int i = fullWidth; i-- > 0;) { //some compilers recognize this as a memset :)
      memory[i] = dc;
    }
  }

  //  /** todo: return whether value is reasonable compared to what is expected from smoothing*/
  //  bool copacetic(int newvalue){
  //    //todo: whether abs(newvalue-now()) is less than some multiple of the std error.
  //    return true;
  //  }

  void update(int Ynew) {
    int Yold = memory[phaser];
    memory[phaser++] = Ynew;
    //order of the following is quite important:
    Y0 -= Yold;
    Y1 -= Y0;
    Y1 += hwidth * (Ynew + Yold);
    Y0 += Ynew;
  }

  /** @returns average of the data. You may wish to multiply this by the quantum of the input integer data. */
  double mean() {
    return Y0 * invS0;
  }

  /** @returns engineering value of slope (1st derivative) at center of timeframe. You may wish to multiply this by the quantum of the input data divided by the unit of time sampling. */
  double drift() {
    return Y1 * invS2;
  }

  /** @returns fitted value of most recent point added, using lagging filter from center of timeframe. */
  double now() {
    //return mean()+drift()*hwidth;
    return (Y0 * (hwidth + 1) + 3 * Y1) * invS1; //same as above, minimizing integer -> float conversions.
  }
};

#endif // LINEARSMOOTHER_H
