#ifndef PEAKFINDRECORDS_H
#define PEAKFINDRECORDS_H
#include "storable.h" //todo:2 try to remove storable from here, or create a microcontroller compatible one.
#include "storednumeric.h"

struct PeakSearch:public Stored {
  StoredReal energy;//0== don't use, stale entry in table
  StoredReal width;//0== don't use in resolution calibration
  StoredInt  which;//spectrum
  PeakSearch (Storable &node);
  bool isValid()const;
  bool usableWidth()const;
};

struct PeakFind: public Stored {
  StoredReal center;
  StoredReal amplitude; //at center
  StoredReal low;    //channel of greatest positive slope
  StoredReal riser;  //most positive slope
  StoredReal high;   //channel of greatest negative slope
  StoredReal faller; //most negative slope
  StoredInt si;  //spectrum backlink, for convenience.
  StoredBoolean efit;//used in fit
  StoredBoolean rfit;
  PeakFind (Storable &node);
  void clear();
  double width()const;
  bool hasWidth()const;
};
#endif // PEAKFINDRECORDS_H
