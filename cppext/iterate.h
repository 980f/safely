#ifndef _ITERATE_H_
#define _ITERATE_H_

//@deprecated  these were used to port in some code that misused stl iterators, these marked usages that needed manual inspection when there was less project pressure.

/** these macro's go together, the first saves the begin so that the distance can be later computed.
the caching of  (collection).end() violates the iteration validity, it only works if the collection is not added to or removed from during the iteration.
*/

#define ITERATE(iterator_type, iterator, collection) \
  for(iterator_type _ ## iterator ## _begin = (collection).begin(), _ ## iterator ## _end = (collection).end(), iterator = _ ## iterator ## _begin;\
      iterator != _ ## iterator ## _end; ++iterator)

#define INDEX(iterator) (std::distance(_ ## iterator ## _begin, iterator))

#endif /* _ITERATE_H_ */
