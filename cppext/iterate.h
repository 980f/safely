#ifndef _ITERATE_H_
#define _ITERATE_H_

#include <vector>
#include <functional>
template <typename V> class VectorIterator {
  std::vector<V> &v;
public:
  VectorIterator(std::vector<V> &v):v(v){
    //#done
  }
  void foreach (std::function<void(V&)> body) {
    for(auto it=v.begin();it!=v.end();++it){
      body(*it);
    }
  }
};

/** these deprecated macro's go together, the first saves the begin so that the distance can be later computed.
the caching of  (collection).end() violates the iteration validity, it only works if the collection is not added to or removed from during the iteration.
*/

#define ITERATE(iterator_type, iterator, collection) \
  for(iterator_type _ ## iterator ## _begin = (collection).begin(), _ ## iterator ## _end = (collection).end(), iterator = _ ## iterator ## _begin;\
      iterator != _ ## iterator ## _end; ++iterator)

#define INDEX(iterator) (std::distance(_ ## iterator ## _begin, iterator))

#endif /* _ITERATE_H_ */
