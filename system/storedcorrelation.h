#ifndef STOREDCORRELATION_H
#define STOREDCORRELATION_H
#include "storable.h"
#include "storednumeric.h"
#include "realstatistic.h"
#include "linearfit.h"

/** Storage wrapped simple statistic */
class StoredStatistic : public Stored {
  RealStatistic &wrapped;
  StoredReal sum;
  StoredReal sumSquares;
  StoredInt count;
public:
  StoredStatistic(Storable &node,RealStatistic &wrapped);
  void onPrint();
};


/** Storage wrapped linear correlator */
class StoredCorrelation : public LinearFit, public Stored {
  StoredStatistic xx;
  StoredStatistic yy;
  StoredReal sumCross;

public:
  StoredCorrelation(Storable &node);
  void onPrint();

};

class WatchedCorrelation : public StoredCorrelation {
  SimpleSignal watchers;
public:
  WatchedCorrelation(Storable &node);
  sigc::connection whenUpdated(SimpleSlot slott,bool kickme = false);
  void updateComplete();
};

#endif // STOREDCORRELATION_H
