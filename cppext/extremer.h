#ifndef EXTREMER_H
#define EXTREMER_H

/** classes for finding minima and maxima in a push environment (as data arrives in event driven system), versus the typical pull environment (a for loop iterating over
 * data that is all present)
 *  //todo: encapsulate the members.
 *  //todo: move 'not found' values into constructor args
 */

template<typename Scalar> class Extremer {
public:
/** whether we are seeking smallest else largest */
  bool negatory;
/** whether on new value equal to present extreme we prefer to mark the new value as the extremum */
  bool preferLatter;
/** whether any data has been inspected (else location is not valid) */
  bool started;
  //don't assign to these if you want this class's logic to prevail:
  int location;
  Scalar extremum;
  //usually called from a specialization
  Extremer(bool negatory = false,bool preferLatter = false) :
    negatory(negatory),preferLatter(preferLatter),
    started(false),location(-1){
    extremum = 0;//extremum set just for debug
  }

  /** @return whether the extremum or location was updated */
  bool inspect(int loc,const Scalar&value ){
    if(started) {
      if(extremum==value) {
        if(preferLatter ? loc<location : loc> location) {//then check index, same index is moot.
          return false;
        }
      } else if(negatory ? extremum<value : extremum> value) {
        return false;
      }
    }
    started = true;
    extremum = value;
    location = loc;
    return true;
  } // inspect

}; // class Extremer

class MaxDouble : public Extremer<double>{
public:
  MaxDouble(bool preferLatter = false) : Extremer<double>(false,preferLatter){
//might do this for debug:    extremum=-Inf;
  }

};

class MinDouble : public Extremer<double>{
public:
  MinDouble(bool preferLatter = false) : Extremer<double>(true,preferLatter){
//might do this for debug:    extremum=-Inf;
  }

};

//add more wrapped specializations here.

#endif // EXTREMER_H
