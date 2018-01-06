#ifndef FITSTAT_H
#define FITSTAT_H   "(C) 2017 Andy Heilveil"
/**
  * report on a fit (regression) operation
  */
struct FitStat {
  int num; //number of independent samples, negative if insufficient data for fit.
  int df;
  double chiSquare;
  FitStat();
  void init(int df = 0);
  void apply(double deviation);
  FitStat &operator=(const FitStat &&other);
};
#endif // FITSTAT_H
