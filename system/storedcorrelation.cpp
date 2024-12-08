#include "storedcorrelation.h"

StoredStatistic::StoredStatistic(Storable &node, RealStatistic &wrapped) : Stored(node),
  wrapped(wrapped),
  ConnectChild(sum),
  ConnectChild(sumSquares),
  ConnectChild(count){
  sum.onChangeUpdate(wrapped.sum,true);
  sumSquares.onChangeUpdate(wrapped.sumSquares,true);
  count.onChangeUpdate(wrapped.count,true);
}

void StoredStatistic::onPrint(){
  sum = wrapped.sum;
  sumSquares = wrapped.sumSquares;
  count = wrapped.count;
}

/////////////////////////////

StoredCorrelation::StoredCorrelation(Storable &node) : Stored(node),
  ConnectChild(xx,correlation.xx),
  ConnectChild(yy,correlation.yy),
  ConnectChild(sumCross){
  sumCross.onChangeUpdate(correlation.sumCross,true);
}

void StoredCorrelation::onPrint(){
  xx.onPrint();
  yy.onPrint();
  sumCross = correlation.sumCross;
}

WatchedCorrelation::WatchedCorrelation(Storable &node) : StoredCorrelation(node){

}

sigc::connection WatchedCorrelation::whenUpdated(SimpleSlot slott, bool kickme){
  if(kickme) {
    slott();
  }
  return watchers.connect(slott);
}

void WatchedCorrelation::updateComplete(){
  onPrint();
  watchers();
}
