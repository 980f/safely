#ifndef WORSTCASE_H
#define WORSTCASE_H

#include "stopwatch.h"
#include "maxtracked.h"
#include "textpointer.h"

/** static allocate and use mark and show around something needing worst case time tracking */
struct WorstCase {
  MaxTracked<NanoSeconds> latency;
  StopWatch watcher;
  Text format;

  void mark();

  void show();

  void lap(const WorstCase &other);

  /** defaults to process time. make a separate one for realtime if you want both */
  WorstCase(TextKey fmt,bool realElseProcess=false);

  /** either mark() or lap() then return 'this' in the @param scanner.
   * Usage is to have WC first; WC second; WC*chain=nullptr; first.Chain(&chain); *chain.show(), second.Chain(&chain); ...
 */
  void Chain(WorstCase **scanner);

  /** usage:   {AutoShow ignored(timer.time()); \n code to time; }*/
  struct AutoShow {
    WorstCase &timer;
    AutoShow (WorstCase &timer):timer(timer){
      timer.mark();
    }
    ~AutoShow(){
      timer.show();
    }
  };

  AutoShow time();
};
#endif // WORSTCASE_H
