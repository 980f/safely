#ifndef CLOCKGENERATOR_H
#define CLOCKGENERATOR_H

#include "peripheral.h"



class ClockGenerator : public Peripheral {

public://lacking full documentation these numbers are magic (reg[*2]
  enum For {
    gp0=14,
    gp1,
    gp2,
    pcm=19,
    pwm=20
  };
  ClockGenerator();
  struct Setting {
    bool usePLLD; //1 or PLLD:6
    unsigned divI; //basic linear divider
    unsigned divF; //jitter rate
    enum Dithering: unsigned{
      Off=0,
      Simple, //bounces between two frequencies
      Jitter, //bounces between four frequencies
      Smear   //bounces between eight frequencies
    } MASH;//broadcom's name for the field.
    /** a divI less than this will fault, see lowDiv */
    unsigned mindivi()const{
      return 1+(1<<MASH);//1,2,3,5
    }
    /** @returns the lowest effective divider */
    unsigned lowDiv(){
      return divI+~(1<<(MASH-1)); //0,0,1,3
    }

    /** @returns the highest effective divider */
    unsigned highDiv(){
      return divI+ (1<<MASH);//0,1,2,4
    }
  };

  /** @returns garbage, a side effect of memory ordering operations. (we use the return to force ordering, value is irrelevant to caller)*/
  unsigned init(For which, const Setting &setting);

  /** set divider. MASH mode is selected according to whether the divider is an integer. */
  bool setFreq(For which ,double hz);

  /** available frequencies as input to the divider. */
  static constexpr double coreFrequency(bool pll){
    return pll? 500e6 :19.2E6; //pi3 values. not sure where the oft quoted 250E6 comes from.
  }

  static ClockGenerator module;
};

#endif // CLOCKGENERATOR_H
