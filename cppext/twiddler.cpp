#include "twiddler.h"
#include "minimath.h"

IntegerTwiddler::IntegerTwiddler(){
  freeze();
}

IntegerTwiddler::IntegerTwiddler(u32 numer, u32 denom, bool center){
  setRatio(numer,denom,center);
}

void IntegerTwiddler::setRatio(u32 numer, u32 denom, bool center){
  above = numer;
  below = denom;
  if(center) {
    twiddle = (above - below) / 2;
  } else {
    if(twiddle > below) {
      twiddle = below;
    } else if(twiddle < -above) {
      twiddle = -above;
    }
  }
} /* setRatio */

//a value used to get the maximum precision/resolution available due to the data type of twiddle
const unsigned MaxRes = 0x7FFFFFFF;
//todo:3 above is max pos int, find standard symbol for it.

void Twiddler::setRatio(double ratio){
  if(ratio<=0){
    freeze(false);
  } else if(ratio >= 1.0) {
    IntegerTwiddler::setRatio(MaxRes * ratio, MaxRes);
  } else {
    IntegerTwiddler::setRatio(MaxRes, MaxRes * ratio);
  }
}

double Twiddler::getRatio(){
  return ratio(double(above), double(below)); //use double so as to not lose accuracy on ints near to max.
}

double Twiddler::getDivider()const{
  if(below==0){
    return 0.0;//don't trust ratio(0,0) to stay the same over the ages :)
  }
  return ratio(double(below), double(above));
}

void IntegerTwiddler::setDivider(int rate){
  if(rate<=0){
    freeze(false);
  } else {
    above = 1;
    below = rate;
    twiddle = -rate/2;
  }
}

void IntegerTwiddler::freeze(bool on){
  above = 0;
  below = 0;
  twiddle = on - 1; //#hack
}

bool IntegerTwiddler::pwm(void){
  if(twiddle >= 0) {
    twiddle -= below;
    return true;
  } else {
    twiddle += above;
    return false;
  }
}

/////////////////////
PwmModulator::PwmModulator(){
  quoticks = 0;
}

void PwmModulator::setRatio(double ratio){
  quoticks = unsigned(ratio); //#truncation desired
  Twiddler::setRatio(ratio - quoticks); //todo:3 find math function for getting fraction.
}

double PwmModulator::getRatio(void){
  return Twiddler::getRatio() + quoticks;
}

unsigned PwmModulator::pwm(void){
  return quoticks + Twiddler::pwm();
}

unsigned PwmModulator::quanta(unsigned chunk,bool noFloat){
  if(noFloat){
    //the below ignores the fraction part of dither, in order to not use floating point
    return ::quanta(chunk, quoticks);
  } else {
    return ratio(double(chunk), quoticks+Twiddler::getRatio());
  }
}
