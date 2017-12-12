#ifndef CONTINUEDFRACTIONRATIOGENERATOR_H
#define CONTINUEDFRACTIONRATIOGENERATOR_H

/** find a ratio of integers which best matches a floating point number.
 * This is useful for tuning a PWM.
 */
class ContinuedFractionRatioGenerator{
public:
  double fraction;
  unsigned an;
  //begin repeated fractions algorithm to get best integer ratio, iteration stopped by exceeding bits of divisor which is 12.
  unsigned h[3];
  unsigned k[3];
  /** how large the numerator or denominator may be.
   * The minimal constraint is to not overflow the computer representation.
   * This is usually a power of 2.
*/
  unsigned limit;
public:
  ContinuedFractionRatioGenerator();
  bool restart(double ratio,unsigned limit=1<<30);//todo: derive 30 from number of bits in unsigned
  bool step();
  unsigned numerator(){
    return h[1];
  }
  unsigned denominator(){
    return k[1];
  }
  double approximation(){
    return double(numerator())/double(denominator());//todo: /0 check.
  }
private:
  bool split();
};

#endif // CONTINUEDFRACTIONRATIOGENERATOR_H
