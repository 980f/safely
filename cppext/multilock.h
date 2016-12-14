#ifndef MULTILOCK_H
#define MULTILOCK_H

/**
when many agents wish to lock a resource, and each must remove its own lock.
While this could be packed into a base class with an abstract resource acquire and release let us keep this version as simple classes (no virtual table).

This needs some rework to be a factory of MultiLocker's so that the release works correctly.
*/
class MultiLock {
  /** counted mutexish thing */
  int locked;
public:
  MultiLock();
  /** @returns whether just locked */
  bool lock();
  /** @returns whether just freed */
  bool unlock();
  /** calls either lock or unlock according to @param lockit. suitable for a sigc::slot. */
  void setlock(bool lockit);
  /** @returns whether locked more times than unlocked.*/
  bool isLocked()const;
  //  /** @returns how locked it was, that many subordinate MultiLockers will be invalid. */
  //  int release();
};

/** useful for locking a MultiLock for the lifetime of an object */
class AutoMultiLock {
  MultiLock &resource;
public:
  AutoMultiLock(MultiLock&lock);
  ~AutoMultiLock();
};

class MultiLocker {
  MultiLock &resource;
  int locked;
public:
  MultiLocker(MultiLock&lock);
  ~MultiLocker();
  /** @returns whether just locked */
  bool lock();
  /** @returns whether just freed */
  bool unlock();
  /** calls either lock or unlock according to @param lockit. suitable for a sigc::slot. */
  void setlock(bool lockit);
  /** cute syntax for setlock */
  void operator =(bool lockit);
  /** @returns whether this lock is locked more times than unlocked.*/
  bool isLocked()const;
  /** @returns whether the locked thing is locked regardless of whether this locker is possibly why*/
  bool resourceLocked()const;
};

#endif // MULTILOCK_H
