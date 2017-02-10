#ifndef HOOK_H
#define HOOK_H

/** hook with a return value */
template<typename RetType, typename ... Args> class Hooker {
  typedef RetType (*Pointer)(Args ...);
  Pointer pointer;
  RetType defaultReturn;
 public:
  Hooker(RetType nullAction,Pointer fn=nullptr):pointer(fn),defaultReturn(nullAction){}
  /** set the function pointer.
   * Note that the default value remains that which was set by the constructor. This makes sense as the default is what the owner of the hook chooses, not the individual hoping to use the hook.
   * @returns the old pointer, for those usages which are 'borrowing' the hook, or nice enough to share with previous one (for which you shohuld consider using sigc library). */
  Pointer operator =(Pointer fn){
    Pointer was=pointer;
    pointer=fn;
    return was;
  }

  RetType operator () (Args ... args){
    if(pointer){
      return (*pointer)(args ...);
    } else {
      return defaultReturn;
    }
  }

};

/** until I figure out how to code a void type for a return in Hooker here is a simpler version of that */
template<typename ... Args> class Hook {
  typedef void (*Pointer)(Args ...);
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


//todo: equivalent of sigc hideReturn() and whatever they called supplyReturn()

#endif // HOOK_H
