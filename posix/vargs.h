#ifndef VARGS_H
#define VARGS_H

#include <stdarg.h>

/** for the hyper paranoid: a class to ensure va_end gets called.
 *  for the lazy this removes a tiny bit of typing.
 *  usage:
 *  //instead of:
 *   va_list args;
 *   va_start(args, fmt);
 *   vprintf(fmt,args);
 *   va_end(args);
 *  //
 *   Vargs<decltype(fmt)>args(fmt);
 *   vprintf(fmt,args);
 *
 */
template<typename Format> class Vargs {
public:
  va_list args;
  Vargs(Format &fmt){
    //va_start(args, fmt);
  }

  ~Vargs(){
    va_end(args);
  }

  operator va_list&(){
    return args;
  }
}; // class Vargs

/** the most used instance:
 *  so for printf variants:
 *  PFlist args(fmt);
 *  vprintf(fmt,args);
 */
typedef Vargs<const char *> PFlist;

#endif // VARGS_H
