#ifndef _SMARTPTR_H_
#define _SMARTPTR_H_

#include <list>
#include <glibmm/object.h>

/** a smart pointer wrapper for glib objects */
template< class T > class SmartPtr
{
  T *ptr;
  static std::list< T* > deferred_unref;

  void do_unrefs() {
    static bool unreffing;

    if(unreffing) {//protects against recursive call due to tmp->unreference, but isn't atomic enough for real threads.
      return;
    }
    unreffing = true;
    while(!deferred_unref.empty()) {
      // Breadth first
      T *tmp = deferred_unref.front();
      deferred_unref.pop_front();
      // Depth first
      // T *tmp = deferred_unref.back();
      // deferred_unref.pop_back();
      tmp->unreference();
    }
    unreffing = false;
  }

public:
  SmartPtr():
    ptr(0) {
  }

  // Presumably the ptr was just created and has a refcount of 1
  explicit SmartPtr(T *ptr):
    ptr(ptr) {
  }

  SmartPtr(const SmartPtr< T > &other):
    ptr(other.ptr) {
    if(ptr) {
      ptr->reference();
    }
  }

  template< class T_CastFrom >
  SmartPtr(const SmartPtr< T_CastFrom > &other):
    ptr(other.operator->()) {
    if(ptr) {
      ptr->reference();
    }
  }

  ~SmartPtr() {
    if(ptr) {
      deferred_unref.push_back(ptr);
      do_unrefs();
    }
  }

  void swap(SmartPtr< T > &other) {
    T *const saveptr = ptr;
    ptr = other.ptr;
    other.ptr = saveptr;
  }

  void reset() {
    SmartPtr< T > empty;
    this->swap(empty);
  }

  SmartPtr< T > &operator=(const SmartPtr< T > &other) {
    SmartPtr< T > temp(other);
    this->swap(temp);
    return *this;
  }

  template< class T_CastFrom >
  SmartPtr< T > &operator=(const SmartPtr< T_CastFrom > &other) {
    SmartPtr< T > temp(other);
    this->swap(temp);
    return *this;
  }

  T* operator->() const {
    return ptr;
  }

  operator T*() const {
    return ptr;
  }

  template< class T_CastFrom >
  static SmartPtr< T > cast_dynamic(const SmartPtr< T_CastFrom > &other) {
    T *const other_ptr = dynamic_cast< T* >(other.operator->());
    if(other_ptr) {
      other_ptr->reference();
    }
    return SmartPtr< T >(other_ptr);
  }

  template< class T_CastFrom >
  static SmartPtr< T > cast_static(const SmartPtr< T_CastFrom > &other) {
    T *const other_ptr = static_cast< T* >(other.operator->());
    if(other_ptr) {
      other_ptr->reference();
    }
    return SmartPtr< T >(other_ptr);
  }

  template< class T_CastFrom >
  static SmartPtr< T > cast_const(const SmartPtr< T_CastFrom > &other) {
    T *const other_ptr = const_cast< T* >(other.operator->());
    if(other_ptr) {
      other_ptr->reference();
    }
    return SmartPtr< T >(other_ptr);
  }
};

template< class T >
std::list< T* > SmartPtr< T >::deferred_unref;

namespace Glib {

/** Partial specialization for SmartPtr<> to Glib::Object.
 * @ingroup glibmmValue
 */
template< class T >
class Value< SmartPtr< T > >: public ValueBase_Object
{
public:
  static GType value_type() {
    return T::get_base_type();
  }

  void set(const SmartPtr< T >& data) {
    set_object(data.operator->());
  }

  SmartPtr< T > get() const {
    Glib::RefPtr< Glib::ObjectBase > obj = get_object_copy();
    obj.operator->()->reference();
    SmartPtr< Glib::ObjectBase > smart_obj(obj.operator->());
    return SmartPtr< T >::cast_dynamic(smart_obj);
  }
};

/** Partial specialization for SmartPtr<> to const Glib::Object.
 * @ingroup glibmmValue
 */
template< class T >
class Value< SmartPtr< const T > >: public ValueBase_Object
{
public:
  static GType value_type() {
    return T::get_base_type();
  }

  void set(const SmartPtr< const T >& data) {
    set_object(const_cast< T* >(data.operator->()));
  }

  SmartPtr< const T > get() const {
    Glib::RefPtr< Glib::ObjectBase > obj = get_object_copy();
    obj.operator->()->reference();
    SmartPtr< Glib::ObjectBase > smart_obj(obj.operator->());
    return SmartPtr< T >::cast_dynamic(smart_obj);
  }
};

}

#endif // _SMARTPTR_H_
