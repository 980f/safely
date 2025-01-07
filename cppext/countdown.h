#pragma once // "(C) Andrew L. Heilveil, 2017"

/** an unsigned that will not wrap when you increment or decrement it.
 * This is most useful when counting down and stopping "when done" */
class CountDown {
  unsigned counter;

public:
  CountDown(unsigned count = 0): counter(count) {}

  unsigned operator =(unsigned quantity) {
    return counter = quantity;
  }

  /** getter */
  operator unsigned() const {
    return counter;
  }

  bool isDone() const noexcept {
    return counter == 0;
  }

  /** decrements counter unless it is already zero, @returns whether it just became zero */
  unsigned operator--(void) noexcept {
    if (counter) {
      --counter;
    }
    return counter;
  }

  unsigned operator--(int) noexcept {
    auto was=counter;
    --counter;
    return was;
  }

  /** test, decrements if not already zero and @returns whether it just became zero */
  bool last() noexcept {
    if (counter) {
      --counter;
      return counter == 0;
    } else {
      return false;
    }
  }

  /** attempt to increment counter, @returns whether it did increment, else it is all ones.**/
  bool backup() noexcept {
    if (++counter == 0) {
      counter = ~0;
      return false;
    }
    return true;
  }

  /** sometimes you go back one, but we disallow wrapping to zero */
  unsigned operator ++() noexcept {
    backup();
    return counter;
  }

  unsigned operator ++(int) noexcept {
    unsigned was = counter;
    backup();
    return was;
  }

  bool hasNext() const noexcept {
    return counter > 0;
  }

  /** just how weird can c++ be?
   * @returns a reference to the element of the @param array associated with this count value After decrementing it.
   * Note then when counting is done this will get you the 0th item repeatedly, not an exception. */
  template<typename Many> Many &nextFrom(Many array[]) {
    return array[(--*this)];
  }
};
