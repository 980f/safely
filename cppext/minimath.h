#ifndef minimath_h
#define minimath_h

/**
math related functions that either had platform/compiler variations or have platform specific optimizations (such as implementing in assembly code using processor specific instructions).
*/


#include "eztypes.h"

//portable nan etc. symbols, our compilers don't seem to agree on these guys, or the syntax is horrible.
const extern double Infinity;
const extern double Nan;
/** specifically nan, not infinity, @see isSignal() */
bool isNan(double d);
/** isNan(int) exists to allow templates to use isNan for floats without fancy type testing*/
inline bool isNan(int){
  return false;
}

inline bool isNan(unsigned){
  return false;
}

/** is a normalized fp number, excludes zero and signals */
bool isNormal(double d);
/** is not a value */
bool isSignal(double d);

/** Note: 'signbit' is a macro in math.h that pertains only to floating point arguments
  * @returns sign of operand, and converts operand to its magnitude, MININT(0x800...) is still MININT and must be interpreted as unsigned to work correctly
  */
template <typename Numerical> int signabs(Numerical &absolutatus) {
  if(absolutatus < 0) {
    absolutatus = -absolutatus;
    return -1;
  }
  return absolutatus ? 1 : 0;
}

/** Note: 'signbit' is a macro in math.h that pertains only to floating point arguments
  * @returns sign of operand, and converts operand to its magnitude, MININT(0x800...) is still MININT and must be interpreted as unsigned to work correctly
  */
template <typename Numerical> Numerical absvalue(Numerical absolutatus, int *sign=nullptr) {
  if(absolutatus < 0) {
    if(sign){
      *sign = -1;
    }
    return -absolutatus;
  }
  if(sign){
    *sign=absolutatus ? 1 : 0;
  }
  return absolutatus;
}


//yet another filter to reconcile platform math.h issues. Make specializations per platform for performance.
template< typename mathy > int signof(mathy x) {
  if(x < 0) {
    return -1;
  }
  if(x != 0) {//using != instead of > makes NaN's positive instead of 0
    return +1;
  }
  return 0;
}

/** legacy */
inline int signum(int anint) {
  return signof(anint);
}

/** @returns positivity as a multiplier */
inline int polarity(bool positive){
  return positive?1:-1;
}

/** @return negative if lhs is < rhs, 0 if lhs==rhs, +1 if lhs>rhs .
to sort ascending if returns + then move lhs to higher than rhs.
*/
template< typename mathy > int compareof(mathy lhs,mathy rhs) {
  return signof(lhs-rhs);
}

/** 'round to nearest' ratio of integers*/
template <typename Integer> Integer rate(Integer num, Integer denom) {
  if(denom == 0) {
    return num == 0 ? 1 : 0; //pathological case
  }
  return (num + (denom / 2)) / denom;
}

inline unsigned half(unsigned sum) {
  return (sum + 1) / 2;
}

//#rate() function takes unsigned which blows hard when have negative numbers
inline int half(int sum) {
  if(sum<0){//truncate towards larger magnitude
    return -half(-sum);//probably gratuitous but we also shouldn't be calling this with negatives so we can breakpoint here to detect that.
  }
  return (sum + 1) / 2;
}


/** quantity of bins needed to hold num items at denom items per bin*/
template <typename Integer,typename Inttoo> Integer quanta(Integer num, Inttoo denom) {
  if(denom == 0) {
    return num == 0 ? 1 : 0; //pathological case
  }
  return (num + denom - 1) / denom;
}

/** protect against garbage in (divide by zero) note: 0/0 is 1*/
inline double ratio(double num, double denom) {
  if(denom == 0) { //pathological case
    return num; //attempt to make 0/0 be 1 gave us 1.0 cps for unmeasured spectra  may someday return signed inf.
  }
  return num / denom;
}

/** protect against garbage in (divide by zero) note: 0/0 is 0, at one tim ethis returned 1 for that.*/
inline float ratio(float num, float denom) {
  if(denom == 0) { //pathological case
    return num;// == 0 ? 1 : 0; //may someday return signed inf.
  }
  return num / denom;
}

/** @returns whether d is a numerical value, excludes signalling values but includes zero.*/
bool isSignal(double d);

/** quantity of bins needed to hold num items at denom items per bin*/
u32 chunks(double num, double denom);

/** round to a quantum, to kill trivial trailing DECIMAL digits*/
inline double rounder(double value, double quantum) {
  return quantum * chunks(value, quantum);
}

/** @returns canonical value % cycle, minimum positive value
  0<= return <cycle;
  Note: the C '%' operator gives negative out for negative in.
*/
int modulus(int value, unsigned cycle);

/** @param accum is reduced to a number less than @param length, @returns the number of subtractions that were necessary to do so.
 * Named for use in reporting rotary position from encoder without an index pulse to pick out the revolutions.
*/
template<typename Integrish, typename Integrash> Integrish revolutions(Integrish &accum, Integrash length){
  if(length==0){
    return accum;
  }
  //todo: see if std::div or std::remquo can be applied here, for greater portability or whatever.
  Integrish cycles=accum/length;
  accum=accum%length;
  return cycles;
}

/** standard math lib's f_r_exp does a stupid thing for some args, we wrap it here and fix that.*/
int fexp(double d) ISRISH;

/** @returns whether the difference of the two numbers is less than a power of two times the lesser of the two. */
template <typename floating> bool nearly(floating value, floating other, int bits = 32) {
  if(isSignal(value) &&isSignal(other)){//deals with nan's and inf's and also frequent cases such as comparing zero's.
    return true;
  }
  floating diff = value - other;
  if(diff == 0.0) { //frequent case, and handles many pathologies as well
    return true;
  }
  int f1 = fexp(value);
  int f2 = fexp(other);
  //if either is zero absolute compare the other to 2^-bits;
  if(value == 0.0) { //fexp on 0 isn't sanitizable.
    return f2 + bits < 0;
  }
  if(other == 0.0) {
    return f1 + bits < 0;
  }
  int cf = fexp(diff);
  cf += bits;
  return cf <= f1 && cf <= f2;
}

//tables were tricky to access, let's see who needs them.
//extern const u32 Decimal1[];
//extern const u64 Decimal2[];

/** @returns The base 10 exponent of @param value. Note that the number of digits for values >0 is 1+ilog10().
 * For zero this returns -1, most logic will have problems if you don't check that. */
int ilog10(u32 value);
int ilog10(u64 value);

/** an integer power of 10. out of bounds arg gets you nothing but trouble ... */
u32 i32pow10(unsigned power);

/** an integer power of 10. out of bounds arg gets you nothing but trouble ... */
u64 i64pow10(unsigned power);

/** @param p19 is 1-^19 times a fractional value. @param digits is the number of digits past the virtual radix point you are interested in.
@returns a properly rounded int that has those digits of interest, but you may need to pad with leading zeroes. */
u64 keepDecimals(u64 p19,unsigned digits);

/** @param p19 is 1-^19 times a fractional value. @param digits is the number of digits past the virtual radix point you are interested in.
@returns a truncated int that has those digits of interest, but you may need to pad with leading zeroes. */
u64 truncateDecimals(u64 p19,unsigned digits);
/** filtering in case we choose to optimize this */
double pow10(int exponent);

template <typename mathy> double squared(mathy x) {
  return x * x;
}

double degree2radian(double theta);

/** n!/r!(n-r)! combinatorial function.
 * Was formerlry named and documented as Pnr, but implementation was correct for Cnr and so was its usages.
*/
u32 Cnr(unsigned n, unsigned  r);

inline u32 min(u32 a, u32 b) {
  if(a < b) {
    return a;
  } else {
    return b;
  }
}

/** if a is greater than b set it to b and @return whether a change was made.
if orequal is true then also return true if args are equal.
if a is Nan then do the assign and return true */
template< typename S1, typename S2 > bool depress(S1 &a, S2 b,bool orequal=false) {
  if(isNan(b)){
    return false;
  }
  S1 b1 = S1(b); //so incomparable types gives us just one error.
  if(isNan(a) || a > b1) {
    a=b1;
    return true;
  }
  return orequal && a==b1;
}

/** if a is less than b set it to b and @return whether a change was made.
if orequal is true then also return true if args are equal.
if a is Nan then do the assign and return true */
template< typename S1, typename S2 > bool elevate(S1 &a, S2 b,bool orequal=false) {
  if(isNan(b)){
    return false;
  }
  S1 b1 = S1(b); //so incomparable types gives us just one error.
  if(isNan(a) || a < b1) {
    a=b1;
    return true;
  }
  return orequal && a==b1;
}

//using 'lesser' and 'greater' while we check if all of our compilers now have compatible min and max std functions.

//todo:2 see if compiler can use this for min of convertible types:
template< typename S1, typename S2 > S1 lesser(S1 a, S2 b) {
  S1 b1 = S1(b); //so incomparable types gives us just one error.
  if(a < b1) {
    return a;
  } else {
    return b1;
  }
}

template< typename S1, typename S2 > S1 greater(S1 a, S2 b) {
  S1 bb = S1(b);
  if(a > bb) {
    return a;
  } else {
    return bb;
  }
}

template< typename Scalar > void swap(Scalar &a, Scalar &b) {
  Scalar noxor;//don't trust that xor is non corrupting for all scalars.
  noxor = a;
  a = b;
  b = noxor;
}


/** Things that are coded in assembler on some platforms, due to efficiency concerns. In 2009 one version of the GCC compiler for ARM often produced horrible and sometimes incorrect code. Time permitting these should be compiled from the C equivalents and compared to the hand coded assembler to see if we can abandon the assembler due to compiler improvements. */
extern "C" {
/* @returns integer part of d, modify d to be its fractional part.
*/
  int splitter(double &d);

  /** the time delay given by ticks is ambiguous, it depends upon processor clock. @72MHz 1000 ticks is roughly one microsecond.*/
  void nanoSpin(unsigned ticks); //fast spinner, first used in soft I2C.

  //rounded and overflow managed 'multiply by ratio'
  u32 muldivide(u32 arg, u32 num, u32 denom);

  /** @param fractionalThereof changed from double to float due to compiler error, passed arg in wrong registers! (probably early gcc 4.1, need to retest) */
  u16 saturated(unsigned quantity, float fractionThereof);

  //fraction is a fractional multiplier, with numbits stating how many fractional bits it has.
  u16 fractionallyScale(u16 number, u16 fraction, u16 numbits);
  /** 1 + the integer part of log base 2 of the given number, pretty much is just "count the leading zeroes".
  * Note well that this will give 0 as the log of 0 rather than negative infinity, precheck the argument if you can't live with that.
  * mathematical definition: "number of right shifts necessary for an unsigned number to become 0"
  */
  u32 log2Exponent(u32 number);

  /** return eff * 2^pow2  where pow2 is signed. This can be done rapidly via bitfiddling*/
  float shiftScale(float eff, int pow2);

  double flog(u32 number);
  /** @return the natural logarithm of the ratio of @param over over @param under.
   * This is computable as the difference of their logs, but we wrap that here so that some fancy fidding can reduce the number of logarithms executed.  */
  double logRatio(u32 over, u32 under);

  u16 uround(float scaled);
  s16 sround(float scaled);

  /**NB: copyObject() and fillObject() can NOT be used with objects that contain polymorphic objects*/
  void copyObject(const void *source, void *target, u32 length);
  void fillObject(void *target, u32 length, u8 fill);

  //EraseThing only works on non-polymorphic types. On polymorphs it also  kills the vtable!
#define EraseThing(thing) fillObject(&(thing), sizeof(thing), 0);
  //see warning for EraseThing.
#define CopyThing(thing1,thing2) copyObject(thing1,thing2,min(sizeof(thing1),sizeof(thing2)))
  //documenting accessible portions of microcontroller startup code:
  void memory_copy(const void *source, void *target, void *sourceEnd);

  void memory_set(void *target, void *targetEnd, u8 value);

#if 0 //  fixmelater //!defined( QT_CORE_LIB ) && !defined() //std lib's differ between pc and arm.
  //the difference of two u16's should be a signed int. test your compiler.
  inline u16 abs(int value) {
    return value > 0 ? value : -value;
  }
#endif

} //end extern C for assembly coded routines.

/** variants of splitter, allowing for greater range. @see splitter is optimized for numbers less than 32k
@param d is replaced with its fractional part, the function @returns the integer part, @see standard math lib's modf for edge cases.
*/
//template <typename Integrish, typename Floater> Integrish intbin(Floater &d);
//template <> int intbin<int,double>(double &d);
//template <> long intbin<long,double>(double &d);
//template <> u64 intbin<u64,double>(double &d);

template <typename Integrish,typename Floater> Integrish intbin(Floater &d){
  double eye;
  d= modf(d,&eye);
  return Integrish(eye);
}
#endif /* ifndef minimath_h */
