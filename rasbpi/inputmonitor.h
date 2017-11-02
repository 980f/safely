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

  /** @returns whether most recent reading was active */
  operator bool() const noexcept {
    return lastSample;
  }

  bool isHigh(double debounced)const noexcept;

  bool isLow(double debounced)const noexcept;

};

/** filtering edge detector */
struct InputEvent {
  InputMonitor &inp;
  bool lastStable;
  double debounce[2];
  InputEvent (InputMonitor &inp,double lowfilter=0.0,double highfilter=0.0);
  void init(double timestamp);
  /** take fresh sample and @returns true if lastStable gets updated.*/
  bool changed(double timestamp);

};


#endif // INPUTMONITOR_
