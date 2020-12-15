// (C) 2020 Andrew Heilveil (980f) created on 11/10/20.
#pragma once

#include <systick.h>
#include "circularindexer.h"

/**
 * A traceback buffer, useful for tracking state changes when breakpoints screw up the process.
 *
 * @param TraceItem class must be copy assignable.
 * if @param Ticked is true then it must have a 'tick' member of some integral type and this class will timestamp the object using SystemTimer (cortexm SysTick).
 * */
template<class TraceItem, unsigned depth = 100, bool Ticked = false> class Tracer {
  TraceItem memory[depth];
public:
  CircularIndexer<TraceItem> looper{memory, sizeof(memory)};

  /** the argument type passed to this is usually a member of the TraceItem class, so usage involves wrapping in {} so that a constructor can fill out the object and timestamp it for classes that support Ticked.*/
  void operator()(TraceItem &&braced) {
    if constexpr (Ticked) {
      braced.tick =  sizeof(braced.tick>4)? SystemTimer::snapLongTime(): SystemTimer::tocks();
    }
    looper.next() = braced;
  }

  /** if you want ticks you have to provide them explicitly if you use this method */
  void operator()(const TraceItem &braced) {
    looper.next() = braced;
  }
};
