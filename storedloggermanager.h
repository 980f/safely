#ifndef STOREDLOGGERMANAGER_H
#define STOREDLOGGERMANAGER_H

#include <logger.h>
#include <stored.h>
#include <storedgroup.h>
#include <storednumeric.h>
#include"storable.h"

class StoredLoggerManager: public StoredGroup<StoredBoolean>,Logger::Manager {
public:
  StoredLoggerManager(Storable &node);
  void onCreation(Logger &logger);
  void onDestruction(Logger &logger);
};

#endif // STOREDLOGGERMANAGER_H
