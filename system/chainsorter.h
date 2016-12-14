#ifndef CHAINSORTER_H
#define CHAINSORTER_H
#include "chain.h"
#include "sigcuser.h"


/**
a class that implements sorted insertion with related actions on some existing chain.
NB: compiler is broken in that a friend declaration didn't allow for direct access to the v[] member of the ChainWrapper::list, and we also shouldn't have to use the laborious syntax to access that list.
*/
template< typename T > class ChainSorter: public ChainWrapper< T >{
  /** it only makes sense to have a single compare function applied to a sorted list */
  sigc::slot<int,const T*,const T*>comparator;
public:
  ChainSorter(Chain<T> &list,sigc::slot<int,const T*,const T*>comparator):
    ChainWrapper<T>(list),
    comparator(comparator){
    //there is no basis for a default comparator.
  }

  /** @return where to insert @param thing based on comparator. A value less than 0 indicates where a matching element has been found.
  this method relies upon the list being sorted, see lastIndex if you are probing a not-yet-sorted list */
  int sortIndex(const T*thing) const {
    int i=ChainWrapper<T>::list.quantity();
    if(i<9){//arbitrary cutoff for linear being faster than binary search
      while(i-->0){
        int cmp=comparator(thing,ChainWrapper<T>::list[i]);
        if(cmp<0){ //existing entity is 'less than'
          return 1+i;
        } else if(cmp==0) {
          return ~i;
        } //else keep on looking
      }
      return 0;//if before all items then insert at 0.
    } else { //do binary search
      int imax=i-1;
      int imin=0;
      while (imax > imin){
        int imid = (imin+imax)/2;//rounding down
        int cmp=comparator(thing,ChainWrapper<T>::list[imid]);
        if (cmp<0){
          // change min index to search upper subarray
          imin = imid+1;
        } else {// change max index to search lower subarray
          imax = imid;
        }
      }
      if(imax==imin){
        int cmp=comparator(thing,ChainWrapper<T>::list[imin]);
        if(cmp==0){
          return ~imin; //found a match on final step
        } else if(cmp<0){ //final entity is 'less than'
          return imin+1;
        } else {
          return imin; //which is 0 if new thing is below the whole list.
        }
      } else {
        return imin+1;
      }
    }
  }

  /** @returns @param thing after inserting it into list. This doesn't check if the thing itself is already in the list. Items of equal value to existing items follow the existing item. does NOT insert null pointers into the list. */
  T *insertSorted(T *thing){
    if(thing==nullptr){
      return nullptr;
    }
    int location=sortIndex(thing);
    if(location>=0){
      return ChainWrapper<T>::list.insert(thing,location);
    } else {//we have a duplicate
      return ChainWrapper<T>::list.insert(thing,1+~location);//newer follows older
    }
  }

  /** @returns @param thing after inserting it into list IFFI no equal valued item is in the list. @returns nullptr if given one or an equal valued item is in the list.
*/
  T *insertUnique(T *thing){
    if(thing==nullptr){
      return nullptr;
    }
    int location=sortIndex(thing);
    if(location>=0){//not in list, returns proper location for it
      return ChainWrapper<T>::list.insert(thing,location);
    } else {//we have a duplicate, unless there is nothing yet present
      if(ChainWrapper<T>::list.v.size()==0){
        return ChainWrapper<T>::list.insert(thing,0);
      } else {
        return nullptr;
      }
    }
  }

  /** @returns index of an element upon which the ordinator returns 0.
   * if there is no such element then @returns ~ of where it should be if the ordinator matches this class's comparator.
   * if the ordinator is not compatible with the comparator then the return value when <0 has no meaning other than 'none found'.
*/
  int indexFor(sigc::slot<int,const T*>ordinator){
    int top=ChainWrapper<T>::list.quantity();
    if(top--==0){
      return ~0;
    }
    int bot=0;
    while(top > bot){
      int middle = (top+bot)/2;//rounding down
      int cmp=ordinator(ChainWrapper<T>::list[middle]);
      if (cmp<0){//target is above middle
        bot = middle+1;
      } else {//target is below OR AT middle
        top = middle;
      }
    }

    int cmp=ordinator(ChainWrapper<T>::list[bot]);//may rerun last compare
    if(cmp==0){
      return bot; //found a match on final step
    } else if(cmp<0){ //final entity is 'less than'
      return ~(bot+1);
    } else {
      return ~bot;
    }
  }

  /** physically reorganize the wrapped chain to be sorted by the builtin rule.
    @returns the number of relocations needed to do that, 0 if chain was already sorted.*/
  int resort(){
    //bubble sort, presume chain is mostly sorted with just a few manually screwed up entries.
    int swaps(0); //diagnostic counter
    int top(ChainWrapper<T>::list.quantity());//FUE
    int i=top-1;//top down
    while(i-->0){//n-1 steps for n items, a single item is inherently sorted.
      T* thing=ChainWrapper<T>::list[i];
      int cmp=comparator(thing,ChainWrapper<T>::list[i+1]);//list[above i] are sorted by now
      if(cmp>0){ //must move it up
        ++swaps;
        //use linear search for now, factor out binary search later.
        for(int j=i;++j<top; ){//scan upwards as we are optimizing for only slightly disordered.
          cmp=comparator(thing,ChainWrapper<T>::list[j]);
          if(cmp<=0){//we've found our spot
            ChainWrapper<T>::list.relocate(i,j);
          }
        }
      }
    }
    return swaps;
  }
};

#endif // CHAINSORTER_H
