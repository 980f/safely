#pragma once //   "(C) 2017 Andy Heilveil"

/**
  * report on a fit (regression) operation
  */
struct FitStat {
  int num; //number of independent samples, negative if insufficient data for fit.
  /** degrees of freedom */	
  unsigned df;
  /** something like the unscaled amount of variance */
  double chiSquare;
  FitStat();
  void init(unsigned df = 0);
  void apply(double deviation);
  FitStat &operator=(const FitStat &&other);
};
