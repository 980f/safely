#ifndef EXTREMER_H
#define EXTREMER_H

/** classes for finding minima and maxima in a push environment (as data arrives in event driven system), versus the typical pull environment (a for loop iterating over
 * data that is all present)
 *  //todo: encapsulate the members.
 *  //todo: move 'not found' values into constructor args
 *
 */

/** negatory is whether we are seeking smallest else largest
 * preferLatter is how to deal with ties between new value and reigning champion, whether to have the latest one take the crown.
 */
template<typename Scalar,bool negatory=false,bool preferLatter=false> class Extremer {
public: //for convenience.
/** whether any data has been inspected (else location and extremum are not valid)
 * This is easier than trying to come up with a initial value of extremum.
*/
  bool started=false;
public: //could make these read-only
  int location=~0;//default for debug
  /** */
  Scalar extremum;
public:
  /** @return whether the extremum or location was updated */
  bool inspect(int loc,const Scalar&value){
    if(started) {
      if(negatory){
        if(extremum<value){
          return false;
        }
      } else {
        if(extremum> value) {
          return false;
        }
      }

      if(extremum==value) {
        if(preferLatter) {
          if(loc<location){
            return false;
          }
        } else {
          if(loc> location) {//then check index, same index is moot.
            return false;
          }
        }
      }
    } else {
      started = true;
      extremum = value;
      location = loc;
      return true;
    }
  } // inspect

}; // class Extremer


//prebuild the common ones:
class MaxDouble : public Extremer<double,false,false>{};
class MinDouble : public Extremer<double,true,false>{};

//add more wrapped specializations here.


#endif // EXTREMER_H
