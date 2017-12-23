#ifndef CONTINUEDFRACTIONRATIOGENERATOR_H
#define CONTINUEDFRACTIONRATIOGENERATOR_H

/** find a ratio of integers which best matches a floating point number.
 * This is useful for tuning a PWM.
 *
 * It sometimes gives something that isn't the absolute best due to numerical error in the 1/double computation.
 * One can extend it with something that computes the estimate on each pass and records the best as the iteration seeks it. That will nearly double the computational burden so we won't do that in this class.
 * Another variation would be a class for trying to reduce a ratio of integers to fit into a smaller integer representation.
 *
 * Note: during development the ratio was easily inverted by minor changes to the initialization, which leans a tad illegitimately on the iterative step to save on some code.
 */
class ContinuedFractionRatioGenerator{
public:
  double fraction;
  unsigned an;
  //begin repeated fractions algorithm to get best integer ratio, iteration stopped by exceeding bits of divisor which is 12.
  unsigned h[3];//recursion relationship refers to 0,-1,-2 indices.
  unsigned k[3];
  /** how large the numerator or denominator may be.
   * The minimal constraint is to not overflow the computer representation.
   * This is usually a power of 2.
*/
  unsigned limit;
public:
  /** create one for learning the algorithm*/
  ContinuedFractionRatioGenerator();

  /** create and iterate. By the time this returns it holds the best ratio. */
  ContinuedFractionRatioGenerator(double ratio,unsigned limit=0){
    ContinuedFractionRatioGenerator generator;
    generator.restart(ratio,limit);
    best();
  }

  /** if @param numerator and denominator will be <limit when iteration stops. for hardware use value should be ((1<<numbits)-1)
   * @param ratio is expected to be positive, if not hilarity ensues. @returns an OK to iterate which is not yet sane, ignore it. */
  bool restart(double ratio,unsigned limit=~0U);

  /** @returns whether the number format allows a more precise computation, if so then computation is done and stored internally.*/
  bool step();

  /** @returns a number greater than denominator if input value is greater than 1 */
  unsigned numerator(){
    return k[1];
  }

  /** @returns a number greater than numerator if input value is less than 1 */
  unsigned denominator(){
    return h[1];
  }

  /** best approximation */
  double approximation(){
    return double(numerator())/double(denominator());//todo: /0 check.
  }

  /** calls steps() until it says to stop then @returns approximation() */
  double best(){
    //we use a counter to guard against bad input parameters. Termination is normally done due to the 'limit' value which is default set to note overflow in the computation.
    for (unsigned steps=32;steps-->0&&step();){}
    return approximation();
  }

private:
  bool split();
  bool bump(unsigned hk[]);
};

#endif // CONTINUEDFRACTIONRATIOGENERATOR_H
