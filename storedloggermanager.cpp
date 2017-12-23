#include "storedloggermanager.h"

StoredLoggerManager::StoredLoggerManager(Storable &node):StoredGroup<StoredBoolean> (node){
  //last in wins, prior ones leak.
  //todo:1 message wtf if prior is not null
  Logger::manager=this;
}

void StoredLoggerManager::onCreation(Logger &logger){
  dbg("Logger %s defaulted %sabled",logger.prefix,logger.enabled?"en":"dis");
  bool existed=false;
  StoredBoolean &controller= StoredGroup::child(logger.prefix,&existed);
  if(!existed){//program defaults init file, but file is supreme.
    controller=logger.enabled;
  }
  controller.onChangeUpdate(logger.enabled);
  dbg("Logger %s initially is %sabled",logger.prefix,logger.enabled?"en":"dis");
}

void StoredLoggerManager::onDestruction(Logger &logger){
  dbg("Logger %s at exit was %sabled",logger.prefix,logger.enabled?"en":"dis");
  logger.enabled=false;//reduce impact on use after free.
  //sigctracker should take care of deregistering the updater.
  //we retain the setting for file storage.
}

