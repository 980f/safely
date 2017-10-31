#ifndef INPUTMONITOR_H
#define INPUTMONITOR_H

#include "din.h"

/** simple duration based debouncer.
 * user must periodically call sample(), which returns whether an edge just happened.
 * most uses should instead check isHigh or isLow
*/
class InputMonitor {
  bool lastSample;
  unsigned changes[2];
  //timestamp is seconds as double
  double lastChecked;
  double events[2];
   Din &input;
public:
  InputMonitor(Din &input);
  /** take first sample, arrange for isHigh or isLow to not trigger until sample() has been called. */
  void init(double timestamp);
  /** samples the input, records info about it.
   * @returns whether the input just changed due to this call to sample it. */
  bool sample(double timestamp);
  operator bool() const noexcept;
  bool isHigh(double debounced)const noexcept;

  bool isLow(double debounced)const noexcept;

};

//class InputEvent {
//  int level;
//  double debounce;
//  InputMonitor &inp;
//  bool check(double timestamp);

//};


#endif // INPUTMONITOR_
