#ifndef STACKED_H
#define STACKED_H "(C) Andrew L. Heilveil, 2017"

/** when an object has some state that needs to be stacked, such as in a recursive descent parser,
this class uses destruction to ensure a pop occurs when it should.

This is similar to 'assignonexit'  and 'copyonexit'

The type must be assignment copyable and copy constructable
*/

template <typename Scalar> class Stacked {
  Scalar &object;
  const Scalar oldvalue;
public:
  /** point of declaration pushes present value */
  Stacked(Scalar &object):
    object(object),  //store address
    oldvalue(object) //store value
  {
    //#nada, using copy construction for push
  }

  /** point of destruction pops that value */
  ~Stacked(){
    object=oldvalue;
  }
  //and for convenience let this stand in for the thing pushed, which might have an annoying to type name.
  operator Scalar &() const {
    return object;
  }
};

#endif // STACKED_H
