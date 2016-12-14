/** convenience class for interpreting unsigned 16 bit digital values.*/
#ifndef scaler_h
#define scaler_h
#include "eztypes.h"
#include "minimath.h"

class Scaler {
public:
  double gain;
  double offset;
public:
  Scaler(double gain = 0, double offset = 0): gain(gain), offset(offset){
    //defaults leave object unusable. gain=1.0 would make failure to init be too subtle.
  }
  double real(u16 ticks) const {
    return ticks * gain + offset;
  }

  u16 quanta(double real) const {
    double delta = real - offset;

    if(delta <= 0) {
      return 0;
    }
    u32 bins = chunks(delta, gain);
    if(bins > 65535) {
      return 65535;
    }
    return u16(bins);
  } /* quanta */

  Scaler& setto(double gain, double offset){
    this->gain = gain;
    this->offset = offset;
    return *this;
  }

};
#endif /* ifndef scaler_h */
