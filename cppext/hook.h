#pragma once
// "(C) Andrew L. Heilveil, 2017"

#include <functional>
/** hook with a return value.
*/
template<typename RetType, typename... Args> class Hooker {
public: //expose function's type for use in arguments to be passed to this guy
  using Pointer = std::function<RetType(Args...)>;

private:
  Pointer pointer;
  RetType defaultReturn;

public:
  /** can set function to call, must set what to return if no function is set. */
  Hooker(RetType nullAction, Pointer fn = nullptr): pointer(fn), defaultReturn(nullAction) {}
  /** set the function pointer.
   * Note that the default value remains unchanged. This makes sense as the default is what the owner of the hook chooses, not the individual hoping to use the hook.
   * @returns the old pointer, for those usages which are 'borrowing' the hook, or nice enough to share with previous one (for which you should consider using sigc library). */
  Pointer operator =(Pointer fn) {
    Pointer was = pointer;
    pointer = fn;
    return was;
  }

  /** invoke the hook with some args. */
  RetType operator ()(Args... args) const {
    if constexpr (std::is_same<RetType, void>::value) {
      if (pointer) {
        pointer(args...);
      }
    } else {
      if (pointer) {
        return pointer(args...);
      } else {
        return defaultReturn;
      }
    }
  }

  /** @returns whether there is any point in calling this hook. (so that you can distinguish between return of default and return that happens to match default)*/
  operator bool() const {
    return bool(pointer);
  }
};


/** reserve namespace for a Hooker that defaults to passing its arg through rather than feeding a constant */
template<typename InAndOout> using Filter = Hooker<InAndOout, InAndOout>;

template<typename InAndOout> InAndOout trivialFilter(InAndOout in) {
  return in;
}


/** until I figure out how to code a void type for a return in Hooker here is a simpler version of that. Might be able to solve that with a constexper if in invopker. */
template<typename... Args> class Hook {
public: //expose function's type for use in arguments to be passed to this guy
  using Pointer = std::function<void(Args...)>;

private:
  Pointer pointer;

public:
  Hook(Pointer fn = nullptr): pointer(fn) {}

  Pointer operator =(Pointer fn) {
    Pointer was = pointer;
    pointer = fn;
    return was;
  }

  void operator ()(Args... args) const {
    if (pointer) {
      pointer(args...);
    }
  }

  /** @returns whether there is any point in calling this hook. (can distinguish between return of default and return that happens to match default)*/
  operator bool() const {
    return bool(pointer);
  }
};
