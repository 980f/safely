
#pragma once
/** convenience class for interpreting unsigned 16 bit digital values.*/
#include <cstdint>
#include "minimath.h" //chunks

class Scaler {
public:
  double gain;
  double offset;
public:
  Scaler(double gain = 0, double offset = 0): gain(gain), offset(offset){
    //defaults leave object unusable. gain=1.0 would make failure to init be too subtle.
  }
  double real(uint16_t ticks) const {
    return ticks * gain + offset;
  }

  /** limiting to 16 bits as this class was made for real world ADC/DAC interfacing */
  uint16_t quanta(double real) const {
    double delta = real - offset;

    if(delta <= 0) {
      return 0;
    }
    //todo:1 we have this 'saturate to unsigned 16 somewhere in our libraries
    auto bins = chunks(delta, gain);
    if(bins > 65535) {
      return 65535;
    }
    return uint16_t(bins);
  } /* quanta */

  Scaler& setto(double gain, double offset){
    this->gain = gain;
    this->offset = offset;
    return *this;
  }

};
