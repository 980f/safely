#ifndef MAPITERATOR_H
#define MAPITERATOR_H


#include <map>
/** java like iterator for just the values of a map */
template <typename KeyType, typename ValueType>
class MapValueIterator {
  typedef typename std::map<KeyType, ValueType> CollectionType;
  CollectionType &collection;//cached for removes

  typedef typename CollectionType::iterator iterator;
  iterator arful;
  iterator end;//cached for performance

public:
  MapValueIterator (CollectionType &collection):
    collection(collection),
    arful(collection.begin()),
    end(collection.end()){
    //#nada
  }

  bool hasNext(){
    return arful!=end;
  }

  ValueType next(){
    return (arful++)->second;
  }

  ValueType value(){
    return arful->second;
  }

  /** remove present item ( @see call value() ) and leaves iterator pointing to what was the next before the removal*/
  void remove(){
    collection.erase(arful);
  }

  void operator ++(){
    ++arful;
  }

};

#endif // MAPITERATOR_H
