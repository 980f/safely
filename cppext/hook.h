#ifndef HOOK_H
#define HOOK_H
//(C)opyright Andrew L. Heilveil (980F) 2017,2020

#include "cheaptricks.h"  //take()

/** a null checked function pointer, with a value to return if the pointer is null.
 * a variation with the argument supplied via a template arg caused 'template explosion' */
template<typename RetType, typename ... Args> class Hooker {
public:
  using Pointer=RetType(*)(Args ...);
protected:
  Pointer pointer;
  RetType defaultReturn;
 public:
  Hooker(RetType nullAction,Pointer fn=nullptr):pointer(fn),defaultReturn(nullAction){}
  /** set the function pointer.
   * Note that the default value remains that which was set by the constructor. This makes sense as the default is what the owner of the hook chooses, not the individual hoping to use the hook.
   * @returns the old pointer, for those usages which are 'borrowing' the hook, or nice enough to share with previous one (perhaps via a compose of some sort). */
  Pointer operator =(Pointer fn){
    Pointer was=pointer;
    pointer=fn;
    return was;
  }

  RetType operator () (Args ... args)const{
    if(pointer){
      return (*pointer)(args ...);
    } else {
      return defaultReturn;
    }
  }

};

///** a null checked function pointer, with a value to return if the pointer is null */
//template<typename RetType, RetType defaultReturn, typename ... Args> class Hooker {
//public:
//  using Pointer=RetType(*)(Args ...);
//protected:
//  Pointer pointer;
// public:
//  Hooker(Pointer fn=nullptr):pointer(fn){}
//  /** set the function pointer.
//   * Note that the default value remains that which was set by the constructor. This makes sense as the default is what the owner of the hook chooses, not the individual hoping to use the hook.
//   * @returns the old pointer, for those usages which are 'borrowing' the hook, or nice enough to share with previous one (for which you shohuld consider using sigc library). */
//  Pointer operator =(Pointer fn){
//    Pointer was=pointer;
//    pointer=fn;
//    return was;
//  }
//
//  //invoke. @returns what wrapped function returns, or the defaultReturn value if the function is not set/defined.
//  RetType operator () (Args ... args)const{
//    if(pointer){
//      return (*pointer)(args ...);
//    } else {
//      return defaultReturn;
//    }
//  }
//
//};


/** until I figure out how to code a void type for a return in Hooker here is a simpler version of that */
template<typename ... Args> class Hook {
public:
  using Pointer=void(*)(Args ...);
protected:
  Pointer pointer;
 public:
  Hook(Pointer fn=nullptr):pointer(fn){}

  Pointer operator =(Pointer fn){
    Pointer was=pointer;
    pointer=fn;
    return was;
  }

  void operator () (Args ... args){
    if(pointer){
      (*pointer)(args ...);
    }
  }

};

/** like hook but only runs once. Adds boolean to precheck if it will do something, to avoid building an expensive argument list if it won't be needed. */
template<typename ... Args> class HookOnce {
  using Pointer=void(*)(Args ...);
  Pointer pointer;
public:
  HookOnce(Pointer fn=nullptr):pointer(fn){}

  operator bool()const noexcept {
    return pointer!=nullptr;
  }

  /** @returns previous value so that a hook can be 'borrowed' */
  Pointer operator =(Pointer fn){
    Pointer was=pointer;
    pointer=fn;
    return was;
  }

  void operator () (Args ... args){
    if(pointer){
      Pointer preclear=take(pointer);
      //By clearing the pointer before invoking it the invoked function can set a pointer to handle the next event.
      (*preclear)(args ...);
    }
  }

};

//todo: equivalent of sigc hideReturn() and whatever they called supplyReturn()

#endif // HOOK_H
