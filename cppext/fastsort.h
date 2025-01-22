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

  void operator()(Object storage[], unsigned length, const Object &pivot) {
    if (length <= 1) {
      return;
    }
    if (length == 2) { //optional, expeditious especially if function call overhead is notable.
      if (isGreaterThan(storage[0], storage[1])) {
        std::swap(storage[0], storage[1]);
      }
      return;
    }
    //an optimization for 3 is barely worth it and takes as many lines of code as follows, so not bothering with it for now.
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
      *this(storage, lowlength, storage[lowlength / 2]);
    }

    auto highlength = length - lowlength;
    if (highlength > 1) {
      *this(upper, highlength, storage[lowlength + highlength / 2]);
    }
  }
};

/*
quicksort

For list length 1 or 0 do nothing,
For list length 2 compare and maybe swap.

with pointers to start and end of list and a value likely to be near the median (or picked at random such as from the center of the list):
if next is after pivot exchange with present last, decrementing last pointer.
if next was before or equal to pivot increment start pointer.
loop until start pointer equals last pointer.
We now have two lists and all the members of one are less than all the members of the other.

for each sublist split where the pointers hit each other:
recurse on list with pivot picked from the list, perhaps at its midpoint.

Refinement: if the items have a numeric range associated with the compare average that for each partition to use as the pivot for the recursions.
Refinement: track the nesting depth and size of list to detect pathological input.

This sort behaves abysmally if the input is nearly sorted, or nearly anti-sorted. If you have reason to suspect that you should bubble sort.
 */
