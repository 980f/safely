#ifndef STOREDLOGGERMANAGER_H
#define STOREDLOGGERMANAGER_H

#include <logger.h>
#include <stored.h>
#include <storedgroup.h>
#include <storednumeric.h>
#include"storable.h"

/** controls state of each logging stream. This will dominate over values set in Logger() constructor. */
class StoredLoggerManager: public StoredGroup<StoredBoolean>,Logger::Manager {
public:
  StoredLoggerManager(Storable &node);
  void onCreation(Logger &logger);
  void onDestruction(Logger &logger);
  /** loggers have names, set control flag for one by name.
 This can be called before the logger itself actually exists. */
  static void Set(Cstr name, bool enable);
};

#endif // STOREDLOGGERMANAGER_H
