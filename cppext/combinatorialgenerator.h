/**
// Created by andyh on 1/31/25.
// Copyright (c) 2025 Andy Heilveil, (github/980f). All rights reserved.
*/

#pragma once

/* initialized with n, produces Cnr for r= 0 .. n inclusive, sequentially
 *
 * tested 31jan2005 via godbolt.org
 */
class CombinatorialGenerator {
  unsigned n;
  unsigned r;
  unsigned Cnr;

public:
  unsigned N() const {
    return n;
  }

  unsigned R() const {
    return r;
  }

  /** setting N clears R, Ie restarts generation, you can't get a random Cnr from this class.*/
  void setN(unsigned n) {
    this->n = n;
    this->r = 0;
    Cnr = 1;
  }

  CombinatorialGenerator(unsigned enn) {
    setN(enn);
  }

  unsigned operator()() {
    //if r==0 increment and return
    unsigned precalced = Cnr;
    Cnr *= n-r;//n,n-1,n-2 : n-r
    Cnr /= ++r;//1,1,2,3,4 : r but not if 0 unless n/0 = n on your processor!
    return precalced;
  }

  operator bool() const {
    return r<=n;
  }
};
