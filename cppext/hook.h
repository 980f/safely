#pragma once
// "(C) Andrew L. Heilveil, 2017"

#include <functional>

/** joint parts of @see Hook and @see Hooker, see those for what this is all about.*/
template<typename RetType, typename... Args> class Hookbase {
protected: //this class exists to be extended.
  using Pointer = std::function<RetType(Args...)>;

  Hookbase(Pointer fn = nullptr): pointer(fn) {}

  Pointer pointer;

  /** set the function pointer.
   * Note that the default value remains unchanged. This makes sense as the default is what the owner of the hook chooses, not the individual hoping to use the hook.
   * @returns the old pointer, for those usages which are 'borrowing' the hook, or nice enough to share with previous one (for which you should consider using sigc library). */
  Pointer assign(Pointer fn) {
    Pointer was = pointer;
    pointer = fn;
    return was;
  }

  /** @returns whether there is any point in calling this hook. (can distinguish between return of default and return that happens to match default)*/
  operator bool() const {
    return pointer != nullptr;
  }
};

/** just does something, or not. */
template<typename... Args> class Hook : public Hookbase<void, Args...> {
public:
  using Base = Hookbase<void, Args...>;
  using Pointer = typename Base::Pointer;
  Hook(Pointer fn = nullptr): Base(fn) {}

  void operator ()(Args... args) const {
    if (*this) {
      Base::pointer(args...);
    }
  }
};

/** hook with a return value. If nobody has taken the hook then a construction time set value is returned when the hook is invoked.
*/
template<typename RetType, typename... Args> class Hooker : Hookbase<RetType, Args...> {
public: //expose function's type for use in arguments to be passed to this guy
  using Base = Hookbase<RetType, Args...>;
  using Pointer = typename Base::Pointer;

  /** may set a function to call, must set what to return if no function is set. */
  Hooker(RetType nullAction, Pointer fn = nullptr): Base(fn), defaultReturn(nullAction) {}

  /** @returns what function that was set by construction or operator = returns , if none then a default value set only by constructor */
  RetType operator ()(Args... args) const {
    if (*this) {
      return Base::pointer(args...);
    } else {
      return defaultReturn;
    }
  }

  Pointer operator=(Pointer fn) {
    return Base::assign(fn);
  }

private:
  RetType defaultReturn;
};


/** reserve namespace for a Hooker that defaults to passing its arg through rather than feeding a constant */
template<typename InAndOut> using Filter = Hooker<InAndOut, InAndOut>;

template<typename InAndOut> InAndOut trivialFilter(InAndOut in) {
  return in;
}
