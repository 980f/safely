#include "storedcorrelation.h"

StoredStatistic::StoredStatistic(Storable &node, RealStatistic &wrapped):Stored(node),
  wrapped(wrapped),
  ConnectChild(sum),
  ConnectChild(sumSquares),
  ConnectChild(count){
  sum.onChangeUpdate(wrapped.sum,true);
  sumSquares.onChangeUpdate(wrapped.sumSquares,true);
  count.onChangeUpdate(wrapped.count,true);
}

void StoredStatistic::onPrint(){
  sum=wrapped.sum;
  sumSquares=wrapped.sumSquares;
  count=wrapped.count;
}

/////////////////////////////

StoredCorrelation::StoredCorrelation(Storable &node):Stored(node),
  ConnectChild(xx,RealCorrelation::xx),
  ConnectChild(yy,RealCorrelation::yy),
  ConnectChild(sumCross){
  sumCross.onChangeUpdate(RealCorrelation::sumCross,true);
}

void StoredCorrelation::onPrint(){
  xx.onPrint();
  yy.onPrint();
  sumCross=RealCorrelation::sumCross;
}

WatchedCorrelation::WatchedCorrelation(Storable &node):StoredCorrelation (node)
{

}

sigc::connection WatchedCorrelation::whenUpdated(SimpleSlot slott, bool kickme){
  if(kickme){
    slott();
  }
  return watchers.connect(slott);
}

void WatchedCorrelation::updateComplete(){
  onPrint();
  watchers();
}

