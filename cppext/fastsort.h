/**
// Created by andyh on 1/21/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once
#include <functional>

template<typename Object> class FastSort {
  using Comparator = std::function<bool(const Object *, const Object *)>;
  Comparator isGreaterThan;
  //swap must be inherent in type, see std::swap.
public:
  explicit FastSort(Comparator &compare) : isGreaterThan{compare} {}

  void operator()(Object storage[], unsigned length, const Object &pivot, unsigned level = 0) {
    if (length <= 1) {
      return;
    }
    if (length == 2) { //optional, expedites leaves
      if (isGreaterThan(storage[0], storage[1])) {
        std::swap(storage[0], storage[1]);
      }
      return;
    }
    //an optimzation for 3 is barely worth it and takes as many lines of code as follows, so not bothering with it for now.
    Object *lower = storage;
    Object *upper = storage + length; //one past end
    while (lower < upper) {
      if (isGreaterThan(lower, pivot)) {
        std::swap(*lower, *--upper);
      } else {
        ++lower;
      }
    }
    auto lowlength = std::distance(storage, lower); //or upper, they are equal ;)
    if (lowlength > 1) { //measurable performance tweak since call overhead is nontrivial
      *this(storage, lowlength, storage[lowlength / 2], level + 1);
    }

    auto highlength = length - lowlength;
    if (highlength > 1) {
      *this(upper, highlength, storage[lowlength + highlength / 2], level + 1);
    }
  }
};
