#pragma once
#include <cstdint>

/** marker class for timing services. All usages have at least 32 bits for a timer source, larger ones are not yet interesting or relevant as this is a practical class and nobody waits long enough for 32 bits to be inadequate.  */
using Ticks = uint32_t;
/** shared timer services. 
  * an isr will determine that the given time has expired, setting the done bit.
  * but the interested code will have to look at each object to determine that the event occurred OR
  * will have to derive a class and overload onDone().
  */


class PolledTimer {
protected:
  bool running; //changed from 'done' to 'running' so that we can init via joint ram clear rather than init code.
  Ticks systicksRemaining;
public:
  PolledTimer();
  virtual ~PolledTimer();//in case derived classes need significant destruction.
  virtual void restart(Ticks ticks);
  void restart(float seconds);//float (not double) as is often in time critical code.
  /** stops countdown without triggering onDone() */
  void freeze();
  //what the timer isr calls:
  void check();
  /** called when systicksRemaining goes to 0.
   * Overload to have something done <B>within</B> the timer interrupt service routine */
  virtual void onDone();
  /** @returns whether this is no longer counting */
  inline bool isDone() const{
    return !running;
  }
};


/** automatic restart. If you are slow at polling it it may become done again while you are handling a previous done.
 * periodic event, with fairly low jitter. The period is fairly stable but the action routine can be delayed by other actions.
 */
class CyclicTimer : public PolledTimer {
protected:
  Ticks period;
  uint32_t fired;
public:
  CyclicTimer(Ticks period,bool andStart=false):period(period){
    if(andStart){
      retrigger();
    }
  }

  bool hasFired();

  operator bool (){
    return hasFired();
  }

  void retrigger(){
    //leave fired as is.
    PolledTimer::restart(period);
  }

  void restart(Ticks ticks) override{
    PolledTimer::restart(period = ticks);
  }

  /** called when systicksRemaining goes to 0.
   * Overload to have something done within the timer interrupt service routine */
  void onDone() override;

};
