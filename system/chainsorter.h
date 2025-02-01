#pragma once

#include "chain.h"
#include <functional>

/**
 *  a class that implements sorted insertion with related actions on some existing chain.
 *  NB: compiler is broken in that a friend declaration didn't allow for direct access to the v[] member of the ChainWrapper::list, and we also shouldn't have to use
 * the laborious syntax to access that list.
 */
template<typename T> class ChainSorter {
  Chain<T> &list;
  /** it only makes sense to have a single compare function applied to a sorted list */
  using Sortation = int(*)(const T *, const T *);
  Sortation comparator;
  /** whether a newly inserted item should follow an existing equivalent item */
  bool laterAfter;

public:
  ChainSorter(Chain<T> &list, Sortation comparator, bool laterAfter = false) : list(list), comparator(comparator), laterAfter{laterAfter} {}

  /** @returns where to insert @param thing based on comparator. A value less than 0 indicates where a matching element has been found.
   *  this method relies upon the list being sorted, see lastIndex if you are probing a not-yet-sorted list. */
  int sortIndex(const T *thing) const {
    int imax = list.quantity() - 1;
    int imin = 0;
    while (imax > imin) {
      int imid = (imin + imax) / 2; //rounding down, imid might equal imin, imax never will
      T *member = list[imid];
      if (this == member) { //same item stops search, a pathological case
        return ~imid; //a place for a breakpoint.
      }
      int cmp = comparator(thing, member);
      if (cmp == 0) { //we have a hit, it might be in the middle of a stretch of equivalents
        if (laterAfter) { //look for more in upper half
          --cmp;
        } else { //check lower half
          ++cmp;
        }
      }
      if (cmp < 0) {
        // change min index to search upper subarray
        imin = imid + 1;
      } else { // change max index to search lower subarray
        imax = imid - 1;
      }
    }

    if (imax == imin) {
      int cmp = comparator(thing, list[imin]);
      if (cmp == 0) {
        return ~imin; //found a match on final step
      } else if (cmp < 0) { //final entity is 'less than'
        return imin + 1;
      } else {
        return imin; //which is 0 if new thing is below the whole list.
      }
    } else {
      return imin + 1;
    }
  } // sortIndex

  /** @returns @param thing after inserting it into list. This doesn't check if the thing itself is already in the list. Items of equal value to existing items follow
   * the existing item. does NOT insert null pointers into the list. */
  T *insertSorted(T *thing) {
    if (thing == nullptr) {
      return nullptr;
    }
    int location = sortIndex(thing);
    if (location >= 0) {
      return list.insert(thing, location); //#~cast
    } else { //we have a duplicate
      return list.insert(thing, ~location + laterAfter ? 1 : 0); //newer follows older
    }
  } // insertSorted

  /** @returns @param thing after inserting it into list IFFI no equal valued item is in the list.
   * @returns nullptr if given one or an equal valued item is in the list.
   */
  T *insertUnique(T *thing) {
    if (thing == nullptr) {
      return nullptr;
    }
    int location = sortIndex(thing);
    if (location >= 0) { //not in list, returns proper location for it
      return list.insert(thing, location); //#~cast
    } else { //we have a duplicate, unless there is nothing yet present
      if (list.quantity() == 0) {
        return list.insert(thing, 0);
      } else {
        return nullptr;
      }
    }
  } // insertUnique

  /** @returns index of an element upon which the ordinator returns 0.
   * if there is no such element then @returns ~ of where it should be if the ordinator matches this class's comparator.
   * if the ordinator is not compatible with the comparator then the return value when <0 has no meaning other than 'none found'.
   */
  int indexFor(std::function<int(const T *)> ordinator) {
    int top = list.quantity();
    if (top-- == 0) {
      return ~0;
    }
    int bot = 0;
    while (top > bot) {
      int middle = (top + bot) / 2; //rounding down
      int cmp = ordinator(list[middle]);
      if (cmp < 0) { //target is above middle
        bot = middle + 1;
      } else { //target is below OR AT middle
        top = middle;
      }
    }

    int cmp = ordinator(list[bot]); //may rerun last compare
    if (cmp == 0) {
      return bot; //found a match on final step
    } else if (cmp < 0) { //final entity is 'less than'
      return ~(bot + 1);
    } else {
      return ~bot;
    }
  } // indexFor

  /** physically reorganize the wrapped chain to be sorted by the builtin rule.
   *  @returns the number of relocations needed to do that, 0 if chain was already sorted.*/
  int resort() {
    //bubble sort, presume chain is mostly sorted with just a few manually screwed up entries.
    int swaps(0); //diagnostic counter
    int top(list.quantity()); //FUE
    for (int i = top; i-- > 0;) { //n-1 steps for n items, a single item is inherently sorted.
      T *thing = list[i];
      int cmp = comparator(thing, list[i + 1]); //list[above i] are sorted by now
      if (cmp > 0) { //must move it up
        ++swaps;
        //use linear search for now, factor out binary search later.
        for (int j = i; ++j < top;) { //scan upwards as we are optimizing for only slightly disordered.
          cmp = comparator(thing, list[j]);
          if (cmp <= 0) { //we've found our spot
            list.relocate(i, j);
          }
        }
      }
    }
    return swaps;
  } // resort
}; // class ChainSorter
