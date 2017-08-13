#ifndef COUNTDOWN_H
#define COUNTDOWN_H "(C) Andrew L. Heilveil, 2017"

/** if non-zero then decrement else leave alone */
class CountDown {
  unsigned counter;
public:
  CountDown(unsigned count=0):counter(count){}

  unsigned operator =(unsigned quantity){
    return counter=quantity;
  }

  /** getter */
  operator unsigned() const {
    return counter;
  }

  /** decrements counter unless it is already zero, @returns whether it is now zero */
  bool operator--(int) noexcept;

  bool done() const noexcept{
    return counter==0;
  }

/** sometimes you go back one */
  unsigned operator ++() noexcept{
    return ++counter;
  }

  /** test, decrements if not already zero and @returns whether it just became zero */
  bool last() noexcept;

  bool hasNext() const noexcept {
    return counter>0;
  }

  /** just how weird can c++ be?
   * @returns a reference to the element of the @param array associated with this count value.
   * Note then when counting is done this will get you the 0th item repeatedly, not an exception. */
  template <typename Many> Many& next(Many array[]) const {
    return array[counter];
  }
};
#endif // COUNTDOWN_H
