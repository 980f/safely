/**
// Created by andyh on 1/31/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once

/** generates fibonacci series, starting from arbitrary pair, but does not emit that pair */
template<typename Integrish> class FibonacciGenerator {
  Integrish even;
  Integrish odd;
  unsigned step = ~0;

public:
  FibonacciGenerator(Integrish first, Integrish second): even(first), odd{second} {}

  Integrish operator()() {
    ++step;
    // if constexpr (!skipGenerators) {
    //   if (step == 0) {
    //     return even;
    //   }
    //   if (step == 1) {
    //     return odd;
    //   }
    // }
    if (step & 1) {
      return odd += even;
    }
    //must be even
    return even += odd;
  }

  /** @returns ordinal of prior value, 0 after ist value is requested, ~0 if none have been requested */
  unsigned ordinal() const {
    return step;
  }

  operator bool() const {
    // if constexpr (!skipGenerators) {
    //   if (step == 0) {
    //     return true;
    //   }
    //   if (step == 1) {
    //     return true;
    //   }
    // }
    //
    return step & 1 ? odd < even : even < odd; //we wrapped
  }

/** @returns last value generated, 2nd of the two inits if never stepped */
  Integrish ultimate() const {
    return step & 1 ? odd  : even ; //step&1 is true if never stepped
  }

  /** @returns 2nd back, first of the two inits if never stepped */
  Integrish penultimate() const {
    return step & 1 ? even : odd; //step&1 is true if never stepped
  }
};
